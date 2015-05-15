#ifndef SEEIN_IN_MOUSE_HANDLER_H
#define SEEIN_IN_MOUSE_HANDLER_H

#include <pangolin/pangolin.h>
#include <vector_types.h>

class SeeinInMouseHandler : public pangolin::Handler {
public:
    SeeinInMouseHandler(const float2 viewportSize, const float2 viewportCenter,
                       const float2 * embeddedPoints, const int nEmbeddedPoints);

    void Mouse(pangolin::View & v, pangolin::MouseButton button, int x, int y, bool pressed, int button_state);

    void PassiveMouseMotion(pangolin::View & v, int x, int y, int button_state);

    inline int getHoveredOverPoint() { return hoveredOverPoint_; }

    inline bool hasClicked() { bool retVal = hasClicked_; hasClicked_ = false; return retVal;  }

private:

    int computeClosestEmbeddedPoint(const float2 queryPt);

    float2 vpSize_;
    float2 vpCenter_;
    const float2 * embeddedPoints_;
    int nEmbeddedPoints_;

    int hoveredOverPoint_;
    bool hasClicked_;
};

#endif // SEEIN_IN_MOUSE_HANDLER_H
