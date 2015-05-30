#ifndef FEATURE_PROJECTION_VIZ_H
#define FEATURE_PROJECTION_VIZ_H

#include <caffe/caffe.hpp>

class FeatureProjectionViz {
public:

    FeatureProjectionViz(caffe::Net<float> & net, const std::string activationBlobName);

private:
    int getResponsibleLayerNum(caffe::Net<float> & net, const std::string blobName);

    void undoLayer(const boost::shared_ptr<caffe::Layer<float> > & layer, const caffe::Blob<float> & top, caffe::Blob<float> & bottom);

    void undoConvolution(const boost::shared_ptr<caffe::ConvolutionLayer<float> > & layer, const caffe::Blob<float> & top, caffe::Blob<float> & bottom);
};

#endif // FILTER_PROJECTION_VIZ_H
