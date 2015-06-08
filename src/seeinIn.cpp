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
#include "mouse_handlers/single_embedding_view_mouse_handler.h"
#include "mouse_handlers/filter_view_mouse_handler.h"
#include "mouse_handlers/multi_embedding_view_mouse_handler.h"
#include "mouse_handlers/tool_view_mouse_handler.h"
#include "util/gl_helpers.h"
#include "util/image_io.h"
#include "util/string_format.h"
#include "visualizations/filter_response_viz.h"
#include "visualizations/multi_embedding_viz.h"
#include "visualizations/single_embedding_viz.h"
#include "visualizations/toolbox.h"

static const int guiWidth = 1920;
static const int guiHeight = 1080;
static const int panelWidth = 256;
static const float aspectRatio = guiWidth/(float)guiHeight;

static const int embeddingViewWidth = guiHeight;
static const int embeddingViewHeight = guiHeight;
static const float embeddingViewAspectRatio = embeddingViewWidth/(float)embeddingViewHeight;

static const int filterViewWidth = guiWidth-embeddingViewWidth;
static const int filterViewHeight = guiHeight;
static const float filterViewAspectRatio = filterViewWidth/(float)filterViewHeight;

static const std::string weightFile = "/home/tws10/Development/caffe/examples/siamese/mnist_siamese_iter_50000.caffemodel";
static const std::string netFile = "../networks/mnist_siamese.prototxt";

static const int nClasses = 10;
static const uchar3 digitColors[nClasses] = {
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

static const std::string digitNames[nClasses] = {
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"
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

    // -=-=-=-=- set up font management -=-=-=-=-
    FontManager fontManager("GaramondNo8");

    // -=-=-=-=- set up point shader -=-=-=-=-
    int maxTexBufferSize;
    glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE,&maxTexBufferSize);
    std::cout << maxTexBufferSize << std::endl;

    boost::shared_ptr<caffe::Blob<float> > outputBlob = net.blobs()[net.output_blob_indices()[0]];
    ScatterPlotShader pointShader;

    // -=-=-=-=- set up visualizations -=-=-=-=-
    SingleEmbeddingViz embeddingViz(embeddingViewAspectRatio,testImages,imageWidth,imageHeight,imageTex,overviewWidth,overviewHeight,overviewTex,pointShader,selection.data());
    embeddingViz.setEmbedding((const float2 *)outputBlob->cpu_data(),testColors.data(),nTestImages);

    MultiEmbeddingViz multiEmbeddingViz(embeddingViewAspectRatio,testImages,imageWidth,imageHeight,imageTex,overviewWidth,overviewHeight,overviewTex,pointShader,selection.data());

    // -=-=-=-=- set up mouse handlers -=-=-=-=-
    SingleEmbeddingViewMouseHandler embeddingViewHandler(&embeddingViz);
    MultiEmbeddingViewMouseHandler multiEmbeddingViewHandler(&multiEmbeddingViz);
    EmbeddingViewMouseHandler * activeEmbeddingHandler = &embeddingViewHandler;
    EmbeddingViz * activeEmbeddingViz = &embeddingViz;

    // -=-=-=-=- set up views -=-=-=-=-
    pangolin::View embeddingView(embeddingViewAspectRatio);
    embeddingView.SetHandler(&embeddingViewHandler);
//    embeddingView.SetBounds(pangolin::Attach::Pixel(0),pangolin::Attach::ReversePix(0),pangolin::Attach::Pix(0),pangolin::Attach::Frac(embeddingViewWidth/(float)guiWidth),true);
    embeddingView.SetLock(pangolin::LockLeft,pangolin::LockCenter);

    pangolin::View filterView; //(filterViewAspectRatio);

    pangolin::View toolView;
    toolView.SetBounds(0,1,0,pangolin::Attach::Pix(panelWidth));
    Toolbox toolboxViz(digitColors,digitNames,nClasses,fontManager);
    toolboxViz.setButtonActive(PointSelectionButton,true);

    ToolViewMouseHandler toolViewHandler(&toolboxViz);
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

    // -=-=-=-=- set up gl -=-=-=-=-
    glEnable(GL_PROGRAM_POINT_SIZE);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

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
        toolboxViz.render(make_float2(toolView.GetBounds().w,toolView.GetBounds().h));

        // -=-=-=-=-=-=- embedding view -=-=-=-=-=-=-
        embeddingView.ActivateScissorAndClear();
        embeddingView.ActivatePixelOrthographic();

        CheckGlDieOnError();

        const float2 windowSize = make_float2(embeddingView.GetBounds().w,embeddingView.GetBounds().h);
        activeEmbeddingViz->render(windowSize);
        CheckGlDieOnError();
        //        multiEmbeddingViz.render(embeddingView);

        switch (activeEmbeddingHandler->getSelectionMode()) {
            case SelectionModeSingle:
                {

                } break;
            case SelectionModeLasso:
                {
                    std::vector<float2> lassoPoints = activeEmbeddingHandler->getLassoPoints();
                    if (lassoPoints.size() > 0) {
                        glPushMatrix();
                        setUpViewport(windowSize,activeEmbeddingViz->getViewportSize(),activeEmbeddingViz->getViewportCenter());

                        glColor3ub(0,0,0);
                        glLineWidth(2);
                        glEnableClientState(GL_VERTEX_ARRAY);
                        glVertexPointer(2,GL_FLOAT,0,lassoPoints.data());
                        glDrawArrays(GL_LINE_STRIP,0,lassoPoints.size());
                        glDisableClientState(GL_VERTEX_ARRAY);
                        glLineWidth(1);

                        glPopMatrix();
                    }
                } break;
        }


        CheckGlDieOnError();

        // -=-=-=-=-=-=- filter view -=-=-=-=-=-=-
        filterView.ActivateScissorAndClear();
        filterView.ActivatePixelOrthographic();

        CheckGlDieOnError();

        if (hasSelection) {
            filterResponseViz.render();
            CheckGlDieOnError();
        }

        // -=-=-=-=-=-=- input handling -=-=-=-=-=-=-
        int selectedImage = -1;
        if (activeEmbeddingHandler->hasSelection()) {
            switch (activeEmbeddingHandler->getSelectionMode()) {
                case SelectionModeSingle:
                    {
                        selectedImage = activeEmbeddingHandler->getHoveredOverPoint();
                        if (selectedImage == -1) {
                            std::fill(selection.begin(),selection.end(),0.5f);
                        }
                    } break;
                case SelectionModeLasso:
                    {
                        std::vector<float2> & lassoPoints = activeEmbeddingHandler->getLassoPoints();
                        std::vector<int> enclosedPoints;
                        activeEmbeddingViz->getEnclosedPoints(enclosedPoints,lassoPoints);
                        std::cout << enclosedPoints.size() << " points enclosed" << std::endl;
                        std::fill(selection.begin(),selection.end(),0.0f);
                        for (int i=0; i<enclosedPoints.size(); ++i) {
                            selection[enclosedPoints[i]] = 1.f;
                        }
                        filterResponseViz.setSelection(selection);
                        activeEmbeddingHandler->clearLassoPoints();
                        hasSelection = true;
                    } break;
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
                    activeEmbeddingHandler = &embeddingViewHandler;
                    activeEmbeddingViz = &embeddingViz;
                    embeddingViz.setEmbedding((const float2 *)embeddingBlob->cpu_data(),testColors.data(),nTestImages);
                } else {
                    multiembeddingVizActive = true;
                    activeEmbeddingHandler = &multiEmbeddingViewHandler;
                    activeEmbeddingViz = &multiEmbeddingViz;
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
                }
                embeddingView.SetHandler(activeEmbeddingHandler);
                activeEmbeddingViz->setOverviewImage(overviewImages[layerNum]);

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
        if (toolViewHandler.hasButtonSelection()) {
            ToolboxButton selectedButton = (ToolboxButton)toolViewHandler.getSelectedButton();
            switch (selectedButton) {
                case PointSelectionButton:
                    embeddingViewHandler.setSelectionMode(SelectionModeSingle);
                    multiEmbeddingViewHandler.setSelectionMode(SelectionModeSingle);
                    activeEmbeddingHandler->clearLassoPoints();
                    toolboxViz.setButtonActive(PointSelectionButton,true);
                    toolboxViz.setButtonActive(LassoSelectionButton,false);
                    break;
                case LassoSelectionButton:
                    embeddingViewHandler.setSelectionMode(SelectionModeLasso);
                    multiEmbeddingViewHandler.setSelectionMode(SelectionModeLasso);
                    toolboxViz.setButtonActive(PointSelectionButton,false);
                    toolboxViz.setButtonActive(LassoSelectionButton,true);
                    break;
            }
        } else if (toolViewHandler.hasClassSelection()) {
            const int selectedClass = toolViewHandler.getSelectedClass();
            if (selectedClass >= 0 && selectedClass < nClasses) {
                for (int i=0; i<nTestImages; ++i) {
                    selection[i] = testLabels[i] == selectedClass ? 1.f : 0.f;
                }
                filterResponseViz.setSelection(selection);
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
