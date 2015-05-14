#ifndef SEEIN_IN_MOUSE_HANDLER_H
#define SEEIN_IN_MOUSE_HANDLER_H

#include <pangolin/pangolin.h>
#include <vector_types.h>

class SeeinInMouseHander : public pangolin::Handler {
public:
    SeeinInMouseHander(const float2 viewportSize, const float2 viewportCenter,
                       const float2 * embeddedPoints, const int nEmbeddedPoints);

    void PassiveMouseMotion(pangolin::View & v, int x, int y, int button_state);

    inline int getHoveredOverPoint() { return hoveredOverPoint_; }


private:

    int computeClosestEmbeddedPoint(const float2 queryPt);

    float2 vpSize_;
    float2 vpCenter_;
    const float2 * embeddedPoints_;
    int nEmbeddedPoints_;

    int hoveredOverPoint_;
};

#endif // SEEIN_IN_MOUSE_HANDLER_H
