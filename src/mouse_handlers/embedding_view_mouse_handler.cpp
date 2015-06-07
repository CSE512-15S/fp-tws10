#include "embedding_view_mouse_handler.h"

#include <iostream>
#include <vector_functions.h>
#include <helper_math.h>
#include <limits>

EmbeddingViewMouseHandler::EmbeddingViewMouseHandler(SingleEmbeddingViz * viz) :
    viz_(viz), hasSelection_(false), selectionMode_(SelectionModeSingle),
    zoomSpeed_(1.1f) { }

void EmbeddingViewMouseHandler::Mouse(pangolin::View & v, pangolin::MouseButton button, int x, int y, bool pressed, int button_state) {
    pangolin::Handler::Mouse(v,button,x,y,pressed,button_state);

    switch (button) {
        case pangolin::MouseButtonLeft:
            if (!pressed && !scrolled_) {
                switch (selectionMode_) {
                    case SelectionModeSingle:
//                        if (getHoveredOverPoint() >= 0) { hasSelection_ = true; }
                        hasSelection_ = true;
                        break;
                    case SelectionModeLasso:
                        {
                            float2 vpPoint = getViewportPoint(v,make_float2(x,y));
                            if (lassoPoints_.size() == 0) {
                                // first lasso point - add the click, and then another point for the temporary next point
                                lassoPoints_.push_back(vpPoint);
                                lassoPoints_.push_back(vpPoint);
                            } else if (length(vpPoint - lassoPoints_.front()) < stickStartThreshold_) {
                                // lasso is complete - close the loop
                                lassoPoints_.back() = lassoPoints_.front();
//                                computeEnclosedEmbeddedPoints();
                                hasSelection_ = true;
                            } else {
                                // latch the current point
                                lassoPoints_.push_back(vpPoint);
                            }
                        }
                        break;
                }
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

void EmbeddingViewMouseHandler::MouseMotion(pangolin::View & v, int x, int y, int button_state) {
    pangolin::Handler::MouseMotion(v,x,y,button_state);

    const float2 thisMouse = make_float2(x,y);
    const float2 diff = lastMouse_ - thisMouse;
//    std::cout << "pixel diff: " << diff.x << ", " << diff.y << std::endl;
    const float2 relativeScale = make_float2(viz_->getViewportSize().x/v.GetBounds().w,viz_->getViewportSize().y/v.GetBounds().h);
    const float2 embeddingDiff = relativeScale*diff;
//    std::cout << "embedding diff: " << embeddingDiff.x << ", " << embeddingDiff.y << std::endl;

    viz_->incrementScroll(embeddingDiff);
    viz_->clearHover();

    lastMouse_ = thisMouse;
    scrolled_ = true;

}

void EmbeddingViewMouseHandler::PassiveMouseMotion(pangolin::View & v, int x, int y, int button_state) {
    pangolin::Handler::PassiveMouseMotion(v,x,y,button_state);

    switch (selectionMode_) {
    case SelectionModeSingle:
    {
        float2 vpPoint = getViewportPoint(v,make_float2(x,y));
        viz_->setHoveredOverPoint(vpPoint);
    } break;
    case SelectionModeLasso:
    {
        if (lassoPoints_.size() > 0) {
            float2 vpPoint = getViewportPoint(v,make_float2(x,y));
            if (length(vpPoint - lassoPoints_.front()) < 0.1) {
                vpPoint = lassoPoints_.front();
            }
            lassoPoints_.back() = vpPoint;
        }
    } break;
    }
}


