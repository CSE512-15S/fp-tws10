#include "embedding_view_mouse_handler.h"

EmbeddingViewMouseHandler::EmbeddingViewMouseHandler(EmbeddingViz * viz) :
    selectionMode_(SelectionModeSingle),
    viz_(viz),
    hasSelection_(false),
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
