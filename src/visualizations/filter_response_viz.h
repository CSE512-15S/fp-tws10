#ifndef FILTER_RESPONSE_VIZ_H
#define FILTER_RESPONSE_VIZ_H

#include <caffe/caffe.hpp>
#include <pangolin/pangolin.h>

class FilterResponseViz {
public:
    FilterResponseViz(const boost::shared_ptr<caffe::Blob<float> > responseBlob, const int vizWidth, const float zoom);

    void resize(const int vizWidth, const float zoom);

    void renderResponse(const int image);

    inline int getVizWidth() const { return vizWidth_; }

    inline int getVizHeight() const { return vizHeight_; }

    inline float getVizZoom() const { return zoom_; }

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
    int border_;
    float zoom_;
};


#endif // FILTER_RESPONSE_VIZ_H
