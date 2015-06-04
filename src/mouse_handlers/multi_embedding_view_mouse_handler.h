#ifndef MULTI_EMBEDDING_VIEW_MOUSE_HANDLER_H
#define MULTI_EMBEDDING_VIEW_MOUSE_HANDLER_H

#include "visualizations/multi_embedding_viz.h"

#include <helper_math.h>

class MultiEmbeddingViewMouseHandler : public pangolin::Handler {
public:

    MultiEmbeddingViewMouseHandler(MultiEmbeddingViz * viz);

    void Mouse(pangolin::View & v, pangolin::MouseButton button, int x, int y, bool pressed, int button_state);

    void MouseMotion(pangolin::View & v, int x, int y, int button_state);

    void PassiveMouseMotion(pangolin::View & v, int x, int y, int button_state);

    inline bool hasSelection() { bool retVal = hasSelection_; hasSelection_ = false; return retVal;  }

    inline int getHoveredOverPoint() { return viz_->getHoveredOverPoint(); }

private:
    // -=-=-=-=-=- methods -=-=-=-=-=-
    inline float2 getViewportPoint(pangolin::View & v, const float2 guiPoint) {
        return make_float2((guiPoint.x - v.GetBounds().l)/(float)v.GetBounds().w - 0.5,
                           (guiPoint.y - v.GetBounds().b)/(float)v.GetBounds().h - 0.5)*viz_->getViewportSize() + viz_->getViewportCenter();
    }

    inline float2 getViewPoint(pangolin::View & v, const float2 viewportPoint) {
        return ((viewportPoint - viz_->getViewportCenter()) / viz_->getViewportSize() + make_float2(0.5,0.5))*
                make_float2(v.GetBounds().w,v.GetBounds().h) + make_float2(v.GetBounds().l,v.GetBounds().b);
    }

    // -=-=-=-=-=- members -=-=-=-=-=-
    MultiEmbeddingViz * viz_;

    bool hasSelection_;

    float2 lastMouse_;
    bool scrolled_;

    float zoomSpeed_;
};

#endif // MULTI_EMBEDDING_VIEW_MOUSE_HANDLER_H
