#include "multi_embedding_view_mouse_handler.h"

MultiEmbeddingViewMouseHandler::MultiEmbeddingViewMouseHandler(MultiEmbeddingViz * viz) :
    viz_(viz),
    zoomSpeed_(1.1f) { }

void MultiEmbeddingViewMouseHandler::Mouse(pangolin::View & v, pangolin::MouseButton button, int x, int y, bool pressed, int button_state) {

    switch (button) {
        case pangolin::MouseButtonLeft:
            if (!pressed && !scrolled_) {
                if (getHoveredOverPoint() >= 0) { hasSelection_ = true; }
            } else {
                lastMouse_ = make_float2(x,y);
                scrolled_ = false;
            }
            break;
        case pangolin::MouseWheelUp:
            {
                const float2 vpPoint = getViewportPoint(v,make_float2(x,y));
                viz_->setZoom(viz_->getZoom()/zoomSpeed_);
                const float2 viewPoint = getViewPoint(v,vpPoint);
                const float2 diff = viewPoint - make_float2(x,y);
                viz_->incrementScroll(diff*(viz_->getViewportSize().x/v.GetBounds().w));
            }
            break;
        case pangolin::MouseWheelDown:
            {
                const float2 vpPoint = getViewportPoint(v,make_float2(x,y));
                viz_->setZoom(viz_->getZoom()*zoomSpeed_);
                const float2 viewPoint = getViewPoint(v,vpPoint);
                const float2 diff = viewPoint - make_float2(x,y);
                viz_->incrementScroll(diff*(viz_->getViewportSize().x/v.GetBounds().w));
            }
            break;
    }

}

void MultiEmbeddingViewMouseHandler::MouseMotion(pangolin::View & v, int x, int y, int button_state) {
    pangolin::Handler::MouseMotion(v,x,y,button_state);

    const float2 thisMouse = make_float2(x,y);
    const float2 diff = lastMouse_ - thisMouse;
    const float2 relativeScale = make_float2(viz_->getViewportSize().x/v.GetBounds().w,viz_->getViewportSize().y/v.GetBounds().h);
    const float2 embeddingDiff = relativeScale*diff;
    viz_->incrementScroll(embeddingDiff);
    viz_->clearHover();

//    viz_->setHoveredOverPoint(-1);

    lastMouse_ = thisMouse;
    scrolled_ = true;

}

void MultiEmbeddingViewMouseHandler::PassiveMouseMotion(pangolin::View & v, int x, int y, int button_state) {

    const float2 vpPoint = getViewportPoint(v,make_float2(x,y));
    viz_->setHoveredOverPoint(vpPoint);

}
