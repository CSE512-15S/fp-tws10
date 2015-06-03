#include "embedding_view_mouse_handler.h"

#include <iostream>
#include <vector_functions.h>
#include <helper_math.h>
#include <limits>

EmbeddingViewMouseHandler::EmbeddingViewMouseHandler(EmbeddingViz * viz) :
    viz_(viz), hasSelection_(false), selectionMode_(SelectionModeSingle),
    zoomSpeed_(1.1f) { }

void EmbeddingViewMouseHandler::Mouse(pangolin::View & v, pangolin::MouseButton button, int x, int y, bool pressed, int button_state) {
    pangolin::Handler::Mouse(v,button,x,y,pressed,button_state);

    switch (button) {
        case pangolin::MouseButtonLeft:
            if (!pressed && !scrolled_) {
                switch (selectionMode_) {
                    case SelectionModeSingle:
                        if (hoveredOverPoint_ >= 0) { hasSelection_ = true; }
                        break;
                    case SelectionModeLasso:
                        {
//                            std::cout << "lasso click" << std::endl;
                            float2 vpPoint = getViewportPoint(v,make_float2(x,y));
                            if (lassoPoints_.size() == 0) {
                                lassoPoints_.push_back(vpPoint);
                                lassoPoints_.push_back(vpPoint);
                            } else if (length(vpPoint - lassoPoints_.front()) < 0.1) {
                                computeEnclosedEmbeddedPoints();
                                lassoPoints_.clear();
                                hasSelection_ = true;
                            } else {
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
            viz_->setZoom(viz_->getZoom()/zoomSpeed_);
            break;
        case pangolin::MouseWheelDown:
            viz_->setZoom(viz_->getZoom()*zoomSpeed_);
            break;
    }


}

void EmbeddingViewMouseHandler::MouseMotion(pangolin::View & v, int x, int y, int button_state) {
    pangolin::Handler::MouseMotion(v,x,y,button_state);

    const float2 thisMouse = make_float2(x,y);
    const float2 diff = lastMouse_ - thisMouse;
//    std::cout << "pixel diff: " << diff.x << ", " << diff.y << std::endl;
    const float relativeScale = viz_->getViewportSize().x/v.GetBounds().w;
    const float2 embeddingDiff = relativeScale*diff;
//    std::cout << "embedding diff: " << embeddingDiff.x << ", " << embeddingDiff.y << std::endl;

    viz_->incrementScroll(embeddingDiff);
    viz_->setHoveredOverPoint(-1);

    lastMouse_ = thisMouse;
    scrolled_ = true;

}

void EmbeddingViewMouseHandler::PassiveMouseMotion(pangolin::View & v, int x, int y, int button_state) {
    pangolin::Handler::PassiveMouseMotion(v,x,y,button_state);

    switch (selectionMode_) {
    case SelectionModeSingle:
    {
        float2 vpPoint = getViewportPoint(v,make_float2(x,y));
        hoveredOverPoint_ = computeClosestEmbeddedPoint(vpPoint);
        viz_->setHoveredOverPoint(hoveredOverPoint_);
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

int EmbeddingViewMouseHandler::computeClosestEmbeddedPoint(const float2 queryPt) {

    int closestPoint = -1;
    float closestDist = std::numeric_limits<float>::infinity();
    for (int i=0; i<viz_->getNumEmbeddedPoints(); ++i) {
        float dist = length(queryPt - viz_->getEmbedding()[i]);
        if (dist < closestDist) {
            closestDist = dist;
            closestPoint = i;
        }
    }

    return closestDist < 0.05 ? closestPoint : -1;

}

void EmbeddingViewMouseHandler::computeEnclosedEmbeddedPoints() {

    std::cout << "computing enclosure" << std::endl;
    for (int i=0; i<viz_->getNumEmbeddedPoints(); ++i) {
        selection_[i] = isInPolygon(viz_->getEmbedding()[i],lassoPoints_);
    }

}

bool EmbeddingViewMouseHandler::isInPolygon(const float2 pt, const std::vector<float2> & polyPoints) {

    int intersectionsLeft = 0;
    int intersectionsRight = 0;

    for (int i=0; i<polyPoints.size(); ++i) {
        float intersectionX;
        if (horizontalIntersection(intersectionX,pt.y,polyPoints[i],polyPoints[(i+1)%polyPoints.size()])) {
            if (intersectionX < pt.x) { ++intersectionsLeft; }
            else { ++intersectionsRight; }
        }
    }
    bool oddLeft = (intersectionsLeft % 2) == 1;
    bool oddRight = (intersectionsRight % 2) == 1;
    if (oddLeft != oddRight) {
        std::cerr << "oddLeft != oddRight" << std::endl;
    }
    return oddLeft;

}

bool EmbeddingViewMouseHandler::horizontalIntersection(float & intersectionX, const float intersectionY, const float2 start, const float2 end) {

    if ( (start.y > intersectionY && end.y >= intersectionY) || (start.y < intersectionY && end.y <= intersectionY) ) {
        return false;
    }
    if (start.x == end.x) {
        intersectionX = end.x;
    } else if (start.x < end.x) {
        double dx = end.x - start.x;
        double dy = end.y - start.y;
        intersectionX = start.x + dx*( (intersectionY - start.y)/dy );
    } else {
        double dx = start.x - end.x;
        double dy = start.y - end.y;
        intersectionX = end.x + dx*( (intersectionY - end.y)/dy );
    }
    return true;

}
