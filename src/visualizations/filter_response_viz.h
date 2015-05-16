#ifndef FILTER_RESPONSE_VIZ_H
#define FILTER_RESPONSE_VIZ_H

#include <caffe/caffe.hpp>
#include <pangolin/pangolin.h>

class FilterResponseViz {
public:
    FilterResponseViz(const boost::shared_ptr<caffe::Blob<float> > responseBlob, const int vizWidth);

    void renderResponse(const int image);

    inline int getVizWidth() const { return vizWidth_; }

    inline int getVizHeight() const { return vizHeight_; }

private:
    const float * data_;
    int channels_;
    int height_;
    int width_;
    pangolin::GlTexture tex_;
    int vizWidth_;
    int vizHeight_;
    int responseRows_;
    int responseCols_;
};

class ConvolutionFilterResponseViz : public FilterResponseViz {
public:

};

class PoolingFilterResponseViz : public FilterResponseViz {

};

#endif // FILTER_RESPONSE_VIZ_H
