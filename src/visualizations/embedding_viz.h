#ifndef EMBEDDING_VIZ_H
#define EMBEDDING_VIZ_H

#include <pangolin/pangolin.h>
#include <vector_types.h>
#include <vector_functions.h>
#include <helper_math.h>
#include <vector>

class EmbeddingViz {
public:

    EmbeddingViz(const float aspectRatio, const float minZoom,
                 const float * images,
                 const int imageWidth, const int imageHeight,
                 pangolin::GlTexture & imageTex,
                 const int overviewWidth, const int overviewHeight,
                 pangolin::GlTexture & overviewTex) :
        zoom_(1.f), scroll_(make_float2(0.f)), aspectRatio_(aspectRatio),
        images_(images), imageWidth_(imageWidth), imageHeight_(imageHeight),
        imageTex_(imageTex), minZoom_(minZoom),
        overviewImage_(overviewWidth*overviewHeight),
        overviewWidth_(overviewWidth), overviewHeight_(overviewHeight),
        overviewTex_(overviewTex), maxZoom_(1.f) { }

    void render(const float2 windowSize);

    inline float2 getViewportSize() { return zoom_*getMaxViewportSize(); }

    inline float2 getViewportCenter() { return getMaxViewportCenter() + scroll_; }

    virtual float2 getMaxViewportSize() = 0;

    virtual float2 getMaxViewportCenter() = 0;

    inline float getZoom() { return zoom_; }

    inline void setZoom(const float zoom) { zoom_ = zoom; clampZoom(); }

    inline void incrementScroll(const float2 increment) { scroll_ += increment; clampScroll(); }

    virtual int getHoveredOverPoint() = 0;

    virtual void setHoveredOverPoint(const float2 viewportPoint) = 0;

    virtual void clearHover() = 0;

    inline float2 getWindowPoint(const float2 viewportPoint, const float2 windowSize) {
        return ((viewportPoint - getViewportCenter())/getViewportSize() + make_float2(0.5))*windowSize;
    }

    inline uchar3 * getOverviewImage() { return overviewImage_.data(); }

protected:
    // -=-=-=-=-=- methods -=-=-=-=-=-
    inline void clampScroll() { scroll_ = fmaxf(getMinScroll(),fminf(scroll_,getMaxScroll())); }

    inline void clampZoom() { zoom_ = std::max(std::min(maxZoom_,zoom_),minZoom_);  clampScroll(); }

    inline float2 getMinScroll() { return -1.f*getMaxScroll(); }

    inline float2 getMaxScroll() { return 0.5f*(getMaxViewportSize() - getViewportSize()); }

    // -=-=-=-=-=- members -=-=-=-=-=-
    float aspectRatio_;
    float zoom_;
    float2 scroll_;

    float minZoom_;
    float maxZoom_;

    const float * images_;
    const int imageWidth_;
    const int imageHeight_;
    std::vector<uchar3> overviewImage_;
    const int overviewWidth_;
    const int overviewHeight_;
    pangolin::GlTexture & imageTex_;
    pangolin::GlTexture & overviewTex_;

    static constexpr float overviewSizePercent_ = 0.2;
    static constexpr float overviewZoomThreshold_ = 0.666f;
    static constexpr float pointSizeHoverMultiplier_ = 1.5f;
    static constexpr float pointSizeViewport_ = 0.015f; // TODO
};

#endif // EMBEDDING_VIZ_H
