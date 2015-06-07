#ifndef SINGLE_EMBEDDING_VIEW_MOUSE_HANDLER_H
#define SINGLE_EMBEDDING_VIEW_MOUSE_HANDLER_H

#include <stdlib.h>
#include <vector_types.h>
#include <vector_functions.h>
#include <helper_math.h>
#include "visualizations/single_embedding_viz.h"
#include "embedding_view_mouse_handler.h"

class SingleEmbeddingViewMouseHandler : public EmbeddingViewMouseHandler {
public:
    SingleEmbeddingViewMouseHandler(SingleEmbeddingViz * viz);

    void Mouse(pangolin::View & v, pangolin::MouseButton button, int x, int y, bool pressed, int button_state);

    void MouseMotion(pangolin::View & v, int x, int y, int button_state);

    void PassiveMouseMotion(pangolin::View & v, int x, int y, int button_state);

    inline int getHoveredOverPoint() { return viz_->getHoveredOverPoint(); }

    inline bool hasSelection() { bool retVal = hasSelection_; hasSelection_ = false; return retVal;  }

    inline std::vector<float2> & getLassoPoints() { return lassoPoints_; }

    inline void clearLassoPoints() { lassoPoints_.clear(); }

private:

    bool hasSelection_;

    std::vector<float2> lassoPoints_;

    float2 lastMouse_;
    bool scrolled_;

    float zoomSpeed_;

    static constexpr float stickStartThreshold_ = 0.1f;

};

#endif // SINGLE_EMBEDDING_VIEW_MOUSE_HANDLER_H
