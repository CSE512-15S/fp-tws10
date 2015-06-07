#ifndef TOOL_VIEW_MOUSE_HANDLER_H
#define TOOL_VIEW_MOUSE_HANDLER_H

#include <pangolin/pangolin.h>

class ToolViewMouseHandler : public pangolin::Handler {
public:

    ToolViewMouseHandler(const int nButtons, const int nButtonCols, const int nButtonRows);

    void Mouse(pangolin::View & v, pangolin::MouseButton button, int x, int y, bool pressed, int button_state);

    inline bool hasSelection() { const bool retval = hasSelection_; hasSelection_ = false; return retval; }

    inline int getSelectedButton() { return selectedButton_; }
private:

    bool hasSelection_;
    int selectedButton_;

    const int buttonSize_;
    const int buttonSpacing_;
    const int nButtons_;
    const int nButtonCols_;
    const int nButtonRows_;

};

#endif // TOOL_VIEW_MOUSE_HANDLER_H
