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
#include "fonts/font_manager.h"
#include "mouse_handlers/single_embedding_view_mouse_handler.h"
#include "mouse_handlers/filter_view_mouse_handler.h"
#include "mouse_handlers/multi_embedding_view_mouse_handler.h"
#include "mouse_handlers/tool_view_mouse_handler.h"
#include "util/caffe_helpers.h"
#include "util/gl_helpers.h"
#include "util/image_io.h"
#include "util/selection_manager.h"
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

inline void printBlobSize(boost::shared_ptr<caffe::Blob<float> > blob) {
    std::cout << blob->num() << " x " << blob->channels() << " x " << blob->height() << " x " << blob->width() << std::endl;
}

inline bool fileExists(std::string filename) {
    struct stat buffer;
    return stat(filename.c_str(), &buffer) == 0;
}

int run(int argc, char * * argv,
        const std::string networkFilename, const std::string weightFilename,
        float * testImages, const int imageWidth,
        const int imageHeight, const int nTestImages, unsigned char * testLabels,
        const int nClasses, const uchar3 * classColors, const std::string * classNames) {

    // -=-=-=-=- set up caffe -=-=-=-=-
    caffe::GlobalInit(&argc,&argv);

#ifndef CPU_ONLY
    caffe::Caffe::SetDevice(0);
    caffe::Caffe::set_mode(caffe::Caffe::GPU);
#endif // CPU_ONLY

    // -=-=-=-=- load learned network -=-=-=-=-
    caffe::Net<float> net(networkFilename,caffe::TEST);
    net.CopyTrainedLayersFrom(weightFilename);

    // -=-=-=-=- load mnist test data -=-=-=-=-

    std::vector<uchar3> testColors(nTestImages);
    for (int i=0; i<nTestImages; ++i) { testColors[i] = classColors[testLabels[i]]; }

//    std::vector<float> selection(nTestImages,0.5f);
    SelectionManager selection(nTestImages);

    // -=-=-=-=- process test data -=-=-=-=-
    boost::shared_ptr<caffe::Blob<float> > inputBlob = net.blobs()[net.input_blob_indices()[0]];
    assert(inputBlob->num() == nTestImages);
    memcpy(inputBlob->mutable_cpu_data(),testImages,inputBlob->count()*sizeof(float));
    net.ForwardPrefilled();

    // -=-=-=-=- set up pangolin -=-=-=-=-
    pangolin::CreateGlutWindowAndBind("Seein' In --- " + net.name() , guiWidth, guiHeight,GLUT_MULTISAMPLE | GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glewInit();

    pangolin::GlTexture imageTex(imageWidth,imageHeight);
    imageTex.SetNearestNeighbour();

    const int overviewWidth = 4096; //256;
    const int overviewHeight = overviewWidth*embeddingViewAspectRatio;
    pangolin::GlTexture overviewTex(overviewWidth,overviewHeight);

    // -=-=-=-=- set up font management -=-=-=-=-
    FontManager fontManager("GaramondNo8");

    // -=-=-=-=- set up point shader -=-=-=-=-
    boost::shared_ptr<caffe::Blob<float> > outputBlob = net.blobs()[net.output_blob_indices()[0]];
    ScatterPlotShader pointShader;

    // -=-=-=-=- set up visualizations -=-=-=-=-
    SingleEmbeddingViz embeddingViz(embeddingViewAspectRatio,testImages,imageWidth,imageHeight,imageTex,pointShader,selection.getSelection().data());
    embeddingViz.setEmbedding((const float2 *)outputBlob->cpu_data(),testColors.data(),nTestImages);

    MultiEmbeddingViz multiEmbeddingViz(embeddingViewAspectRatio,testImages,imageWidth,imageHeight,imageTex,pointShader,selection.getSelection().data());

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
    Toolbox toolboxViz(classColors,classNames,nClasses,fontManager, overviewWidth, overviewHeight, overviewTex);
    toolboxViz.setButtonActive(LassoSelectionButton,false);
    toolboxViz.setActiveEmbeddingViz(activeEmbeddingViz);

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
    std::vector<std::string> blobsToVisualize;
    getBlobsToVisualize(net,blobsToVisualize);

    std::map<std::string,int> blobStrides;
    std::map<std::string,int2> blobReceptiveFields;
    getBlobStridesAndReceptiveFields(net,blobsToVisualize,blobStrides,blobReceptiveFields);

//    std::map<std::string,pangolin::GlTexture*> layerResponseTextures;
    float filterVizZoom = 2.f;
    FilterResponseViz filterResponseViz(net,blobsToVisualize,
                                        blobStrides,filterView.GetBounds().w,filterView.GetBounds().h,fontManager,
                                        filterVizZoom,16);
    filterResponseViz.setSelection(selection.getSelection());

    FilterViewMouseHandler filterViewHandler(&filterResponseViz);
    filterView.SetHandler(&filterViewHandler);

//    FeatureProjector featProjector(net,filterResponsesToVisualize); //"feat");

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

        for (int i=0; i<blobsToVisualize.size(); ++i) {
            std::string blobName = blobsToVisualize[i];
            std::string cachedFilename = stringFormat("/tmp/seeinIn.%s.overview%02d.png",net.name().c_str(),i);
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
                                                       blobReceptiveFields[blobName],
                                                       blobStrides[blobName]);
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
    toolboxViz.setOverviewImage(overviewImages.back());

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

//        if (hasSelection) {
            filterResponseViz.render();
            CheckGlDieOnError();
//        }

        // -=-=-=-=-=-=- input handling -=-=-=-=-=-=-
        int selectedPoint = -1;
        if (activeEmbeddingHandler->hasSelection()) {
            switch (activeEmbeddingHandler->getSelectionMode()) {
                case SelectionModeSingle:
                    {
                        selectedPoint = activeEmbeddingHandler->getHoveredOverPoint();
                        if (selectedPoint == -1) {
                            selection.clearSelection();
                            if (multiembeddingVizActive) {
                                multiEmbeddingViz.updateSelectionCoarseToFine();
                            }
                        }
                    } break;
                case SelectionModeLasso:
                    {
                        std::vector<float2> & lassoPoints = activeEmbeddingHandler->getLassoPoints();
                        std::vector<int> enclosedPoints;
                        activeEmbeddingViz->getEnclosedPoints(enclosedPoints,lassoPoints);
                        std::cout << enclosedPoints.size() << " points enclosed" << std::endl;
                        if (multiembeddingVizActive && multiEmbeddingViz.hasSubselection()) {
                            multiEmbeddingViz.setSubselection(enclosedPoints);
                            multiEmbeddingViz.updateSelectionFineToCoarse();
                        } else {
                            selection.setSelection(enclosedPoints);
                        }
                        filterResponseViz.setSelection(selection.getSelection());
                        activeEmbeddingHandler->clearLassoPoints();
                        hasSelection = true;
                    } break;
            }
        }
        if (selectedPoint >= 0) {
            int selectedImage = selectedPoint;
            if (multiembeddingVizActive) {
                selectedImage = multiEmbeddingViz.getPointImage(selectedPoint);
            }
            filterResponseViz.setSelection(selectedImage);
            hasSelection = true;
            if (multiembeddingVizActive && multiEmbeddingViz.hasSubselection()) {
                multiEmbeddingViz.setSubselection(selectedPoint);
            } else {
                if (activeEmbeddingHandler->isAppendSelction()) {
                    selection.appendSelection(selectedImage);
                } else {
                    selection.setSelection(selectedImage);
                }
            }
        }

        const bool layerSelection = filterViewHandler.hasLayerSelection();
        const bool unitSelection = filterViewHandler.hasUnitSelection();
        if ( layerSelection || unitSelection) {
            std::cout << "selected layer " << filterViewHandler.getSelectedLayer() << std::endl;
            const int layerNum = filterViewHandler.getSelectedLayer();
            static int lastLayerNum = -1;
            if (layerNum >= 0 && layerNum != lastLayerNum ) {
                filterResponseViz.setEmbeddingLayer(filterViewHandler.getSelectedLayer());
                std::string blobName = blobsToVisualize[layerNum];
                std::cout << "opening embedding " << blobName << std::endl;
                boost::shared_ptr<caffe::Blob<float> > embeddingBlob = net.blob_by_name(blobName);
                const int dims = embeddingBlob->channels();
                if (multiembeddingVizActive && multiEmbeddingViz.hasSubselection() ) {
                    multiEmbeddingViz.updateSelectionFineToCoarse();
                }
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
                                                       blobReceptiveFields[blobName],
                                                       blobStrides[blobName]);
                    }
                    multiEmbeddingViz.setZoom(1.f);

                    if (unitSelection) {
                        // navigate to the unit
                        const int selectedUnit = filterViewHandler.getSelectedUnit();
                        multiEmbeddingViz.centerOnFeature(selectedUnit);
                    }

                }
                embeddingView.SetHandler(activeEmbeddingHandler);
                toolboxViz.setOverviewImage(overviewImages[layerNum]);
                toolboxViz.setActiveEmbeddingViz(activeEmbeddingViz);

            } else if (unitSelection && multiembeddingVizActive) {
                const int selectedUnit = filterViewHandler.getSelectedUnit();
                multiEmbeddingViz.centerOnFeature(selectedUnit);
            }
            lastLayerNum = layerNum;

        }
//        else if (filterViewHandler.hasUnitSelection()) {
//            std::cout << "selected unit " << filterViewHandler.getSelectedUnit() << " in layer " << filterViewHandler.getSelectedLayer() << std::endl;
//            int selectedImage = filterResponseViz.getSelection();
//            if (selectedImage >= 0) {
//                const int selectedLayerNum = filterViewHandler.getSelectedLayer();
//                const int selectedUnit = filterViewHandler.getSelectedUnit();
//                const std::string blobName = filterResponsesToVisualize[selectedLayerNum];
//                const float activationValue = net.blob_by_name(blobName)->cpu_data()[selectedUnit];
//                std::cout << "activated at " << activationValue << std::endl;
//                featProjector.computeProjection(blobName,selectedImage,selectedUnit,activationValue);
//                std::cout << "stored as " << featProjector.getResponse(blobName)[selectedUnit] << std::endl;
//                filterResponseViz.setResponse(featProjector);
//            }
//        }
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
                case BiggerPointButton:
                    activeEmbeddingViz->setBasePointSize(activeEmbeddingViz->getBasePointSize()*1.25f);
                    break;
                case SmallerPointButton:
                    activeEmbeddingViz->setBasePointSize(activeEmbeddingViz->getBasePointSize()/1.25f);
                    break;
            }
        } else if (toolViewHandler.hasClassSelection()) {
            const int selectedClass = toolViewHandler.getSelectedClass();
            if (selectedClass >= 0 && selectedClass < nClasses) {
                for (int i=0; i<nTestImages; ++i) {
                    selection.getSelection()[i] = testLabels[i] == selectedClass ? SelectionManager::selectedVal_ : SelectionManager::unselectedVal_;
                }
                filterResponseViz.setSelection(selection.getSelection());
                if (multiembeddingVizActive) {
                    multiEmbeddingViz.updateSelectionCoarseToFine();
                }
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
