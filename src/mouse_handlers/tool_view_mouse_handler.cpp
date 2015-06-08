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

    switch(button) {
        case pangolin::MouseButtonLeft:
            if (!pressed) {

                const float2 toolboxPoint = make_float2(x - v.GetBounds().l,v.GetBounds().t() - y);
                ToolboxSection section = toolbox_->getSection(toolboxPoint);

                switch(section) {
                    case ButtonSection:
                        hasButtonSelection_ = true;
                        selectedButton_ = toolbox_->getButton(toolboxPoint);
                        break;
                    case LabelSection:
                        hasClassSelection_ = true;
                        selectedClass_ = toolbox_->getClass(toolboxPoint);
                        break;
                    case OverviewSection:
                        toolbox_->processOverviewCentering(toolboxPoint,v.GetBounds().w);
                        break;
                }

            }
            break;
        case pangolin::MouseWheelUp:
            toolbox_->processZoom(1.f/zoomSpeed_);
            break;
        case pangolin::MouseWheelDown:
            toolbox_->processZoom(zoomSpeed_);
            break;
    }

}

void ToolViewMouseHandler::MouseMotion(pangolin::View & v, int x, int y, int button_state) {

    const float2 toolboxPoint = make_float2(x - v.GetBounds().l,v.GetBounds().t() - y);
    ToolboxSection section = toolbox_->getSection(toolboxPoint);
    switch(section) {
        case OverviewSection:
            toolbox_->processOverviewCentering(toolboxPoint, v.GetBounds().w);
            break;
    }

}
