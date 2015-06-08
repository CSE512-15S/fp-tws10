#include "filter_response_viz.h"

#include "util/gl_helpers.h"
#include <helper_math.h>
#include <limits>

inline int ceilDivide(const int num, const int denom) {
    const div_t result = std::div(num,denom);
    return result.quot + (result.rem == 0 ? 0 : 1);
}

inline int floorDivide(const int num, const int denom) {
    return num / denom;
}

FilterResponseViz::FilterResponseViz(const caffe::Net<float> & net,
                                     const std::vector<std::string> & layerResponsesToVisualize,
                                     const std::map<std::string,int> & layerRelativeScales,
                                     const int vizWidth,
                                     const int vizHeight,
                                     FontManager & fontManager,
                                     const float zoom,
                                     const int fontSize) :
    fontManager_(fontManager),
    responseNames_(layerResponsesToVisualize),
    vizWidth_(vizWidth),
    vizHeight_(vizHeight),
    zoom_(zoom),
    scroll_(0),
    textMarginVert_(4),
    selection_(-1),
    embeddingLayer_(layerResponsesToVisualize.size()-1) {

    fontSize_ = fontSize;

    for (std::string layerResponse : layerResponsesToVisualize) {
        const boost::shared_ptr<caffe::Blob<float> > responseBlob = net.blob_by_name(layerResponse);
        std::map<std::string,int>::const_iterator it = layerRelativeScales.find(layerResponse);
        assert(it != layerRelativeScales.end());
        individualVizs_.push_back(new IndividualFilterResponseViz(responseBlob,vizWidth,zoom*it->second));
        baseZooms_.push_back(it->second);
    }

}

FilterResponseViz::~FilterResponseViz() {

    for (IndividualFilterResponseViz * viz : individualVizs_) {
        delete viz;
    }

}

void FilterResponseViz::resize(const int vizWidth, const int vizHeight, const float zoom) {

    scroll_ = (scroll_ + vizHeight_/2)*zoom/zoom_ - vizHeight/2;
    vizWidth_ = vizWidth;
    vizHeight_ = vizHeight;
    zoom_ = zoom;

    for (int i=0; i<individualVizs_.size(); ++i) {
        IndividualFilterResponseViz * viz = individualVizs_[i];
        viz->resize(vizWidth,zoom*baseZooms_[i]);
    }

    scrollMax_ = -vizHeight;
    for (IndividualFilterResponseViz * viz : individualVizs_) {
        scrollMax_ += (viz->getVizHeight()+fontSize_+2*textMarginVert_);
    }
    scrollMax_ = std::max(0,scrollMax_);

    clampScroll();

}

void FilterResponseViz::incrementScroll(const int incrementValue) {

    scroll_ += incrementValue;
    clampScroll();

}

void FilterResponseViz::clampScroll() {
    scroll_ = std::max(std::min(scrollMax_,scroll_),0);
}

void FilterResponseViz::setSelection(const int selectedImage) {

    selection_ = selectedImage;
    for (IndividualFilterResponseViz * viz : individualVizs_) {
        viz->setSelection(selectedImage);
    }

}

void FilterResponseViz::setSelection(std::vector<float> & selection) {

    for (IndividualFilterResponseViz * viz : individualVizs_) {
        viz->setSelection(selection);
    }

}

void FilterResponseViz::setResponse(FeatureProjector & responseSource) {

    for (int i=0; i<individualVizs_.size(); ++i) {
        std::string blobName = responseNames_[i];
        IndividualFilterResponseViz * viz = individualVizs_[i];
        const float * response = responseSource.getResponse(blobName);
        viz->setResponse(response);
    }

}

void FilterResponseViz::getClickInfo(const int clickX, const int clickY,
                                     int & layer, int & unit) {

    const int clickYviz = clickY + scroll_;

    int runningY = 0;
    for (int i=0; i<individualVizs_.size(); ++i) {
        runningY += (fontSize_+2*textMarginVert_);
        if (runningY > clickYviz) {
            layer = i;
            unit = -1;
            return;
        }
        IndividualFilterResponseViz * viz = individualVizs_[i];
        runningY += viz->getVizHeight();
        if (runningY > clickYviz) {
            layer = i;
            unit = viz->getSelectedUnit(clickX,clickY-(runningY-viz->getVizHeight()));
            return;
        }
    }
    layer = -1;
    unit = -1;
}

void FilterResponseViz::render() {

    glColor3ub(255,255,255);
    glPushMatrix();
    glTranslatef(0,vizHeight_+scroll_,0);
    for (int i=0; i<individualVizs_.size(); ++i) {
        glColor3ub(0,0,0);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        fontManager_.printString(responseNames_[i],10,-fontSize_-textMarginVert_,fontSize_,i == embeddingLayer_ ? FontStyleBold : FontStyleRegular);
        glDisable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);

        glLineWidth(1);
        glColor3ub(128,128,128);
        glBegin(GL_LINES);
        glVertex2f(0,-fontSize_/2-textMarginVert_);
        glVertex2f(5,-fontSize_/2-textMarginVert_);
        glVertex2f(fontManager_.getStringLength(responseNames_[i],fontSize_,i == embeddingLayer_ ? FontStyleBold : FontStyleRegular) + 15,
                   -fontSize_/2-textMarginVert_);
        glVertex2f(vizWidth_,
                   -fontSize_/2-textMarginVert_);
        glEnd();

        glColor3ub(255,255,255);
        IndividualFilterResponseViz * viz = individualVizs_[i];
        glTranslatef(0,-(viz->getVizHeight()+fontSize_+2*textMarginVert_),0);
        viz->render();
    }
    glPopMatrix();

}

FilterResponseViz::IndividualFilterResponseViz::IndividualFilterResponseViz(const boost::shared_ptr<caffe::Blob<float> > responseBlob, const int vizWidth, const float zoom) :
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

    tex_.SetNearestNeighbour();

    std::fill(response_.begin(),response_.end(),(maxDataVal_ - minDataVal_)/2);
    std::cout << minDataVal_ << " -> " << maxDataVal_ << std::endl;
}

void FilterResponseViz::IndividualFilterResponseViz::resize(const int vizWidth, const float zoom) {
    zoom_ = zoom;
    vizWidth_ = vizWidth;
    responseCols_ = std::floor((vizWidth_ - border_)/(width_*zoom_ + border_));
    responseRows_ = ceilDivide(channels_,responseCols_);

    vizHeight_ = responseRows_*(zoom_*height_+border_)+border_;
}

void FilterResponseViz::IndividualFilterResponseViz::setSelection(const int selectedImage) {
    std::memcpy(response_.data(),data_ + selectedImage*channels_*height_*width_,response_.size()*sizeof(float));
}

void FilterResponseViz::IndividualFilterResponseViz::setResponse(const float * response) {
    std::memcpy(response_.data(),response,response_.size()*sizeof(float));
}

void FilterResponseViz::IndividualFilterResponseViz::setSelection(std::vector<float> & selection) {
    std::memset(response_.data(),0,response_.size()*sizeof(float));
    int n = 0;
    for (int i=0; i<selection.size(); ++i) {
        if (selection[i] == 1.f) {
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

int FilterResponseViz::IndividualFilterResponseViz::getSelectedUnit(const int clickX, const int clickY) const {

    const int row = (clickY-border_) / (zoom_*height_+border_);
    const int col = (clickX-border_) / (zoom_*width_+border_);
    const int channel = col + row*responseCols_;
    std::cout << "channel " << channel << std::endl;
    if (channel >= channels_) { return -1; }

    const int h = ((clickY-border_) - row*(zoom_*height_+border_))/zoom_;
    std::cout << "h " << h << std::endl;
    if (h >= height_) { return -1; }

    const int w = ((clickX-border_) - col*(zoom_*width_+border_))/zoom_;
    std::cout << "w " << w << std::endl;
    if (w >= width_) { return -1; }

    return w + width_*(h + height_*channel);

}


void FilterResponseViz::IndividualFilterResponseViz::render() {
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

