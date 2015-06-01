#include "filter_view_mouse_handler.h"

void FilterViewMouseHandler::Mouse(pangolin::View & v, pangolin::MouseButton button, int x, int y, bool pressed, int button_state) {
    pangolin::Handler::Mouse(v,button,x,y,pressed,button_state);

    std::cout << "mouse" << std::endl;

    switch(button) {
    case pangolin::MouseWheelUp:
//        std::cout << "wheel up" << std::endl;
//        std::cout << button_state << std::endl;
        viz_->incrementScroll(scrollSpeed_);
        break;
    case pangolin::MouseWheelDown:
//        std::cout << "wheel down" << std::endl;
        viz_->incrementScroll(-scrollSpeed_);
//        std::cout << button_state << std::endl;
        break;
    case pangolin::MouseButtonMiddle:
//        std::cout << "middle" << std::endl;
        break;
    case pangolin::MouseButtonLeft:
//        std::cout << "left" << std::endl;
        break;
    case pangolin::MouseButtonRight:
//        std::cout << "right" << std::endl;
        break;
    default:
        std::cout << button << std::endl;
        break;
    }


}
