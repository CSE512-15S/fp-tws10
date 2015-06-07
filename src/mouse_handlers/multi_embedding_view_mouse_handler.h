#ifndef MULTI_EMBEDDING_VIEW_MOUSE_HANDLER_H
#define MULTI_EMBEDDING_VIEW_MOUSE_HANDLER_H

#include "embedding_view_mouse_handler.h"
#include "visualizations/multi_embedding_viz.h"

#include <helper_math.h>

class MultiEmbeddingViewMouseHandler : public EmbeddingViewMouseHandler {
public:

    MultiEmbeddingViewMouseHandler(MultiEmbeddingViz * viz);

    void Mouse(pangolin::View & v, pangolin::MouseButton button, int x, int y, bool pressed, int button_state);

    void MouseMotion(pangolin::View & v, int x, int y, int button_state);

    void PassiveMouseMotion(pangolin::View & v, int x, int y, int button_state);

    inline bool hasSelection() { bool retVal = hasSelection_; hasSelection_ = false; return retVal;  }

    inline int getHoveredOverPoint() { return viz_->getHoveredOverPoint(); }

private:

    bool hasSelection_;

    float2 lastMouse_;
    bool scrolled_;

    float zoomSpeed_;
};

#endif // MULTI_EMBEDDING_VIEW_MOUSE_HANDLER_H
