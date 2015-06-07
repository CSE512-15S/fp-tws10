#include "tool_view_mouse_handler.h"

ToolViewMouseHandler::ToolViewMouseHandler(const int nButtons, const int nButtonCols, const int nButtonRows) : hasSelection_(false), selectedButton_(-1),
    buttonSize_(32), buttonSpacing_(2), nButtons_(nButtons), nButtonCols_(nButtonCols), nButtonRows_(nButtonRows) { }

void ToolViewMouseHandler::Mouse(pangolin::View & v, pangolin::MouseButton button, int x, int y, bool pressed, int button_state) {
    pangolin::Handler::Mouse(v,button,x,y,pressed,button_state);

    if (button == pangolin::MouseButtonLeft && !pressed) {

        hasSelection_ = true;
        selectedButton_ = -1;

        const int buttonCol = (x - v.GetBounds().l - buttonSpacing_) / (buttonSize_ + buttonSpacing_);
        const int buttonRow = (v.GetBounds().t() - buttonSpacing_ - y) / (buttonSize_ + buttonSpacing_);
        if (buttonCol >= 0 && buttonCol < nButtonCols_ && buttonRow >= 0 && buttonRow < nButtonRows_) {
            selectedButton_ = buttonCol + buttonRow*nButtonCols_;
            std::cout << "clicked " << buttonRow << ", " << buttonCol << std::endl;
        }

    }

}
