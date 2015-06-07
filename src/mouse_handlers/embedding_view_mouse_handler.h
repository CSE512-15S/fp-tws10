#ifndef EMBEDDING_VIEW_MOUSE_HANDLER_H
#define EMBEDDING_VIEW_MOUSE_HANDLER_H

#include <pangolin/pangolin.h>
#include "visualizations/embedding_viz.h"

enum SelectionMode {
    SelectionModeSingle,
    SelectionModeLasso
};

class EmbeddingViewMouseHandler : public pangolin::Handler {
public:

    EmbeddingViewMouseHandler(EmbeddingViz * viz);

    inline void setSelectionMode(SelectionMode mode) { selectionMode_ = mode; }

    inline SelectionMode getSelectionMode() { return selectionMode_; }

    inline std::vector<float2> & getLassoPoints() { return lassoPoints_; }

    inline void clearLassoPoints() { lassoPoints_.clear(); }

    inline bool hasSelection() { bool retVal = hasSelection_; hasSelection_ = false; return retVal;  }

    inline int getHoveredOverPoint() { return viz_->getHoveredOverPoint(); }

protected:
    // -=-=-=-=-=- methods -=-=-=-=-=-
    inline float2 getViewportPoint(pangolin::View & v, const float2 guiPoint) {
        return make_float2((guiPoint.x - v.GetBounds().l)/(float)v.GetBounds().w - 0.5,
                           (guiPoint.y - v.GetBounds().b)/(float)v.GetBounds().h - 0.5)*viz_->getViewportSize() + viz_->getViewportCenter();
    }

    inline float2 getViewPoint(pangolin::View & v, const float2 viewportPoint) {
        return ((viewportPoint - viz_->getViewportCenter()) / viz_->getViewportSize() + make_float2(0.5,0.5))*
                make_float2(v.GetBounds().w,v.GetBounds().h) + make_float2(v.GetBounds().l,v.GetBounds().b);
    }

    void zoomIn(pangolin::View & v, const int x, const int y);

    void zoomOut(pangolin::View & v, const int x, const int y);

    // -=-=-=-=-=- members -=-=-=-=-=-
    EmbeddingViz * viz_;

    SelectionMode selectionMode_;

    std::vector<float2> lassoPoints_;

    bool hasSelection_;

    float2 lastMouse_;
    bool scrolled_;

    float zoomSpeed_;

    static constexpr float stickyStartThresholdPixels_ = 10.f;
};

#endif // EMBEDDING_VIEW_MOUSE_HANDLER_H
