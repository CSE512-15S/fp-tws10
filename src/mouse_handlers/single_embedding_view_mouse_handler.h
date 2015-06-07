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

};

#endif // SINGLE_EMBEDDING_VIEW_MOUSE_HANDLER_H
