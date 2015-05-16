#include "filter_response_viz.h"

#include "gl_helpers.h"
#include <helper_math.h>

inline int ceilDivide(const int num, const int denom) {
    const div_t result = std::div(num,denom);
    return result.quot + (result.rem == 0 ? 0 : 1);
}

inline int floorDivide(const int num, const int denom) {
    return num / denom;
}

FilterResponseViz::FilterResponseViz(const boost::shared_ptr<caffe::Blob<float> > responseBlob, const int vizWidth, const float zoom) :
    tex_(responseBlob->width(),responseBlob->width()),
    channels_(responseBlob->channels()),
    height_(responseBlob->height()),
    width_(responseBlob->width()),
    data_(responseBlob->cpu_data()),
    border_(1) {

    resize(vizWidth, zoom);
}

void FilterResponseViz::resize(const int vizWidth, const float zoom) {
    zoom_ = zoom;
    vizWidth_ = vizWidth;
    responseCols_ = std::floor((vizWidth_/zoom_ - border_)/(width_ + border_));
    responseRows_ = ceilDivide(channels_,responseCols_);

    vizHeight_ = zoom_*responseRows_*(height_+border_)+border_;
}

void FilterResponseViz::renderResponse(const int image) {

    for (int c=0; c<channels_; ++c) {
        const int row = c / responseCols_;
        const int col = c % responseCols_;
        tex_.Upload(data_ + (image*channels_ + c)*height_*width_,GL_LUMINANCE,GL_FLOAT);
        renderTexture(tex_,make_float2(zoom_*(width_+border_)*col,vizHeight_-zoom_*(height_+border_)*(row+1)),zoom_*make_float2(width_,height_));
    }
}

