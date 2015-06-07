#include <pangolin/pangolin.h>
#include <pangolin/gl.h>
#include <pangolin/timer.h>

#include <fstream>
#include <iostream>
#include <limits>

#include <assert.h>
#include <sys/stat.h>

#include <Eigen/Dense>
#include <caffe/caffe.hpp>

#include <vector_types.h>
#include <vector_functions.h>
#include <helper_math.h>

#include "feature_projector.h"
#include "mnist_io.h"
#include "fonts/font_manager.h"
#include "mouse_handlers/embedding_view_mouse_handler.h"
#include "mouse_handlers/filter_view_mouse_handler.h"
#include "mouse_handlers/multi_embedding_view_mouse_handler.h"
#include "mouse_handlers/tool_view_mouse_handler.h"
#include "util/gl_helpers.h"
#include "util/image_io.h"
#include "util/string_format.h"
#include "visualizations/single_embedding_viz.h"
#include "visualizations/filter_response_viz.h"
#include "visualizations/multi_embedding_viz.h"

static const int guiWidth = 1920;
static const int guiHeight = 1080;
static const int panelWidth = 274;
static const float aspectRatio = guiWidth/(float)guiHeight;

static const int embeddingViewWidth = guiHeight;
static const int embeddingViewHeight = guiHeight;
static const float embeddingViewAspectRatio = embeddingViewWidth/(float)embeddingViewHeight;

static const int filterViewWidth = guiWidth-embeddingViewWidth;
static const int filterViewHeight = guiHeight;
static const float filterViewAspectRatio = filterViewWidth/(float)filterViewHeight;

static const std::string weightFile = "/home/tws10/Development/caffe/examples/siamese/mnist_siamese_iter_50000.caffemodel";
static const std::string netFile = "../networks/mnist_siamese.prototxt";

static const uchar3 digitColors[10] = {
    make_uchar3(166,206,227 ),
    make_uchar3(31,120,180  ),
    make_uchar3(178,223,138 ),
    make_uchar3(51,160,44   ),
    make_uchar3(251,154,153 ),
    make_uchar3(227,26,28   ),
    make_uchar3(253,191,111 ),
    make_uchar3(255,127,0   ),
    make_uchar3(202,178,214 ),
    make_uchar3(106,61,154  )
};

inline void printBlobSize(boost::shared_ptr<caffe::Blob<float> > blob) {
    std::cout << blob->num() << " x " << blob->channels() << " x " << blob->height() << " x " << blob->width() << std::endl;
}

inline bool fileExists(std::string filename) {
    struct stat buffer;
    return stat(filename.c_str(), &buffer) == 0;
}

int main(int argc, char * * argv) {

    // -=-=-=-=- set up caffe -=-=-=-=-
    caffe::GlobalInit(&argc,&argv);

#ifndef CPU_ONLY
    caffe::Caffe::SetDevice(0);
    caffe::Caffe::set_mode(caffe::Caffe::GPU);
#endif // CPU_ONLY

    // -=-=-=-=- load learned network -=-=-=-=-
    caffe::Net<float> net(netFile,caffe::TEST);
    net.CopyTrainedLayersFrom(weightFile);

    // -=-=-=-=- load mnist test data -=-=-=-=-
    int nTestImages, imageWidth, imageHeight;
    float * testImages = loadMNISTImages(mnistTestImageFile,nTestImages, imageWidth, imageHeight);
    std::vector<unsigned char> testLabels;
    loadMNISTLabels(mnistTestLabelFile,testLabels);
    assert(nTestImages == testLabels.size());
    std::vector<uchar3> testColors(nTestImages);
    for (int i=0; i<nTestImages; ++i) { testColors[i] = digitColors[testLabels[i]]; }

    std::vector<float> selection(nTestImages,0.5f);

    // -=-=-=-=- process test data -=-=-=-=-
    boost::shared_ptr<caffe::Blob<float> > inputBlob = net.blobs()[net.input_blob_indices()[0]];
    assert(inputBlob->num() == nTestImages);
    memcpy(inputBlob->mutable_cpu_data(),testImages,inputBlob->count()*sizeof(float));
    net.ForwardPrefilled();

    // -=-=-=-=- set up pangolin -=-=-=-=-
    pangolin::CreateGlutWindowAndBind("Seein' In", guiWidth, guiHeight,GLUT_MULTISAMPLE | GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glewInit();

    pangolin::GlTexture imageTex(imageWidth,imageHeight);
    imageTex.SetNearestNeighbour();

    const int overviewWidth = 256;
    const int overviewHeight = overviewWidth*embeddingViewAspectRatio;
    pangolin::GlTexture overviewTex(overviewWidth,overviewHeight);

    // -=-=-=-=- set up point shader -=-=-=-=-
    int maxTexBufferSize;
    glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE,&maxTexBufferSize);
    std::cout << maxTexBufferSize << std::endl;

    boost::shared_ptr<caffe::Blob<float> > outputBlob = net.blobs()[net.output_blob_indices()[0]];
    ScatterPlotShader pointShader(10000,10,outputBlob->cpu_data());

    // -=-=-=-=- set up visualizations -=-=-=-=-
    SingleEmbeddingViz embeddingViz(embeddingViewAspectRatio,testImages,imageWidth,imageHeight,imageTex,overviewWidth,overviewHeight,overviewTex,pointShader,selection.data());
    embeddingViz.setEmbedding((const float2 *)outputBlob->cpu_data(),testColors.data(),nTestImages);

    MultiEmbeddingViz multiEmbeddingViz(embeddingViewAspectRatio,testImages,imageWidth,imageHeight,imageTex,overviewWidth,overviewHeight,overviewTex,pointShader,selection.data());

    // -=-=-=-=- set up mouse handlers -=-=-=-=-
    EmbeddingViewMouseHandler embeddingViewHandler(&embeddingViz);
    MultiEmbeddingViewMouseHandler multiEmbeddingViewHandler(&multiEmbeddingViz);

    // -=-=-=-=- set up views -=-=-=-=-
    pangolin::View embeddingView(embeddingViewAspectRatio);
    embeddingView.SetHandler(&embeddingViewHandler);
//    embeddingView.SetBounds(pangolin::Attach::Pixel(0),pangolin::Attach::ReversePix(0),pangolin::Attach::Pix(0),pangolin::Attach::Frac(embeddingViewWidth/(float)guiWidth),true);
    embeddingView.SetLock(pangolin::LockLeft,pangolin::LockCenter);


    pangolin::View filterView; //(filterViewAspectRatio);

    pangolin::View toolView;
    toolView.SetBounds(0,1,0,pangolin::Attach::Pix(panelWidth));
    pangolin::GlTexture pointSelection(64,64);
    pangolin::GlTexture lassoSelection(64,64);
    pointSelection.LoadFromFile("../src/icons/pointSelection.png");
    lassoSelection.LoadFromFile("../src/icons/lassoSelection.png");
    ToolViewMouseHandler toolViewHandler(2,2,1);
    toolView.SetHandler(&toolViewHandler);

//    filterView.SetBounds(pangolin::Attach::Frac(0),pangolin::Attach::Frac(1),pangolin::Attach::Frac(embeddingViewWidth/(float)guiWidth),pangolin::Attach::Frac(1),true);

    filterView.SetLock(pangolin::LockRight,pangolin::LockCenter);

//    pangolin::CreatePanel("panel").SetBounds(0.0, 1.0, 0.0, pangolin::Attach::Pix(panelWidth));

    pangolin::Display("display")
            .SetBounds(0.0,1.0,0.0,1.0)
//            .SetBounds(1.0,0.0,1.0,pangolin::Attach::Pix(panelWidth))
            .AddDisplay(toolView)
            .AddDisplay(embeddingView)
            .AddDisplay(filterView)
            .SetLayout(pangolin::LayoutHorizontal);

    std::cout << embeddingViewAspectRatio << std::endl;
    std::cout << filterViewAspectRatio << std::endl;
    std::cout << embeddingView.GetBounds().w << " x " << embeddingView.GetBounds().h << std::endl;
    std::cout << filterView.GetBounds().w << " x " << filterView.GetBounds().h << std::endl;

    bool hasSelection = false;


    // -=-=-=-=- set up font management -=-=-=-=-
    FontManager fontManager("GaramondNo8");

    // -=-=-=-=- set up layer visualizations -=-=-=-=-
    std::vector<std::string> filterResponsesToVisualize;
    filterResponsesToVisualize.push_back("data");
    filterResponsesToVisualize.push_back("conv1");
    filterResponsesToVisualize.push_back("pool1");
    filterResponsesToVisualize.push_back("conv2");
    filterResponsesToVisualize.push_back("pool2");
    filterResponsesToVisualize.push_back("ip1");
    filterResponsesToVisualize.push_back("ip2");
    filterResponsesToVisualize.push_back("feat");

    std::map<std::string,int> layerRelativeScales;
    layerRelativeScales["data"] = 1;
    layerRelativeScales["conv1"] = 1;
    layerRelativeScales["pool1"] = 2;
    layerRelativeScales["conv2"] = 2;
    layerRelativeScales["pool2"] = 4;
    layerRelativeScales["ip1"] = 4;
    layerRelativeScales["ip2"] = 4;
    layerRelativeScales["feat"] = 4;

    std::map<std::string,int2> layerReceptiveFields;
    layerReceptiveFields["data"] = make_int2(1,1);
    layerReceptiveFields["conv1"] = make_int2(5,5);
    layerReceptiveFields["pool1"] = make_int2(6,6);
    layerReceptiveFields["conv2"] = make_int2(14,14);
    layerReceptiveFields["pool2"] = make_int2(16,16);
    layerReceptiveFields["ip1"] = make_int2(28,28);
    layerReceptiveFields["ip2"] = make_int2(28,28);
    layerReceptiveFields["feat"] = make_int2(28,28);

//    std::map<std::string,pangolin::GlTexture*> layerResponseTextures;
    float filterVizZoom = 2.f;
    FilterResponseViz filterResponseViz(net,filterResponsesToVisualize,
                                        layerRelativeScales,filterView.GetBounds().w,filterView.GetBounds().h,fontManager,
                                        filterVizZoom,16);

    FilterViewMouseHandler filterViewHandler(&filterResponseViz);
    filterView.SetHandler(&filterViewHandler);

    FeatureProjector featProjector(net,filterResponsesToVisualize); //"feat");

//    pangolin::RegisterKeyPressCallback(' ',[&embeddingViewHandler, &hasSelection](){ embeddingViewHandler.setSelectionMode(embeddingViewHandler.getSelectionMode() == SelectionModeSingle ? SelectionModeLasso : SelectionModeSingle); hasSelection = false;} );
    pangolin::RegisterKeyPressCallback('+',[&filterResponseViz, &filterVizZoom, &filterView](){
        filterVizZoom *= 1.25;
        filterResponseViz.resize(filterView.GetBounds().w,filterView.GetBounds().h,filterVizZoom);
    });
    pangolin::RegisterKeyPressCallback('-',[&filterResponseViz, &filterVizZoom, &filterView](){
        filterVizZoom /= 1.25;
        filterResponseViz.resize(filterView.GetBounds().w,filterView.GetBounds().h,filterVizZoom);
    });


    glEnable(GL_PROGRAM_POINT_SIZE);

    // -=-=-=-=- render previews -=-=-=-=-
    std::vector<uchar3*> overviewImages;
    {
        pangolin::GlRenderBuffer overviewRenderBuffer(overviewWidth,overviewHeight);
        pangolin::GlFramebuffer overviewFrameBuffer(overviewTex,overviewRenderBuffer);

        overviewFrameBuffer.Bind();
        glViewport(0,0,overviewWidth,overviewHeight);
        glEnable(GL_SCISSOR_TEST);
        glScissor(0,0,overviewWidth,overviewHeight);
        glClearColor(1,1,1,1);

        for (int i=0; i<filterResponsesToVisualize.size(); ++i) {
            std::string blobName = filterResponsesToVisualize[i];
            std::string cachedFilename = stringFormat("/tmp/seeinIn.overview%02d.png",i);
            if (fileExists(cachedFilename)) {

                std::cout << "loading cached preview for " << blobName << std::endl;
                int width, height, channels;
                unsigned char * image = readPNG(cachedFilename.c_str(),width,height,channels);
                if (width == overviewWidth && height == overviewHeight && channels == 3) {
                    overviewImages.push_back((uchar3 *)image);
                } else {
                    delete [] image;
                }

            }

            if (overviewImages.size() != (i+1) ){

                std::cout << "making preview for " << blobName << std::endl;
                boost::shared_ptr<caffe::Blob<float> > embeddingBlob = net.blob_by_name(blobName);
                const int dims = embeddingBlob->channels();
                EmbeddingViz * viz;
                if (dims == 2) {
                    embeddingViz.setEmbedding((const float2 *)embeddingBlob->cpu_data(),testColors.data(),nTestImages);
                    viz = &embeddingViz;
                } else {
                    if (embeddingBlob->width() == 1 && embeddingBlob->height() == 1) {
                        multiEmbeddingViz.setEmbedding(embeddingBlob->cpu_data(),dims,
                                                       testColors.data(),nTestImages);
                    } else {
                        multiEmbeddingViz.setEmbedding(embeddingBlob->cpu_data(),dims,
                                                       testColors.data(),nTestImages,
                                                       embeddingBlob->width(),embeddingBlob->height(),
                                                       layerReceptiveFields[blobName],
                                                       layerRelativeScales[blobName]);
                    }

                    viz = &multiEmbeddingViz;
                }
                viz->setZoomOverridingLimits(1.f);
                //viz->setZoom(1.f);

                glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glOrtho(0, overviewWidth, 0, overviewHeight, -1, 1);
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
                viz->render(make_float2(overviewWidth,overviewHeight));
                overviewFrameBuffer.Unbind();
                overviewImages.push_back(new uchar3[overviewWidth*overviewHeight]);
                overviewTex.Download(overviewImages.back(),GL_RGB,GL_UNSIGNED_BYTE);

                writePNG(cachedFilename.c_str(),overviewImages.back(),overviewWidth,overviewHeight);

                overviewFrameBuffer.Bind();
            }
        }

        overviewFrameBuffer.Unbind();

    }
    multiEmbeddingViz.setShowOverview(true);
    embeddingViz.setShowOverview(true);
    embeddingViz.setOverviewImage(overviewImages.back());

    bool multiembeddingVizActive = false;

    for (long frame=1; !pangolin::ShouldQuit(); ++frame) {

        CheckGlDieOnError();

//        static pangolin::basetime lastTime = pangolin::TimeNow();
//        pangolin::basetime timeNow = pangolin::TimeNow();
//        std::cout << pangolin::TimeDiff_s(lastTime,timeNow) << std::endl;
//        lastTime = pangolin::TimeNow();

        if (pangolin::HasResized()) {
            pangolin::DisplayBase().ActivateScissorAndClear();
            pangolin::DisplayBase().ResizeChildren();

            filterResponseViz.resize(filterView.GetBounds().w,filterView.GetBounds().h,filterVizZoom);
        }

        CheckGlDieOnError();

        glClearColor(1,1,1,1);

        // -=-=-=-=-=-=- tool view -=-=-=-=-=-=-
        toolView.ActivateScissorAndClear();
        toolView.ActivatePixelOrthographic();
        glColor3ub(255,255,255);
        renderTexture(pointSelection,make_float2(0,toolView.GetBounds().t() - 34),make_float2(32,32));
        renderTexture(lassoSelection,make_float2(34,toolView.GetBounds().t() - 34),make_float2(32,32));

        // -=-=-=-=-=-=- embedding view -=-=-=-=-=-=-
        embeddingView.ActivateScissorAndClear();
        embeddingView.ActivatePixelOrthographic();

        CheckGlDieOnError();

        const float2 windowSize = make_float2(embeddingView.GetBounds().w,embeddingView.GetBounds().h);
        if (multiembeddingVizActive) {
            multiEmbeddingViz.render(windowSize);

            CheckGlDieOnError();
        } else {
            embeddingViz.render(windowSize);

            CheckGlDieOnError();
    //        multiEmbeddingViz.render(embeddingView);

//            glColor3ub(255,255,255);
//            renderTexture(previewTex,make_float2(0),make_float2(previewWidth,previewHeight),false);

            glPushMatrix();
            setUpViewport(windowSize,embeddingViz.getViewportSize(),embeddingViz.getViewportCenter());

    //        {
    //            glColor3ub(0,0,0);
    //            glLineWidth(3);
    //            glBegin(GL_LINE_LOOP);
    //            glVertex2f(viewportCenter.x - viewportSize.x/2 + 1e-4, viewportCenter.y - viewportSize.y/2 + 1e-4);
    //            glVertex2f(viewportCenter.x - viewportSize.x/2 + 1e-4, viewportCenter.y + viewportSize.y/2 - 1e-4);
    //            glVertex2f(viewportCenter.x + viewportSize.x/2 - 1e-4, viewportCenter.y + viewportSize.y/2 - 1e-4);
    //            glVertex2f(viewportCenter.x + viewportSize.x/2 - 1e-4, viewportCenter.y - viewportSize.y/2 + 1e-4);
    //            glEnd();
    //            glLineWidth(1);
    //        }

            switch (embeddingViewHandler.getSelectionMode()) {
            case SelectionModeSingle:
            {

            } break;
            case SelectionModeLasso:
            {
                std::vector<float2> lassoPoints = embeddingViewHandler.getLassoPoints();
                glColor3ub(0,0,0);
                glLineWidth(2);
                glBegin(GL_LINE_STRIP);
                for (float2 v : lassoPoints) {
                    glVertex(v);
                }
                glEnd();
                glLineWidth(1);
            } break;
            }

    ////        if (selectedImage >= 0) {
    ////            boost::shared_ptr<caffe::Layer<float> > featLayer = net.layer_by_name("feat");

    ////            boost::shared_ptr<caffe::Blob<float> > weightBlob = featLayer->blobs()[0];
    ////            boost::shared_ptr<caffe::Blob<float> > biasBlob = featLayer->blobs()[1];

    ////            boost::shared_ptr<caffe::Blob<float> > featInputBlob = net.blob_by_name("ip2");

    ////            printBlobSize(weightBlob);
    ////            printBlobSize(biasBlob);
    ////            printBlobSize(featInputBlob);

    ////            glLineWidth(2);
    ////            glColor3ub(0,0,0);
    ////            float2 end = make_float2(biasBlob->cpu_data()[0],biasBlob->cpu_data()[1]);
    ////            glBegin(GL_LINES);
    ////            for (int i=0; i<weightBlob->channels(); ++i) {
    ////                float2 W = make_float2(weightBlob->cpu_data()[i],weightBlob->cpu_data()[weightBlob->channels() + i]);
    ////                float x = featInputBlob->cpu_data()[selectedImage*featInputBlob->channels() + i];
    ////                float2 nextEnd = end + x*W;
    ////                glVertex(end);
    ////                glVertex(nextEnd);
    ////                end = nextEnd;
    ////            }
    ////            glEnd();
    ////            glLineWidth(1);
    ////        }

            glPopMatrix();


            CheckGlDieOnError();
        }

        // -=-=-=-=-=-=- filter view -=-=-=-=-=-=-
        filterView.ActivateScissorAndClear();
        filterView.ActivatePixelOrthographic();


        CheckGlDieOnError();
//        {
//            glColor3ub(0,0,0);
//            glLineWidth(3);
//            glBegin(GL_LINE_LOOP);
//            glVertex2f(1e-4, 1e-4);
//            glVertex2f(1e-4, filterView.GetBounds().h - 1e-4);
//            glVertex2f(filterView.GetBounds().w - 1e-4, filterView.GetBounds().h - 1e-4);
//            glVertex2f(filterView.GetBounds().w - 1e-4, 1e-4);
//            glEnd();
//            glLineWidth(1);
//        }

        if (hasSelection) {
            filterResponseViz.render();

            CheckGlDieOnError();
        }

        // -=-=-=-=-=-=- input handling -=-=-=-=-=-=-
        int selectedImage = -1;
        if (embeddingViewHandler.hasSelection()) {
            switch (embeddingViewHandler.getSelectionMode()) {
                case SelectionModeSingle:
                    {
                        selectedImage = embeddingViewHandler.getHoveredOverPoint();
                        if (selectedImage == -1) {
                            std::fill(selection.begin(),selection.end(),0.5f);
                        }
                    } break;
                case SelectionModeLasso:
                    {
                        std::vector<float2> & lassoPoints = embeddingViewHandler.getLassoPoints();
//                        embeddingViz
                        std::vector<int> enclosedPoints;
                        embeddingViz.getEnclosedPoints(enclosedPoints,lassoPoints);
                        std::cout << enclosedPoints.size() << " points enclosed" << std::endl;
                        std::fill(selection.begin(),selection.end(),0.0f);
                        for (int i=0; i<enclosedPoints.size(); ++i) {
                            selection[enclosedPoints[i]] = 1.f;
                        }
                        filterResponseViz.setSelection(selection);
                        embeddingViewHandler.clearLassoPoints();
                        hasSelection = true;
                    } break;
            }
        } else if (multiEmbeddingViewHandler.hasSelection()) {
            selectedImage = multiEmbeddingViewHandler.getHoveredOverPoint();
            if (selectedImage == -1) {
                std::fill(selection.begin(),selection.end(),0.5f);
            }
        }
        if (selectedImage >= 0) {
            filterResponseViz.setSelection(selectedImage);
            hasSelection = true;
            std::memset(selection.data(),0.f,selection.size()*sizeof(float));
            selection[selectedImage] = 1.f;
        }

        if (filterViewHandler.hasLayerSelection()) {
            std::cout << "selected layer " << filterViewHandler.getSelectedLayer() << std::endl;
            const int layerNum = filterViewHandler.getSelectedLayer();
            if (layerNum >= 0) {
                filterResponseViz.setEmbeddingLayer(filterViewHandler.getSelectedLayer());
                std::string blobName = filterResponsesToVisualize[layerNum];
                std::cout << "opening embedding " << blobName << std::endl;
                boost::shared_ptr<caffe::Blob<float> > embeddingBlob = net.blob_by_name(blobName);
                const int dims = embeddingBlob->channels();
                if (dims == 2) {
                    multiembeddingVizActive = false;
                    embeddingView.SetHandler(&embeddingViewHandler);
                    embeddingViz.setEmbedding((const float2 *)embeddingBlob->cpu_data(),testColors.data(),nTestImages);
                    embeddingViz.setOverviewImage(overviewImages[layerNum]);
                } else {
                    multiembeddingVizActive = true;
                    embeddingView.SetHandler(&multiEmbeddingViewHandler);
                    if (embeddingBlob->width() == 1 && embeddingBlob->height() == 1) {
                        multiEmbeddingViz.setEmbedding(embeddingBlob->cpu_data(),dims,
                                                       testColors.data(),nTestImages);
                    } else {
                        multiEmbeddingViz.setEmbedding(embeddingBlob->cpu_data(),dims,
                                                       testColors.data(),nTestImages,
                                                       embeddingBlob->width(),embeddingBlob->height(),
                                                       layerReceptiveFields[blobName],
                                                       layerRelativeScales[blobName]);
                    }
                    multiEmbeddingViz.setZoom(1.f);
                    multiEmbeddingViz.setOverviewImage(overviewImages[layerNum]);
                }
            }
        } else if (filterViewHandler.hasUnitSelection()) {
            std::cout << "selected unit " << filterViewHandler.getSelectedUnit() << " in layer " << filterViewHandler.getSelectedLayer() << std::endl;
            int selectedImage = filterResponseViz.getSelection();
            if (selectedImage >= 0) {
                const int selectedLayerNum = filterViewHandler.getSelectedLayer();
                const int selectedUnit = filterViewHandler.getSelectedUnit();
                const std::string blobName = filterResponsesToVisualize[selectedLayerNum];
                const float activationValue = net.blob_by_name(blobName)->cpu_data()[selectedUnit];
                std::cout << "activated at " << activationValue << std::endl;
                featProjector.computeProjection(blobName,selectedImage,selectedUnit,activationValue);
                std::cout << "stored as " << featProjector.getResponse(blobName)[selectedUnit] << std::endl;
                filterResponseViz.setResponse(featProjector);
            }
        }
        if (toolViewHandler.hasSelection()) {
            int selectedButton = toolViewHandler.getSelectedButton();
            switch (selectedButton) {
                case 0:
                    embeddingViewHandler.setSelectionMode(SelectionModeSingle);
                    break;
                case 1:
                    embeddingViewHandler.setSelectionMode(SelectionModeLasso);
                    break;
            }
        }

        CheckGlDieOnError();

        glClearColor(0,0,0,1);
        pangolin::FinishGlutFrame();

    }

    for (uchar3 * overviewImage : overviewImages) {
        delete [] overviewImage;
    }

    return 0;
}
