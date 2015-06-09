#include "single_embedding_view_mouse_handler.h"

#include <iostream>
#include <vector_functions.h>
#include <helper_math.h>
#include <limits>

SingleEmbeddingViewMouseHandler::SingleEmbeddingViewMouseHandler(SingleEmbeddingViz * viz) :
    EmbeddingViewMouseHandler(viz) { }

void SingleEmbeddingViewMouseHandler::Mouse(pangolin::View & v, pangolin::MouseButton button, int x, int y, bool pressed, int button_state) {
    pangolin::Handler::Mouse(v,button,x,y,pressed,button_state);

    switch (button) {
        case pangolin::MouseButtonLeft:
            if (!pressed && !scrolled_) {
                switch (selectionMode_) {
                    case SelectionModeSingle:
                        processSingleClick(button_state);
                        break;
                    case SelectionModeLasso:
                        processLassoClick(v,x,y);
                        break;
                }
            } else {
                lastMouse_ = make_float2(x,y);
                scrolled_ = false;
            }
            break;
        case pangolin::MouseButtonRight:
            clearLassoPoints();
            break;
        case pangolin::MouseWheelUp:
            zoomIn(v,x,y);
            break;
        case pangolin::MouseWheelDown:
            zoomOut(v,x,y);
            break;
    }

}

void SingleEmbeddingViewMouseHandler::MouseMotion(pangolin::View & v, int x, int y, int button_state) {
    pangolin::Handler::MouseMotion(v,x,y,button_state);

    if (selectionMode_ == SelectionModeSingle || lassoPoints_.size() == 0) {
        const float2 thisMouse = make_float2(x,y);
        const float2 diff = lastMouse_ - thisMouse;
        const float2 relativeScale = make_float2(viz_->getViewportSize().x/v.GetBounds().w,viz_->getViewportSize().y/v.GetBounds().h);
        const float2 embeddingDiff = relativeScale*diff;

        viz_->incrementScroll(embeddingDiff);
        viz_->clearHover();

        lastMouse_ = thisMouse;
        scrolled_ = true;
    }

}

void SingleEmbeddingViewMouseHandler::PassiveMouseMotion(pangolin::View & v, int x, int y, int button_state) {
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
                    if (length(vpPoint - lassoPoints_.front()) <
                            viz_->getViewportUnitsPerPixel(make_float2(v.GetBounds().w,v.GetBounds().h)).x*stickyStartThresholdPixels_) {
                        vpPoint = lassoPoints_.front();
                    }
                    lassoPoints_.back() = vpPoint;
                }
            } break;
    }
}


