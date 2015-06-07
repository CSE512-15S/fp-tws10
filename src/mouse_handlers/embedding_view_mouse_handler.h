#ifndef SEEIN_IN_MOUSE_HANDLER_H
#define SEEIN_IN_MOUSE_HANDLER_H

#include <pangolin/pangolin.h>
#include <vector_types.h>
#include <vector_functions.h>
#include <helper_math.h>
#include "visualizations/single_embedding_viz.h"

enum SelectionMode {
    SelectionModeSingle,
    SelectionModeLasso
};

class EmbeddingViewMouseHandler : public pangolin::Handler {
public:
    EmbeddingViewMouseHandler(SingleEmbeddingViz * viz);

    void Mouse(pangolin::View & v, pangolin::MouseButton button, int x, int y, bool pressed, int button_state);

    void MouseMotion(pangolin::View & v, int x, int y, int button_state);

    void PassiveMouseMotion(pangolin::View & v, int x, int y, int button_state);

    inline int getHoveredOverPoint() { return viz_->getHoveredOverPoint(); }

    inline bool hasSelection() { bool retVal = hasSelection_; hasSelection_ = false; return retVal;  }

    inline void setSelectionMode(SelectionMode mode) { selectionMode_ = mode; }

    inline SelectionMode getSelectionMode() { return selectionMode_; }

    inline std::vector<float2> & getLassoPoints() { return lassoPoints_; }

    inline void clearLassoPoints() { lassoPoints_.clear(); }

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
    SingleEmbeddingViz * viz_;

    bool hasSelection_;

    SelectionMode selectionMode_;
    std::vector<float2> lassoPoints_;

    float2 lastMouse_;
    bool scrolled_;

    float zoomSpeed_;

    static constexpr float stickStartThreshold_ = 0.1f;

};

#endif // SEEIN_IN_MOUSE_HANDLER_H
