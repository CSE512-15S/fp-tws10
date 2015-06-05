#ifndef FILTER_VIEW_MOUSE_HANDLER_H
#define FILTER_VIEW_MOUSE_HANDLER_H

#include <pangolin/pangolin.h>
#include "visualizations/filter_response_viz.h"

class FilterViewMouseHandler : public pangolin::Handler {
public:
    FilterViewMouseHandler(FilterResponseViz * viz) : viz_(viz), scrollSpeed_(20), selectedLayer_(-1) { }

    void Mouse(pangolin::View & v, pangolin::MouseButton button, int x, int y, bool pressed, int button_state);

    inline bool hasLayerSelection() { bool retVal = hasLayerSelection_; hasLayerSelection_ = false; return retVal; }

    inline bool hasUnitSelection() { bool retVal = hasUnitSelection_; hasUnitSelection_ = false; return retVal; }

    inline int getSelectedLayer() { return selectedLayer_; }

    inline int getSelectedUnit() { return selectedUnit_; }

private:
    FilterResponseViz * viz_;
    int scrollSpeed_;
    int selectedLayer_;
    int selectedUnit_;
    bool hasLayerSelection_;
    bool hasUnitSelection_;
};

#endif // FILTER_VIEW_MOUSE_HANDLER_H
