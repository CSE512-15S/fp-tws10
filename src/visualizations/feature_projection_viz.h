#ifndef FEATURE_PROJECTION_VIZ_H
#define FEATURE_PROJECTION_VIZ_H

#include <caffe/caffe.hpp>
#include <pangolin/pangolin.h>

class FeatureProjectionViz {
public:

    FeatureProjectionViz(caffe::Net<float> & net, const std::string activationBlobName);

    void render() { tex.RenderToViewportFlipY(); }

private:

    // -=-=-=-=-=- methods -=-=-=-=-=-
    int getResponsibleLayerNum(caffe::Net<float> & net, const int blobNum);

    int getInputBlobNum(caffe::Net<float> & net, const int layerNum);

    void undoConvolution(const boost::shared_ptr<caffe::ConvolutionLayer<float> > & layer, std::vector<caffe::Blob<float>*> & tmpTops, std::vector<caffe::Blob<float>*> & tmpBottoms);

    void undoInnerProduct(const boost::shared_ptr<caffe::InnerProductLayer<float> > & layer, std::vector<caffe::Blob<float>*> & tmpTops, std::vector<caffe::Blob<float>*> & tmpBottoms);

    void undoPooling(const boost::shared_ptr<caffe::PoolingLayer<float> > & layer, std::vector<caffe::Blob<float>*> & tmpTops, std::vector<caffe::Blob<float>*> & tmpBottoms);

    void undoReLU(const boost::shared_ptr<caffe::ReLULayer<float> > & layer, std::vector<caffe::Blob<float>*> & tmpTops, std::vector<caffe::Blob<float>*> & tmpBottoms);

    // -=-=-=-=-=- members -=-=-=-=-=-
    pangolin::GlTexture tex;
};

#endif // FILTER_PROJECTION_VIZ_H
