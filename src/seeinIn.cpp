#include <pangolin/pangolin.h>

#include <iostream>
#include <limits>

#include <assert.h>

#include <Eigen/Dense>
#include <caffe/caffe.hpp>

#include <vector_types.h>
#include <vector_functions.h>
#include <helper_math.h>

#include "gl_helpers.h"
#include "mnist_io.h"
#include "seein_in_mouse_handler.h"

static const int guiWidth = 1920;
static const int guiHeight = 1080;
static const int panelWidth = 200;
static const float aspectRatio = guiWidth/(float)guiHeight;

static const std::string weightFile = "/home/tws10/Development/caffe/examples/siamese/mnist_siamese_iter_50000.caffemodel";
static const std::string netFile = "/home/tws10/Development/caffe/examples/siamese/mnist_siamese.prototxt";

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

//void loadWeights(caffe::Net<float> & net, std::string weightFile) {

//    caffe::NetParameter params;
//    caffe::ReadProtoFromBinaryFile(weightFile,&params);

//    const int nLayers = params.layer_size();
//    std::cout << nLayers << std::endl;
//    for (int i=0; i<nLayers; ++i) {
//        const caffe::LayerParameter & layerParam = params.layer(i);
//        std::cout << layerParam.name() << std::endl;
//        const int nParams = layerParam.param_size();
//        std::cout << nParams << std::endl;
//        for (int p=0; p<nParams; ++p) {
//            caffe::ParamSpec param = layerParam.param(p);
//            std::cout << param.name() << std::endl;
//            std::map<std::string,int>::const_iterator it = net.param_names_index().find(param.name());
//            if (it != net.param_names_index().end()) {
//                std::cout << "net has a matching param" << std::endl;
//                const caffe::BlobProto & blob = layerParam.blobs(p);
//                const caffe::BlobShape & blobShape = blob.shape();
//                std::cout << blobShape.dim(0);
//                for (int d=1; d<blobShape.dim_size(); ++d) {
//                    std::cout << " x " << blobShape.dim(d);
//                } std::cout << std::endl;

//                boost::shared_ptr<caffe::Blob<float> > netParam = net.params()[it->second];
//                std::cout << netParam->shape()[0];
//                for (int d=1; d<netParam->shape().size(); ++d) {
//                    std::cout << " x " << netParam->shape()[d];
//                } std::cout << std::endl;
//            }
//        }

//    }

//}

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

    // -=-=-=-=- process test data -=-=-=-=-
    boost::shared_ptr<caffe::Blob<float> > inputBlob = net.blobs()[net.input_blob_indices()[0]];
    assert(inputBlob->num() == nTestImages);
    memcpy(inputBlob->mutable_cpu_data(),testImages,inputBlob->count()*sizeof(float));
    net.ForwardPrefilled();

    // -=-=-=-=- process embedding -=-=-=-=-
    boost::shared_ptr<caffe::Blob<float> > outputBlob = net.blobs()[net.output_blob_indices()[0]];
    float2 minEmbedding = make_float2(std::numeric_limits<float>::infinity(),std::numeric_limits<float>::infinity());
    float2 maxEmbedding = -1*minEmbedding;
    for (int i=0; i<nTestImages; ++i) {
        float2 embedding = make_float2(outputBlob->cpu_data()[2*i],outputBlob->cpu_data()[2*i + 1]);
        minEmbedding = fminf(minEmbedding,embedding);
        maxEmbedding = fmaxf(maxEmbedding,embedding);
    }

    float2 embeddingSize = maxEmbedding - minEmbedding;

    float2 paddedEmbeddingSize = embeddingSize + make_float2(0.1,0.1);

    float2 viewportSize;
    if (paddedEmbeddingSize.x / aspectRatio < paddedEmbeddingSize.y) {
        //embedding height is limiting dimension
        viewportSize = make_float2(paddedEmbeddingSize.y * aspectRatio, paddedEmbeddingSize.y);
    } else {
        //embedding width is limiting dimension
        viewportSize = make_float2(paddedEmbeddingSize.x, paddedEmbeddingSize.x / aspectRatio );
    }
    float2 viewportCenter = minEmbedding + 0.5*embeddingSize;

    std::cout << "embedding spans " << minEmbedding.x << " -> " << maxEmbedding.x << ", " << minEmbedding.y << " -> " << maxEmbedding.y << std::endl;
    std::cout << "embedding size: " << embeddingSize.x << ", " << embeddingSize.y << std::endl;
    std::cout << "embedding center: " << viewportCenter.x << ", " << viewportCenter.y << std::endl;
    std::cout << "viewport size: " << viewportSize.x << ", " << viewportSize.y << std::endl;

    // -=-=-=-=- set up pangolin -=-=-=-=-
    pangolin::CreateGlutWindowAndBind("Seein' In", guiWidth+panelWidth, guiHeight,GLUT_MULTISAMPLE | GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glewInit();

    SeeinInMouseHander handler(viewportSize,viewportCenter,
                               (const float2 *)outputBlob->cpu_data(),nTestImages);

    pangolin::View & imgDisp = pangolin::Display("img").SetAspect(aspectRatio).SetHandler(&handler);

    pangolin::CreatePanel("panel").SetBounds(0.0, 1.0, 0.0, pangolin::Attach::Pix(panelWidth));

    pangolin::Display("display")
            .SetBounds(1.0,0.0,1.0,pangolin::Attach::Pix(panelWidth))
            .SetLayout(pangolin::LayoutEqual)
            .AddDisplay(imgDisp);

    pangolin::GlTexture imageTex(imageWidth,imageHeight);
    imageTex.SetNearestNeighbour();

    for (long frame=1; !pangolin::ShouldQuit(); ++frame) {

        if (pangolin::HasResized()) {
            pangolin::DisplayBase().ActivateScissorAndClear();
        }

        glClearColor(1,1,1,1);
        imgDisp.ActivateScissorAndClear();
        imgDisp.ActivatePixelOrthographic();
        glPushMatrix();
        setUpViewport(imgDisp,viewportSize,viewportCenter);

        glPointSize(3);
        glColor3ub(0,0,0);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2,GL_FLOAT,0,outputBlob->cpu_data());

        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(3,GL_UNSIGNED_BYTE,0,testColors.data());

        glDrawArrays(GL_POINTS, 0, nTestImages);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);

        int hoveredPointIndex = handler.getHoveredOverPoint();
        glPointSize(12);
        glBegin(GL_POINTS);
        glColor3ub(255,255,255);
        glVertex2fv(outputBlob->cpu_data() + 2*hoveredPointIndex);
        glEnd();
        glPointSize(9);
        glBegin(GL_POINTS);
        glColor(testColors[hoveredPointIndex]);
        glVertex2fv(outputBlob->cpu_data() + 2*hoveredPointIndex);
        glEnd();
        glPointSize(1);

        if (hoveredPointIndex >= 0 && hoveredPointIndex < nTestImages) {
            imageTex.Upload(testImages + hoveredPointIndex*imageWidth*imageHeight,GL_LUMINANCE,GL_FLOAT);
            const float2 hoveredPoint = make_float2(outputBlob->cpu_data()[2*hoveredPointIndex], outputBlob->cpu_data()[2*hoveredPointIndex+1]);
            const float2 textureLocation = hoveredPoint + make_float2(0.075,0.075);
            const float2 textureSize = make_float2(0.5,0.5);

            glLineWidth(2);
            glColor3ub(196,196,196);
            float linePts[] = { textureLocation.x,                 textureLocation.y,
                                textureLocation.x + textureSize.x, textureLocation.y,
                                textureLocation.x + textureSize.x, textureLocation.y + textureSize.y,
                                textureLocation.x,                 textureLocation.y + textureSize.y,
                                textureLocation.x,                 textureLocation.y,
                                hoveredPoint.x,                    hoveredPoint.y };
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer( 2, GL_FLOAT, 0, linePts);

            glDrawArrays(GL_LINE_STRIP, 0, 6);

            glDisableClientState(GL_VERTEX_ARRAY);

            glColor3ub(255,255,255);
            renderTexture(imageTex,
                          textureLocation,
                          textureSize);
            glLineWidth(1);
        }

        glPopMatrix();

        glClearColor(0,0,0,1);
        pangolin::FinishGlutFrame();

    }

    return 0;
}
