#include "feature_projection_viz.h"

FeatureProjectionViz::FeatureProjectionViz(caffe::Net<float> & net, const std::string activationBlobName) {

    caffe::Blob<float> activationBlob;
    activationBlob.ReshapeLike(*net.blob_by_name(activationBlobName));

    int responsibleLayerNum = getResponsibleLayerNum(net, activationBlobName);
    const boost::shared_ptr<caffe::Layer<float> > responsibleLayer = net.layers()[responsibleLayerNum];

    caffe::Blob<float> nextBlobDown;
    nextBlobDown.ReshapeLike(*net.bottom_vecs()[responsibleLayerNum][0]);

    undoLayer(responsibleLayer,activationBlob,nextBlobDown);

}

int FeatureProjectionViz::getResponsibleLayerNum(caffe::Net<float> & net, const std::string blobName) {

    boost::shared_ptr<caffe::Blob<float> > targetBlob = net.blob_by_name(blobName);
    for (int i=0; i<net.top_vecs().size(); ++i) {
        for (int j=0; j<net.top_vecs()[i].size(); ++j) {
            caffe::Blob<float> * blob = net.top_vecs()[i][j];
            if (blob == targetBlob.get()) {
                return i;
            }
        }
    }

}

void FeatureProjectionViz::undoLayer(const boost::shared_ptr<caffe::Layer<float> > & layer, const caffe::Blob<float> & top, caffe::Blob<float> & bottom ) {

    std::string layerType(layer->type());
    if (layerType == "Convolution") {\
        undoConvolution(boost::static_pointer_cast<caffe::ConvolutionLayer<float> >(layer),top,bottom);
    } else if (layerType == "Pooling") {
        std::cout << "undoing pooling" << std::endl;
    } else if (layerType == "Relu") {
        std::cout << "undoing rectification" << std::endl;
    } else {
        std::cerr << "can't undo " << layerType << " layers" << std::endl;
    }

}

void FeatureProjectionViz::undoConvolution(const boost::shared_ptr<caffe::ConvolutionLayer<float> > & layer, const caffe::Blob<float> & top, caffe::Blob<float> & bottom) {

    std::cout << "undoing convolution" << std::endl;
    const int N = top.num();
    const boost::shared_ptr<caffe::Blob<float> > weights = layer->blobs()[0];
    const int kernelW = weights->width();
    const int kernelH = weights->height();
    const int kernelCW = kernelW / 2;
    const int kernelCH = kernelH / 2;
//    const int kernelChannels =

    std::cout << weights->num() << " x " << weights->channels() << " x " << weights->height() << " x " << weights->width() << std::endl;

    caffe::caffe_set(bottom.count(),0.f,bottom.mutable_cpu_data());
    for (int n=0; n<N; ++n) {
        for (int c=0; c<top.channels(); ++c) {
            for (int h=0; h<top.height(); ++h) {
                const int minV = std::max(0,kernelCH - h);
                const int maxV = std::min(kernelH,kernelCH - (top.height()-1));
                for (int w=0; w<top.width(); ++w) {
                    const int minU = std::max(0,kernelCW - w);
                    const int maxU = std::min(kernelW,kernelCW - (top.width()-1));
                    for (int v=minV; v<maxV; ++v) {
                        for (int u=minU; u<maxU; ++u) {
                            for (int k=0; k<bottom.channels(); ++k) {
                                const int weightIndex = weights->offset(c,k,v,u);
                                const float weightValue = weights->cpu_data()[weightIndex];
                                const int bottomIndex = bottom.offset(n,k,h-kernelCH+v,w-kernelCW+u);
                                bottom.mutable_cpu_data()[bottomIndex] += weightValue;
                            }
                        }
                    }
                }
            }
        }
    }

}
