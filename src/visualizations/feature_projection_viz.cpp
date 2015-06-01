#include "feature_projection_viz.h"

FeatureProjectionViz::FeatureProjectionViz(caffe::Net<float> & net, const std::string activationBlobName) :
    tex(28,28) {

    const int activationIndex = 0;
    const int imgNum = 20;

    std::map<const caffe::Blob<float>*,caffe::Blob<float>*> tmpBlobs;

    int activationBlobNum;
    for (int i=0; i<net.blob_names().size(); ++i) {
        if (net.blob_names()[i] == activationBlobName) {
            activationBlobNum = i;
            break;
        }
    }

    const boost::shared_ptr<caffe::Blob<float> > activationBlob = net.blob_by_name(activationBlobName);
    std::vector<int> activationBlobShape = activationBlob->shape();
    activationBlobShape[0] = 1;
    caffe::Blob<float> * activationTmpBlob = new caffe::Blob<float>(activationBlobShape);
    tmpBlobs[activationBlob.get()] = activationTmpBlob;

    caffe::caffe_set(activationTmpBlob->count(),0.f,activationTmpBlob->mutable_cpu_data());
    activationTmpBlob->mutable_cpu_data()[activationIndex] = 1;

    int layerNum = getResponsibleLayerNum(net, activationBlobNum);


    std::cout << activationBlobName << " (" << activationBlobNum << ") produced by " << net.layer_names()[layerNum] << " (" << layerNum << ")" << std::endl;


    for (layerNum; layerNum >= 0; --layerNum) {
        std::cout << "-=- " << layerNum << " -=-=-=-=-=-=-=-" << std::endl;
        std::vector<caffe::Blob<float>*> tmpTops, tmpBottoms;
        for (caffe::Blob<float> * top : net.top_vecs()[layerNum]) {
            if (tmpBlobs.find(top) == tmpBlobs.end()) {
                tmpBlobs[top] = new caffe::Blob<float>(1,top->channels(),top->height(),top->width());
            }
            tmpTops.push_back(tmpBlobs[top]);
        }
        for (caffe::Blob<float> * bottom : net.bottom_vecs()[layerNum]) {
            if (tmpBlobs.find(bottom) == tmpBlobs.end()) {
                tmpBlobs[bottom] = new caffe::Blob<float>(1,bottom->channels(),bottom->height(),bottom->width());
            }
            tmpBottoms.push_back(tmpBlobs[bottom]);
        }

        const boost::shared_ptr<caffe::Layer<float> > layer = net.layers()[layerNum];
        std::string layerType(layer->type());
        if (layerType == "Convolution") {\
            undoConvolution(boost::static_pointer_cast<caffe::ConvolutionLayer<float> >(layer),tmpTops,tmpBottoms);
        } else if (layerType == "Pooling") {
            if (tmpTops.size() > 1) {
                std::cout << "copying mask" << std::endl;
                caffe::caffe_copy(tmpTops[1]->count(),net.top_vecs()[layerNum][1]->cpu_data() + imgNum*tmpTops[1]->count(),tmpTops[1]->mutable_cpu_data());
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

    caffe::Blob<float> * dataBlob = tmpBlobs[net.blob_by_name("data").get()];
    std::cout << "output " << dataBlob->num() << " x " << dataBlob->channels() << " x " << dataBlob->height() << " x " << dataBlob->width() << std::endl;

    float dataMin = 1e10;
    float dataMax = -dataMin;
    for (int i=0; i<dataBlob->count(); ++i) {
        dataMin = std::min(dataMin,dataBlob->cpu_data()[i]);
        dataMax = std::max(dataMax,dataBlob->cpu_data()[i]);
    }
    std::cout << "data range: " << dataMin << " -> " << dataMax << std::endl;

    tex.Upload(dataBlob->cpu_data(),GL_LUMINANCE,GL_FLOAT);

    for (std::map<const caffe::Blob<float>*,caffe::Blob<float>*>::iterator it = tmpBlobs.begin(); it != tmpBlobs.end(); ++it) {
        delete it->second;
    }

//    do {
//        int responsibleLayerNum = getResponsibleLayerNum(net, activationBlobNum);
//        const boost::shared_ptr<caffe::Layer<float> > responsibleLayer = net.layers()[responsibleLayerNum];

//        int nextBlobDownNum = getInputBlobNum(net,responsibleLayerNum);
//        nextBlobDown->ReshapeLike(*net.blobs()[nextBlobDownNum]);

//        std::cout << net.blob_names()[activationBlobNum] << " -> " << net.blob_names()[nextBlobDownNum] << std::endl;

////        undoLayer(responsibleLayer,*activationBlob,*nextBlobDown);
//        std::swap(activationBlob,nextBlobDown);

//        activationBlobNum = nextBlobDownNum;
//    } while (net.blobs()[activationBlobNum] != net.blob_by_name(inputBlobName));

//    delete activationBlob;
//    delete nextBlobDown;

}

int FeatureProjectionViz::getResponsibleLayerNum(caffe::Net<float> & net, const int blobNum) {

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

int FeatureProjectionViz::getInputBlobNum(caffe::Net<float> & net, const int layerNum) {

    const caffe::Blob<float> * targetBlob = net.bottom_vecs()[layerNum][0];
    for (int i=0; i<net.blobs().size(); ++i) {
        if (net.blobs()[i].get() == targetBlob) {
            return i;
        }
    }

}

void FeatureProjectionViz::undoConvolution(const boost::shared_ptr<caffe::ConvolutionLayer<float> > & layer, std::vector<caffe::Blob<float>*> & tmpTops, std::vector<caffe::Blob<float>*> & tmpBottoms ) {

    std::cout << "undoing convolution" << std::endl;
    const boost::shared_ptr<caffe::Blob<float> > weights = layer->blobs()[0];
    const int kernelW = weights->width();
    const int kernelH = weights->height();
    const int kernelCW = kernelW / 2;
    const int kernelCH = kernelH / 2;
//    const int kernelChannels =

    std::cout << weights->num() << " x " << weights->channels() << " x " << weights->height() << " x " << weights->width() << std::endl;

    assert(tmpBottoms.size() == 1);
    assert(tmpTops.size() == 1);
    assert(layer->layer_param().convolution_param().pad() == 0);
    assert(layer->layer_param().convolution_param().stride() == 1);
    caffe::Blob<float> & bottom = *tmpBottoms[0];
    caffe::Blob<float> & top = *tmpTops[0];

    caffe::caffe_set(bottom.count(),0.f,bottom.mutable_cpu_data());
    for (int c=0; c<top.channels(); ++c) {
        for (int h=0; h<top.height(); ++h) {
//            const int minV = std::max(0,kernelCH - h);
//            const int maxV = std::min(kernelH,top.height()-1+kernelCH - h);
            for (int w=0; w<top.width(); ++w) {
                const float topValue = top.cpu_data()[top.offset(0,c,h,w)];
                if (topValue == 0.f) { continue; }
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

void FeatureProjectionViz::undoInnerProduct(const boost::shared_ptr<caffe::InnerProductLayer<float> > & layer, std::vector<caffe::Blob<float> *> & tmpTops, std::vector<caffe::Blob<float> *> & tmpBottoms) {

    std::cout << "undoing inner product" << std::endl;
    assert(tmpBottoms.size() == 1);
    assert(tmpTops.size() == 1);
    caffe::Blob<float> & bottom = *tmpBottoms[0];
    caffe::Blob<float> & top = *tmpTops[0];


    const boost::shared_ptr<caffe::Blob<float> > weights = layer->blobs()[0];
    std::cout << "bottom:  " << bottom.num() << " x " << bottom.channels() << " x " << bottom.height() << " x " << bottom.width() << std::endl;
    std::cout << "top:     " << top.num() << " x " << top.channels() << " x " << top.height() << " x " << top.width() << std::endl;
    std::cout << "weights: " << weights->num() << " x " << weights->channels() << " x " << weights->height() << " x " << weights->width() << std::endl;

    caffe::caffe_set(bottom.count(),0.f,bottom.mutable_cpu_data());
    for (int t=0; t<top.count(); ++t) {
        const float topVal = top.cpu_data()[t];
        if (topVal == 0.f) { continue; }
        for (int b=0; b<bottom.count(); ++b) {
            bottom.mutable_cpu_data()[b] += topVal*weights->cpu_data()[weights->offset(t,b,0,0)];
        }
    }
}

void FeatureProjectionViz::undoPooling(const boost::shared_ptr<caffe::PoolingLayer<float> > & layer, std::vector<caffe::Blob<float>*> & tmpTops, std::vector<caffe::Blob<float>*> & tmpBottoms ) {

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

void FeatureProjectionViz::undoReLU(const boost::shared_ptr<caffe::ReLULayer<float> > &layer, std::vector<caffe::Blob<float> *> &tmpTops, std::vector<caffe::Blob<float> *> &tmpBottoms) {

    std::cout << "undoing rectification" << std::endl;

    assert(tmpBottoms.size() == 1);
    assert(tmpTops.size() == 1);
    caffe::Blob<float> & bottom = *tmpBottoms[0];
    caffe::Blob<float> & top = *tmpTops[0];

    for (int i=0; i<top.count(); ++i) {
        bottom.mutable_cpu_data()[i] = std::max(0.f,top.cpu_data()[i]);
    }

}
