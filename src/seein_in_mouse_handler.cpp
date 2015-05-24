#include "seein_in_mouse_handler.h"

#include <iostream>
#include <vector_functions.h>
#include <helper_math.h>
#include <limits>

SeeinInMouseHandler::SeeinInMouseHandler(const float2 viewportSize, const float2 viewportCenter,
                                       const float2 * embeddedPoints, const int nEmbeddedPoints) :
    vpSize_(viewportSize), vpCenter_(viewportCenter),
    embeddedPoints_(embeddedPoints), nEmbeddedPoints_(nEmbeddedPoints),
    hasSelection_(false), selectionMode_(SelectionModeSingle) { }

void SeeinInMouseHandler::Mouse(pangolin::View & v, pangolin::MouseButton button, int x, int y, bool pressed, int button_state) {
    pangolin::Handler::Mouse(v,button,x,y,pressed,button_state);

    switch (selectionMode_) {
    case SelectionModeSingle:
        if (!pressed && button == pangolin::MouseButtonLeft && hoveredOverPoint_ >= 0) { hasSelection_ = true; }
        break;
    case SelectionModeLasso:
        if (!pressed && button == pangolin::MouseButtonLeft) {
            std::cout << "lasso click" << std::endl;
            float2 vpPoint = make_float2((x - v.GetBounds().l)/(float)v.GetBounds().w - 0.5,
                                         (y - v.GetBounds().b)/(float)v.GetBounds().h - 0.5)*vpSize_ + vpCenter_;
            if (lassoPoints_.size() == 0) {
                lassoPoints_.push_back(vpPoint);
                lassoPoints_.push_back(vpPoint);
            } else if (length(vpPoint - lassoPoints_.front()) < 0.1) {
                lassoPoints_.clear();
            } else {
                lassoPoints_.push_back(vpPoint);
            }
        }
        break;
    }
}

void SeeinInMouseHandler::PassiveMouseMotion(pangolin::View & v, int x, int y, int button_state) {
    pangolin::Handler::PassiveMouseMotion(v,x,y,button_state);

    switch (selectionMode_) {
    case SelectionModeSingle:
    {
        float2 vpPoint = make_float2((x - v.GetBounds().l)/(float)v.GetBounds().w - 0.5,
                                     (y - v.GetBounds().b)/(float)v.GetBounds().h - 0.5)*vpSize_ + vpCenter_;
        hoveredOverPoint_ = computeClosestEmbeddedPoint(vpPoint);
    } break;
    case SelectionModeLasso:
    {
        if (lassoPoints_.size() > 0) {
            float2 vpPoint = make_float2((x - v.GetBounds().l)/(float)v.GetBounds().w - 0.5,
                                         (y - v.GetBounds().b)/(float)v.GetBounds().h - 0.5)*vpSize_ + vpCenter_;
            if (length(vpPoint - lassoPoints_.front()) < 0.1) {
                vpPoint = lassoPoints_.front();
            }
            lassoPoints_.back() = vpPoint;
        }
    } break;
    }
}

int SeeinInMouseHandler::computeClosestEmbeddedPoint(const float2 queryPt) {

    int closestPoint = -1;
    float closestDist = std::numeric_limits<float>::infinity();
    for (int i=0; i<nEmbeddedPoints_; ++i) {
        float dist = length(queryPt - embeddedPoints_[i]);
        if (dist < closestDist) {
            closestDist = dist;
            closestPoint = i;
        }
    }

    return closestDist < 0.05 ? closestPoint : -1;

}
