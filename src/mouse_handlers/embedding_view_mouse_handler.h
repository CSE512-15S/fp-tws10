#ifndef SEEIN_IN_MOUSE_HANDLER_H
#define SEEIN_IN_MOUSE_HANDLER_H

#include <pangolin/pangolin.h>
#include <vector_types.h>
#include <vector_functions.h>
#include <helper_math.h>
#include "visualizations/embedding_viz.h"

enum SelectionMode {
    SelectionModeSingle,
    SelectionModeLasso
};

class EmbeddingViewMouseHandler : public pangolin::Handler {
public:
    EmbeddingViewMouseHandler(EmbeddingViz * viz);

    void Mouse(pangolin::View & v, pangolin::MouseButton button, int x, int y, bool pressed, int button_state);

    void MouseMotion(pangolin::View & v, int x, int y, int button_state);

    void PassiveMouseMotion(pangolin::View & v, int x, int y, int button_state);

    inline int getHoveredOverPoint() { return hoveredOverPoint_; }

    inline bool hasSelection() { bool retVal = hasSelection_; hasSelection_ = false; return retVal;  }

    inline void setSelectionMode(SelectionMode mode) { selectionMode_ = mode; }

    inline SelectionMode getSelectionMode() { return selectionMode_; }

    inline std::vector<float2> & getLassoPoints() { return lassoPoints_; }

    inline std::vector<bool> & getSelection() { return selection_; }

private:
    // -=-=-=-=-=- methods -=-=-=-=-=-
    int computeClosestEmbeddedPoint(const float2 queryPt);
    void computeEnclosedEmbeddedPoints();
    bool isInPolygon(const float2 pt, const std::vector<float2> & polyPoints);
    bool horizontalIntersection(float & intersectionX, const float intersectionY, const float2 start, const float2 end);

    inline float2 getViewportPoint(pangolin::View & v, const float2 guiPoint) {
        return make_float2((guiPoint.x - v.GetBounds().l)/(float)v.GetBounds().w - 0.5,
                           (guiPoint.y - v.GetBounds().b)/(float)v.GetBounds().h - 0.5)*viz_->getViewportSize() + viz_->getViewportCenter();
    }

    inline float2 getViewPoint(pangolin::View & v, const float2 viewportPoint) {
        return ((viewportPoint - viz_->getViewportCenter()) / viz_->getViewportSize() + make_float2(0.5,0.5))*
                make_float2(v.GetBounds().w,v.GetBounds().h) + make_float2(v.GetBounds().l,v.GetBounds().b);
    }

    // -=-=-=-=-=- members -=-=-=-=-=-
    EmbeddingViz * viz_;

    int hoveredOverPoint_;
    bool hasSelection_;

    SelectionMode selectionMode_;
    std::vector<float2> lassoPoints_;
    std::vector<bool> selection_;

    float2 lastMouse_;
    bool scrolled_;

    float zoomSpeed_;
};

#endif // SEEIN_IN_MOUSE_HANDLER_H
