#include "seein_in_mouse_handler.h"

#include <iostream>
#include <vector_functions.h>
#include <helper_math.h>
#include <limits>

SeeinInMouseHandler::SeeinInMouseHandler(const float2 viewportSize, const float2 viewportCenter,
                                       const float2 * embeddedPoints, const int nEmbeddedPoints) :
    vpSize_(viewportSize), vpCenter_(viewportCenter),
    embeddedPoints_(embeddedPoints), nEmbeddedPoints_(nEmbeddedPoints),
    hasClicked_(false) { }

void SeeinInMouseHandler::Mouse(pangolin::View & v, pangolin::MouseButton button, int x, int y, bool pressed, int button_state) {
    pangolin::Handler::Mouse(v,button,x,y,pressed,button_state);

    if (!pressed && button == pangolin::MouseButtonLeft && hoveredOverPoint_ >= 0) { hasClicked_ = true; }
}

void SeeinInMouseHandler::PassiveMouseMotion(pangolin::View & v, int x, int y, int button_state) {
    pangolin::Handler::PassiveMouseMotion(v,x,y,button_state);

    float2 vpPoint = make_float2((x - v.GetBounds().l)/(float)v.GetBounds().w - 0.5,
                                 (y - v.GetBounds().b)/(float)v.GetBounds().h - 0.5)*vpSize_ + vpCenter_;
    hoveredOverPoint_ = computeClosestEmbeddedPoint(vpPoint);
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
