#ifndef TOOL_VIEW_MOUSE_HANDLER_H
#define TOOL_VIEW_MOUSE_HANDLER_H

#include <pangolin/pangolin.h>
#include "visualizations/toolbox.h"

class ToolViewMouseHandler : public pangolin::Handler {
public:

    ToolViewMouseHandler(Toolbox * toolbox);

    void Mouse(pangolin::View & v, pangolin::MouseButton button, int x, int y, bool pressed, int button_state);

    inline bool hasButtonSelection() { const bool retval = hasButtonSelection_; hasButtonSelection_ = false; return retval; }

    inline ToolboxButton getSelectedButton() { return selectedButton_; }

    inline bool hasClassSelection() { const bool retval = hasClassSelection_; hasClassSelection_ = false; return retval; }

    inline int getSelectedClass() { return selectedClass_; }

private:

    Toolbox * toolbox_;

    bool hasClassSelection_;
    int selectedClass_;

    bool hasButtonSelection_;
    ToolboxButton selectedButton_;

    const int buttonSize_;
    const int buttonSpacing_;
    const int nButtons_;
    const int nButtonCols_;
    const int nButtonRows_;

};

#endif // TOOL_VIEW_MOUSE_HANDLER_H
