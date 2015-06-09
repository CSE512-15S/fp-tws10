#include "embedding_view_mouse_handler.h"

EmbeddingViewMouseHandler::EmbeddingViewMouseHandler(EmbeddingViz * viz) :
    selectionMode_(SelectionModeSingle),
    viz_(viz),
    hasSelection_(false),
    appendSelection_(false),
    zoomSpeed_(1.1f)  { }

void EmbeddingViewMouseHandler::zoomIn(pangolin::View & v, const int x, const int y) {
    const float2 vpPoint = getViewportPoint(v,make_float2(x,y));
    viz_->setZoom(viz_->getZoom()/zoomSpeed_);
    const float2 viewPoint = getViewPoint(v,vpPoint);
    const float2 diff = viewPoint - make_float2(x,y);
    viz_->incrementScroll(diff*(viz_->getViewportSize().x/v.GetBounds().w));
}

void EmbeddingViewMouseHandler::zoomOut(pangolin::View & v, const int x, const int y) {
    const float2 vpPoint = getViewportPoint(v,make_float2(x,y));
    viz_->setZoom(viz_->getZoom()*zoomSpeed_);
    const float2 viewPoint = getViewPoint(v,vpPoint);
    const float2 diff = viewPoint - make_float2(x,y);
    viz_->incrementScroll(diff*(viz_->getViewportSize().x/v.GetBounds().w));
}

void EmbeddingViewMouseHandler::processLassoClick(pangolin::View & v, const int x, const int y) {
    float2 vpPoint = getViewportPoint(v,make_float2(x,y));
    if (lassoPoints_.size() == 0) {
        // first lasso point - add the click, and then another point for the temporary next point
        lassoPoints_.push_back(vpPoint);
        lassoPoints_.push_back(vpPoint);
    } else if (lassoPoints_.size() > 2 &&
               length(vpPoint - lassoPoints_.front()) <
               viz_->getViewportUnitsPerPixel(make_float2(v.GetBounds().w,v.GetBounds().h)).x*stickyStartThresholdPixels_) {
        // lasso is complete - close the loop
        lassoPoints_.back() = lassoPoints_.front();
        hasSelection_ = true;
    } else {
        // latch the current point
        lassoPoints_.push_back(vpPoint);
    }
}

void EmbeddingViewMouseHandler::processSingleClick(int buttonState) {
    hasSelection_ = true;
    appendSelection_ = ((buttonState & pangolin::KeyModifierShift) != 0);
    if (appendSelection_) {
        std::cout << "shift click" << std::endl;
    }
    std::cout << "..." << std::endl;
}
