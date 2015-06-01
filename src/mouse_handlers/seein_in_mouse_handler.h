#ifndef SEEIN_IN_MOUSE_HANDLER_H
#define SEEIN_IN_MOUSE_HANDLER_H

#include <pangolin/pangolin.h>
#include <vector_types.h>

enum SelectionMode {
    SelectionModeSingle,
    SelectionModeLasso
};

class SeeinInMouseHandler : public pangolin::Handler {
public:
    SeeinInMouseHandler(const float2 viewportSize, const float2 viewportCenter,
                       const float2 * embeddedPoints, const int nEmbeddedPoints);

    void Mouse(pangolin::View & v, pangolin::MouseButton button, int x, int y, bool pressed, int button_state);

    void PassiveMouseMotion(pangolin::View & v, int x, int y, int button_state);

    inline int getHoveredOverPoint() { return hoveredOverPoint_; }

    inline bool hasSelection() { bool retVal = hasSelection_; hasSelection_ = false; return retVal;  }

    inline void setSelectionMode(SelectionMode mode) { selectionMode_ = mode; }

    inline SelectionMode getSelectionMode() { return selectionMode_; }

    inline std::vector<float2> & getLassoPoints() { return lassoPoints_; }

    inline std::vector<bool> & getSelection() { return selection_; }

private:
    // -=-=-=-=-=- methods -=-=-=-=-=-
    int computeClosestEmbeddedPoint(const float2 queryPt);
    void computeEnclosedEmbeddedPoints();
    bool isInPolygon(const float2 pt, const std::vector<float2> & polyPoints);
    bool horizontalIntersection(float & intersectionX, const float intersectionY, const float2 start, const float2 end);

    // -=-=-=-=-=- members -=-=-=-=-=-
    float2 vpSize_;
    float2 vpCenter_;
    const float2 * embeddedPoints_;
    int nEmbeddedPoints_;

    int hoveredOverPoint_;
    bool hasSelection_;

    SelectionMode selectionMode_;
    std::vector<float2> lassoPoints_;
    std::vector<bool> selection_;
};

#endif // SEEIN_IN_MOUSE_HANDLER_H
