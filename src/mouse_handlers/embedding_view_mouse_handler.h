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

    // -=-=-=-=-=- members -=-=-=-=-=-
    SelectionMode selectionMode_;
    EmbeddingViz * viz_;
};

#endif // EMBEDDING_VIEW_MOUSE_HANDLER_H
