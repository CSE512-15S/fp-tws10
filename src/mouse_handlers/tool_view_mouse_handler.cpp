#include "tool_view_mouse_handler.h"
\
#include <vector_functions.h>

ToolViewMouseHandler::ToolViewMouseHandler(Toolbox * toolbox) :
        toolbox_(toolbox),
        hasButtonSelection_(false), selectedButton_(NumButtons),
        hasClassSelection_(false), selectedClass_(-1),
        buttonSize_(toolbox->getButtonSize()),
        buttonSpacing_(toolbox->getButtonSpacing()),
        nButtons_(NumButtons),
        nButtonCols_(NumButtons), nButtonRows_(1) { }

void ToolViewMouseHandler::Mouse(pangolin::View & v, pangolin::MouseButton button, int x, int y, bool pressed, int button_state) {
    pangolin::Handler::Mouse(v,button,x,y,pressed,button_state);

    if (button == pangolin::MouseButtonLeft && !pressed) {

        const float2 toolboxPoint = make_float2(x - v.GetBounds().l,v.GetBounds().t() - y);
        ToolboxSection section = toolbox_->getSection(toolboxPoint);

        switch(section) {
            case ButtonSection:
                std::cout << "clicked button section" << std::endl;
                hasButtonSelection_ = true;
                selectedButton_ = toolbox_->getButton(toolboxPoint);
                break;
            case LabelSection:
                std::cout << "clicked label section" << std::endl;
                break;
        }

//        hasButtonSelection_ = true;
//        selectedButton_ = -1;

//        const int buttonCol = (x - v.GetBounds().l - buttonSpacing_) / (buttonSize_ + buttonSpacing_);
//        const int buttonRow = (v.GetBounds().t() - buttonSpacing_ - y) / (buttonSize_ + buttonSpacing_);
//        if (buttonCol >= 0 && buttonCol < nButtonCols_ && buttonRow >= 0 && buttonRow < nButtonRows_) {
//            selectedButton_ = buttonCol + buttonRow*nButtonCols_;
//            std::cout << "clicked " << buttonRow << ", " << buttonCol << std::endl;
//        }

    }

}
