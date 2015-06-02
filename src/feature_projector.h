#ifndef FEATURE_PROJECTOR_H
#define FEATURE_PROJECTOR_H

#include <caffe/caffe.hpp>
#include <pangolin/pangolin.h>

class FeatureProjector {
public:

    FeatureProjector(caffe::Net<float> & net, std::vector<std::string> & blobsToVisualize);

    ~FeatureProjector();

    void computeProjection(const std::string activationBlobName, const int imgNum, const int activationIndex, const float activationValue);

    const float * getResponse(const std::string blobName);

//    void render() { tex.RenderToViewportFlipY(); }

private:

    // -=-=-=-=-=- methods -=-=-=-=-=-
    int getResponsibleLayerNum(caffe::Net<float> & net_, const int blobNum);

    int getInputBlobNum(caffe::Net<float> & net_, const int layerNum);

    void undoConvolution(const boost::shared_ptr<caffe::ConvolutionLayer<float> > & layer, std::vector<caffe::Blob<float>*> & tmpTops, std::vector<caffe::Blob<float>*> & tmpBottoms);

    void undoInnerProduct(const boost::shared_ptr<caffe::InnerProductLayer<float> > & layer, std::vector<caffe::Blob<float>*> & tmpTops, std::vector<caffe::Blob<float>*> & tmpBottoms);

    void undoPooling(const boost::shared_ptr<caffe::PoolingLayer<float> > & layer, std::vector<caffe::Blob<float>*> & tmpTops, std::vector<caffe::Blob<float>*> & tmpBottoms);

    void undoReLU(const boost::shared_ptr<caffe::ReLULayer<float> > & layer, std::vector<caffe::Blob<float>*> & tmpTops, std::vector<caffe::Blob<float>*> & tmpBottoms);

    // -=-=-=-=-=- members -=-=-=-=-=-
//    pangolin::GlTexture tex;
    caffe::Net<float> & net_;
    std::vector<std::string> blobNames_;
    std::map<const caffe::Blob<float>*,caffe::Blob<float>*> mirroredBlobs_;
};

#endif // FEATURE_PROJECTOR_H
