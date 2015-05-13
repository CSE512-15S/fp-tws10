#include <pangolin/pangolin.h>

#include <iostream>

#include <Eigen/Dense>
#include <GL/glut.h>
#include <caffe/caffe.hpp>

const int guiWidth = 1280;
const int guiHeight = 960;
const int panelWidth = 200;

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

int main() {

    pangolin::CreateGlutWindowAndBind("Seein' In", guiWidth, guiHeight,GLUT_MULTISAMPLE | GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glewInit();

    pangolin::OpenGlRenderState camState(pangolin::ProjectionMatrix(640,480,420,420,320,240,0.1,500));

    pangolin::View & imgDisp = pangolin::Display("img").SetAspect(640.0/480.0);

    pangolin::CreatePanel("panel").SetBounds(0.0, 1.0, 0.0, pangolin::Attach::Pix(panelWidth));

    pangolin::Display("display")
            .SetBounds(1.0,0.0,1.0,pangolin::Attach::Pix(panelWidth))
            .SetLayout(pangolin::LayoutEqual)
            .AddDisplay(imgDisp);

    glEnable(GL_DEPTH_TEST);
    glClearColor(1,1,1,1);

    std::string weightFile = "/home/tws10/Development/caffe/examples/siamese/mnist_siamese_iter_50000.caffemodel";
    std::string netFile = "/home/tws10/Development/caffe/examples/siamese/mnist_siamese.prototxt";

    caffe::Net<float> net(netFile,caffe::TEST);

    net.CopyTrainedLayersFrom(weightFile);

    for (long frame=1; !pangolin::ShouldQuit(); ++frame) {

        if (pangolin::HasResized()) {
            pangolin::DisplayBase().ActivateScissorAndClear();
        }

        imgDisp.ActivateScissorAndClear();
        imgDisp.ActivatePixelOrthographic();



        pangolin::FinishGlutFrame();

    }

    return 0;
}
