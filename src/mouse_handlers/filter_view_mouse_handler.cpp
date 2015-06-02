#include "filter_view_mouse_handler.h"

void FilterViewMouseHandler::Mouse(pangolin::View & v, pangolin::MouseButton button, int x, int y, bool pressed, int button_state) {
    pangolin::Handler::Mouse(v,button,x,y,pressed,button_state);

//    std::cout << "mouse" << std::endl;

    switch(button) {
    case pangolin::MouseWheelUp:
//        std::cout << "wheel up" << std::endl;
//        std::cout << button_state << std::endl;
        viz_->incrementScroll(-scrollSpeed_);
        break;
    case pangolin::MouseWheelDown:
//        std::cout << "wheel down" << std::endl;
        viz_->incrementScroll(scrollSpeed_);
//        std::cout << button_state << std::endl;
        break;
    case pangolin::MouseButtonMiddle:
//        std::cout << "middle" << std::endl;
        break;
    case pangolin::MouseButtonLeft:
//        std::cout << "left" << std::endl;
        if (pressed) {
            int clickX = x - v.GetBounds().l;
            int clickY = v.GetBounds().t() - y;
            viz_->getClickInfo(clickX,clickY,selectedLayer_,selectedUnit_);
//            std::cout << "click" << clickX << ", " << clickY << std::endl;
//            std::cout << "layer " << selectedLayer_ << ", unit " << selectedUnit_ << std::endl;
            if (selectedUnit_ == -1) {
                hasLayerSelection_ = true;
            } else {
                hasUnitSelection_ = true;
            }
        }
        break;
    case pangolin::MouseButtonRight:
//        std::cout << "right" << std::endl;
        break;
    default:
        std::cout << button << std::endl;
        break;
    }


}
