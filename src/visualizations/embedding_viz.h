#ifndef EMBEDDING_VIZ_H
#define EMBEDDING_VIZ_H

#include <pangolin/pangolin.h>
#include <vector_types.h>
#include <vector_functions.h>
#include <helper_math.h>

class EmbeddingViz {
public:

    EmbeddingViz(const float aspectRatio, const float * images,
                 const int imageWidth, const int imageHeight,
                 pangolin::GlTexture & imageTex, const float minZoom) :
        zoom_(1.f), scroll_(make_float2(0.f)), aspectRatio_(aspectRatio),
        images_(images), imageWidth_(imageWidth), imageHeight_(imageHeight),
        imageTex_(imageTex), minZoom_(minZoom) { }

//    void render(const float2 window);

    virtual float2 getViewportSize() = 0;

    virtual float2 getViewportCenter() = 0;

    inline float getZoom() { return zoom_; }

    inline void setZoom(const float zoom) { zoom_ = zoom; clampZoom(); }

    inline void incrementScroll(const float2 increment) { scroll_ += increment; clampScroll(); }

    virtual int getHoveredOverPoint() = 0;

    virtual void setHoveredOverPoint(const float2 viewportPoint) = 0;

    virtual void clearHover() = 0;

    inline float2 getWindowPoint(const float2 viewportPoint, const float2 windowSize) {
        return ((viewportPoint - getViewportCenter())/getViewportSize() + make_float2(0.5))*windowSize;
    }

protected:
    // -=-=-=-=-=- methods -=-=-=-=-=-
    virtual float2 getMinScroll() = 0;

    virtual float2 getMaxScroll() = 0;

    inline void clampScroll() { scroll_ = fmaxf(getMinScroll(),fminf(scroll_,getMaxScroll())); }

    inline void clampZoom() { zoom_ = std::max(std::min(1.f,zoom_),minZoom_);  clampScroll(); }

    // -=-=-=-=-=- members -=-=-=-=-=-
    float aspectRatio_;
    float zoom_;
    float2 scroll_;

    float minZoom_;

    const float * images_;
    const int imageWidth_;
    const int imageHeight_;
//    const uchar3 * previewImg;
    pangolin::GlTexture & imageTex_;
//    pangolin::GlTexture & previewTex_;

};

#endif // EMBEDDING_VIZ_H
