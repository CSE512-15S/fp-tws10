#ifndef FILTER_VIEW_MOUSE_HANDLER_H
#define FILTER_VIEW_MOUSE_HANDLER_H

#include <pangolin/pangolin.h>
#include "visualizations/filter_response_viz.h"

class FilterViewMouseHandler : public pangolin::Handler {
public:
    FilterViewMouseHandler(FilterResponseViz * viz) : viz_(viz) { }

    void Mouse(pangolin::View & v, pangolin::MouseButton button, int x, int y, bool pressed, int button_state);

private:
    FilterResponseViz * viz_;
};

#endif // FILTER_VIEW_MOUSE_HANDLER_H
