#include "feature_projector.h"

FeatureProjector::FeatureProjector(caffe::Net<float> & net, std::vector<std::string> & blobsToVisualize) :
    net_(net),
    blobNames_(blobsToVisualize) {

    for (std::string blobName : blobNames_) {
        boost::shared_ptr<caffe::Blob<float> > netBlob = net.blob_by_name(blobName);
        std::vector<int> blobShape = netBlob->shape();
        blobShape[0] = 1;
        caffe::Blob<float> * mirroredBlob = new caffe::Blob<float>(blobShape);
        mirroredBlobs_[netBlob.get()] = mirroredBlob;
    }

}

FeatureProjector::~FeatureProjector() {

    for (std::pair<const caffe::Blob<float>*,caffe::Blob<float>*> keyVal : mirroredBlobs_) {
        delete keyVal.second;
    }

}

void FeatureProjector::computeProjection(const std::string activationBlobName, const int imgNum, const int activationIndex, const float activationValue) {

    int activationBlobNum;
    for (int i=0; i<net_.blob_names().size(); ++i) {
        if (net_.blob_names()[i] == activationBlobName) {
            activationBlobNum = i;
            break;
        }
    }

    const boost::shared_ptr<caffe::Blob<float> > activationBlob = net_.blob_by_name(activationBlobName);
    std::vector<int> activationBlobShape = activationBlob->shape();
    activationBlobShape[0] = 1;
    caffe::Blob<float> * activationTmpBlob = new caffe::Blob<float>(activationBlobShape);
    mirroredBlobs_[activationBlob.get()] = activationTmpBlob;

    caffe::caffe_set(activationTmpBlob->count(),0.f,activationTmpBlob->mutable_cpu_data());
    activationTmpBlob->mutable_cpu_data()[activationIndex] = activationValue;

    int layerNum = getResponsibleLayerNum(net_, activationBlobNum);

    std::cout << activationBlobName << " (" << activationBlobNum << ") produced by " << net_.layer_names()[layerNum] << " (" << layerNum << ")" << std::endl;

    for (layerNum; layerNum >= 0; --layerNum) {
        std::cout << "-=- " << layerNum << " -=-=-=-=-=-=-=-" << std::endl;
        std::vector<caffe::Blob<float>*> tmpTops, tmpBottoms;
        for (caffe::Blob<float> * top : net_.top_vecs()[layerNum]) {
            if (mirroredBlobs_.find(top) == mirroredBlobs_.end()) {
                mirroredBlobs_[top] = new caffe::Blob<float>(1,top->channels(),top->height(),top->width());
            }
            tmpTops.push_back(mirroredBlobs_[top]);
        }
        for (caffe::Blob<float> * bottom : net_.bottom_vecs()[layerNum]) {
            if (mirroredBlobs_.find(bottom) == mirroredBlobs_.end()) {
                mirroredBlobs_[bottom] = new caffe::Blob<float>(1,bottom->channels(),bottom->height(),bottom->width());
            }
            tmpBottoms.push_back(mirroredBlobs_[bottom]);
        }

        const boost::shared_ptr<caffe::Layer<float> > layer = net_.layers()[layerNum];
        std::string layerType(layer->type());
        if (layerType == "Convolution") {\
            undoConvolution(boost::static_pointer_cast<caffe::ConvolutionLayer<float> >(layer),tmpTops,tmpBottoms);
        } else if (layerType == "Pooling") {
            if (tmpTops.size() > 1) {
                std::cout << "copying mask" << std::endl;
                caffe::caffe_copy(tmpTops[1]->count(),net_.top_vecs()[layerNum][1]->cpu_data() + imgNum*tmpTops[1]->count(),tmpTops[1]->mutable_cpu_data());
            }
            undoPooling(boost::static_pointer_cast<caffe::PoolingLayer<float> >(layer),tmpTops,tmpBottoms);
        } else if (layerType == "ReLU") {
            undoReLU(boost::static_pointer_cast<caffe::ReLULayer<float> >(layer),tmpTops,tmpBottoms);
        } else if (layerType == "InnerProduct") {
            undoInnerProduct(boost::static_pointer_cast<caffe::InnerProductLayer<float> >(layer),tmpTops,tmpBottoms);
        } else {
            std::cerr << "can't undo " << layerType << " layers" << std::endl;
        }

    }

    caffe::Blob<float> * dataBlob = mirroredBlobs_[net_.blob_by_name("data").get()];
    std::cout << "output " << dataBlob->num() << " x " << dataBlob->channels() << " x " << dataBlob->height() << " x " << dataBlob->width() << std::endl;

    float dataMin = 1e10;
    float dataMax = -dataMin;
    for (int i=0; i<dataBlob->count(); ++i) {
        dataMin = std::min(dataMin,dataBlob->cpu_data()[i]);
        dataMax = std::max(dataMax,dataBlob->cpu_data()[i]);
    }
    std::cout << "data range: " << dataMin << " -> " << dataMax << std::endl;

    std::cout << "now activated at " << activationTmpBlob->cpu_data()[activationIndex] << std::endl;
}

const float * FeatureProjector::getResponse(const std::string blobName) {

    boost::shared_ptr<caffe::Blob<float> > blob = net_.blob_by_name(blobName);
    caffe::Blob<float> * mirroredBlob = mirroredBlobs_[blob.get()];
    return mirroredBlob->cpu_data();

}

int FeatureProjector::getResponsibleLayerNum(caffe::Net<float> & net, const int blobNum) {

    boost::shared_ptr<caffe::Blob<float> > targetBlob = net.blobs()[blobNum];
    for (int i=0; i<net.top_vecs().size(); ++i) {
        for (int j=0; j<net.top_vecs()[i].size(); ++j) {
            caffe::Blob<float> * blob = net.top_vecs()[i][j];
            if (blob == targetBlob.get()) {
                return i;
            }
        }
    }

}

int FeatureProjector::getInputBlobNum(caffe::Net<float> & net, const int layerNum) {

    const caffe::Blob<float> * targetBlob = net.bottom_vecs()[layerNum][0];
    for (int i=0; i<net.blobs().size(); ++i) {
        if (net.blobs()[i].get() == targetBlob) {
            return i;
        }
    }

}

void FeatureProjector::undoConvolution(const boost::shared_ptr<caffe::ConvolutionLayer<float> > & layer, std::vector<caffe::Blob<float>*> & tmpTops, std::vector<caffe::Blob<float>*> & tmpBottoms ) {

    std::cout << "undoing convolution" << std::endl;
    assert(layer->blobs().size() == 2);
    const boost::shared_ptr<caffe::Blob<float> > weights = layer->blobs()[0];
    const boost::shared_ptr<caffe::Blob<float> > biases = layer->blobs()[1];
    const int kernelW = weights->width();
    const int kernelH = weights->height();
    const int kernelCW = kernelW / 2;
    const int kernelCH = kernelH / 2;
//    const int kernelChannels =

    std::cout << weights->num() << " x " << weights->channels() << " x " << weights->height() << " x " << weights->width() << std::endl;
    std::cout << biases->num() << " x " << biases->channels() << " x " << biases->height() << " x " << biases->width() << std::endl;

    assert(tmpBottoms.size() == 1);
    assert(tmpTops.size() == 1);
    assert(layer->layer_param().convolution_param().pad() == 0);
    assert(layer->layer_param().convolution_param().stride() == 1);
    caffe::Blob<float> & bottom = *tmpBottoms[0];
    caffe::Blob<float> & top = *tmpTops[0];

    caffe::caffe_set(bottom.count(),0.f,bottom.mutable_cpu_data());
    for (int c=0; c<top.channels(); ++c) {
        const float bias = biases->cpu_data()[biases->offset(c,0,0,0)];
        for (int h=0; h<top.height(); ++h) {
//            const int minV = std::max(0,kernelCH - h);
//            const int maxV = std::min(kernelH,top.height()-1+kernelCH - h);
            for (int w=0; w<top.width(); ++w) {
                if (top.cpu_data()[top.offset(0,c,h,w)] == 0.f) { continue; }
                const float topValue = (top.cpu_data()[top.offset(0,c,h,w)] - bias);
//                const int minU = std::max(0,kernelCW - w);
//                const int maxU = std::min(kernelW,top.width()-1+kernelCW - w);
                for (int v=0; v<kernelH; ++v) {
                    for (int u=0; u<kernelW; ++u) {
                        for (int k=0; k<bottom.channels(); ++k) {
                            const int weightIndex = weights->offset(c,k,kernelH-1-v,kernelW-1-u);
                            const float weightValue = weights->cpu_data()[weightIndex];
                            const int bottomIndex = bottom.offset(0,k,h+v,w+u);
                            bottom.mutable_cpu_data()[bottomIndex] += topValue*weightValue;
                        }
                    }
                }
            }
        }
    }
}

void FeatureProjector::undoInnerProduct(const boost::shared_ptr<caffe::InnerProductLayer<float> > & layer, std::vector<caffe::Blob<float> *> & tmpTops, std::vector<caffe::Blob<float> *> & tmpBottoms) {

    std::cout << "undoing inner product" << std::endl;
    assert(tmpBottoms.size() == 1);
    assert(tmpTops.size() == 1);
    caffe::Blob<float> & bottom = *tmpBottoms[0];
    caffe::Blob<float> & top = *tmpTops[0];

    assert(layer->blobs().size() == 2);
    const boost::shared_ptr<caffe::Blob<float> > weights = layer->blobs()[0];
    const boost::shared_ptr<caffe::Blob<float> > biases = layer->blobs()[1];\

    std::cout << "bottom:  " << bottom.num() << " x " << bottom.channels() << " x " << bottom.height() << " x " << bottom.width() << std::endl;
    std::cout << "top:     " << top.num() << " x " << top.channels() << " x " << top.height() << " x " << top.width() << std::endl;
    std::cout << "weights: " << weights->num() << " x " << weights->channels() << " x " << weights->height() << " x " << weights->width() << std::endl;

    caffe::caffe_set(bottom.count(),0.f,bottom.mutable_cpu_data());
    for (int t=0; t<top.count(); ++t) {
        if (top.cpu_data()[t] == 0.f) { continue; }
        const float topVal = top.cpu_data()[t] - biases->cpu_data()[t];
        for (int b=0; b<bottom.count(); ++b) {
            bottom.mutable_cpu_data()[b] += topVal*weights->cpu_data()[weights->offset(t,b,0,0)];
        }
    }
}

void FeatureProjector::undoPooling(const boost::shared_ptr<caffe::PoolingLayer<float> > & layer, std::vector<caffe::Blob<float>*> & tmpTops, std::vector<caffe::Blob<float>*> & tmpBottoms ) {

    std::cout << "undoing pooling" << std::endl;

    std::cout << tmpTops.size() << " tops" << std::endl;
    std::cout << tmpBottoms.size() << " bottoms" << std::endl;

    assert(tmpBottoms.size() == 1);
    assert(tmpTops.size() == 2);
    caffe::Blob<float> & bottom = *tmpBottoms[0];
    caffe::Blob<float> & top = *tmpTops[0];
    caffe::Blob<float> & mask = *tmpTops[1];

    std::cout << "bottom: " << bottom.num() << " x " << bottom.channels() << " x " << bottom.height() << " x " << bottom.width() << std::endl;
    std::cout << "top:    " << top.num() << " x " << top.channels() << " x " << top.height() << " x " << top.width() << std::endl;
    std::cout << "mask:   " << mask.num() << " x " << mask.channels() << " x " << mask.height() << " x " << mask.width() << std::endl;

    caffe::caffe_set(bottom.count(),0.f,bottom.mutable_cpu_data());
    for (int c=0; c<top.channels(); ++c) {
        for (int y=0; y<top.height(); ++y) {
            for (int x=0; x<top.width(); ++x) {
                int bottomIndex = mask.cpu_data()[mask.offset(0,0,y,x)];
                bottom.mutable_cpu_data()[bottomIndex + c*bottom.width()*bottom.height()] =
                        top.cpu_data()[top.offset(0,c,y,x)];
//                        std::cout << bottomIndex << " ";
            } //std::cout << std::endl;
        }
    }

//    layer->

}

void FeatureProjector::undoReLU(const boost::shared_ptr<caffe::ReLULayer<float> > &layer, std::vector<caffe::Blob<float> *> &tmpTops, std::vector<caffe::Blob<float> *> &tmpBottoms) {

    std::cout << "undoing rectification" << std::endl;

    assert(tmpBottoms.size() == 1);
    assert(tmpTops.size() == 1);
    caffe::Blob<float> & bottom = *tmpBottoms[0];
    caffe::Blob<float> & top = *tmpTops[0];

    for (int i=0; i<top.count(); ++i) {
        bottom.mutable_cpu_data()[i] = std::max(0.f,top.cpu_data()[i]);
    }

}
