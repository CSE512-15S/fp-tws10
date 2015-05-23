#include "filter_response_viz.h"

#include "gl_helpers.h"
#include <helper_math.h>
#include <limits>

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
    border_(1),
    response_(responseBlob->count()/responseBlob->num()) {

    resize(vizWidth, zoom);

    minDataVal_ = std::numeric_limits<float>::infinity();
    maxDataVal_ = -std::numeric_limits<float>::infinity();
    for (int i=0; i<responseBlob->count(); ++i) {
        minDataVal_ = std::min(minDataVal_,responseBlob->cpu_data()[i]);
        maxDataVal_ = std::max(maxDataVal_,responseBlob->cpu_data()[i]);
    }

    std::fill(response_.begin(),response_.end(),(maxDataVal_ - minDataVal_)/2);
    std::cout << minDataVal_ << " -> " << maxDataVal_ << std::endl;
}

void FilterResponseViz::resize(const int vizWidth, const float zoom) {
    zoom_ = zoom;
    vizWidth_ = vizWidth;
    responseCols_ = std::floor((vizWidth_ - border_)/(width_*zoom_ + border_));
    responseRows_ = ceilDivide(channels_,responseCols_);

    vizHeight_ = responseRows_*(zoom_*height_+border_)+border_;
}

void FilterResponseViz::setSelection(const int selectedImage) {
    std::memcpy(response_.data(),data_ + selectedImage*channels_*height_*width_,response_.size()*sizeof(float));
}

void FilterResponseViz::setSelection(std::vector<bool> selection) {
    std::memset(response_.data(),0,response_.size()*sizeof(float));
    int n = 0;
    for (int i=0; i<selection.size(); ++i) {
        if (selection[i]) {
            for (int j=0; j<channels_*height_*width_; ++j) {
                response_[j] += data_[i*channels_*height_*width_ + j];
            }
            ++n;
        }
    }
    const float oneOverN = 1.f/n;
    for (int j=0; j<channels_*height_*width_; ++j) {
        response_[j] *= oneOverN;
    }
}

void FilterResponseViz::renderResponse() {
    float scale = 1.f/(maxDataVal_-minDataVal_);
    float bias = -minDataVal_*scale;
    glScalePixels(make_float3(scale),make_float3(bias));
    for (int c=0; c<channels_; ++c) {
        const int row = c / responseCols_;
        const int col = c % responseCols_;
        tex_.Upload(response_.data() + c*height_*width_,GL_LUMINANCE,GL_FLOAT);
        renderTexture(tex_,make_float2((zoom_*width_+border_)*col,vizHeight_-(zoom_*height_+border_)*(row+1)),zoom_*make_float2(width_,height_));
    }
    glScalePixels();
}

