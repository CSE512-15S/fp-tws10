#ifndef FILTER_RESPONSE_VIZ_H
#define FILTER_RESPONSE_VIZ_H

#include <caffe/caffe.hpp>
#include <pangolin/pangolin.h>
#include "fonts/font_manager.h"

class FilterResponseViz {
private:
    class IndividualFilterResponseViz {
    public:
        IndividualFilterResponseViz(const boost::shared_ptr<caffe::Blob<float> > responseBlob, const int vizWidth, const float zoom);

        void resize(const int vizWidth, const float zoom);

        void render();

        inline int getVizWidth() const { return vizWidth_; }

        inline int getVizHeight() const { return vizHeight_; }

        inline float getVizZoom() const { return zoom_; }

        void setSelection(const int selectedImage);

        void setSelection(std::vector<bool> & selection);

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
        float maxDataVal_;
        float minDataVal_;

        std::vector<float> response_;
    };

public:
    FilterResponseViz(const caffe::Net<float> & net,
                      const std::vector<std::string> & layerResponsesToVisualize,
                      const std::map<std::string,int> & layerRelativeScales,
                      const int vizWidth,
                      FontManager & fontManager,
                      const float zoom = 1.f,
                      const int fontSize = 12);

    ~FilterResponseViz();

    void resize(const int vizWidth,
                const float zoom);

    void setSelection(const int selectedImage);

    void setSelection(std::vector<bool> & selection);

    void render();

private:
    std::vector<IndividualFilterResponseViz*> individualVizs_;
    std::vector<std::string> responseNames_;
    std::vector<int> baseZooms_;
    int fontSize_;
    int vizWidth_;
    float zoom_;
    FontManager & fontManager_;
};


#endif // FILTER_RESPONSE_VIZ_H
