#include "filter_response_viz.h"

#include "gl_helpers.h"

inline int ceilDivide(const int num, const int denom) {
    const div_t result = std::div(num,denom);
    return result.quot + (result.rem == 0 ? 0 : 1);
}

inline int floorDivide(const int num, const int denom) {
    return num / denom;
}

FilterResponseViz::FilterResponseViz(const boost::shared_ptr<caffe::Blob<float> > responseBlob, const int vizWidth) :
    tex_(responseBlob->width(),responseBlob->width()),
    channels_(responseBlob->channels()),
    height_(responseBlob->height()),
    width_(responseBlob->width()),
    data_(responseBlob->cpu_data()),
    vizWidth_(vizWidth) {

    const int border = 1;
    responseCols_ = floorDivide(vizWidth_ - border, width_ + border);
    responseRows_ = ceilDivide(channels_,responseCols_);

    vizHeight_ = responseRows_*(height_+border)+border;
}

void FilterResponseViz::renderResponse(const int image) {

    const int border = 1;
    for (int c=0; c<channels_; ++c) {
        const int row = c / responseCols_;
        const int col = c % responseCols_;
        tex_.Upload(data_ + (image*channels_ + c)*height_*width_,GL_LUMINANCE,GL_FLOAT);
        renderTexture(tex_,make_float2((width_+border)*col,-(height_+border)*row),make_float2(width_,height_));
    }
}
