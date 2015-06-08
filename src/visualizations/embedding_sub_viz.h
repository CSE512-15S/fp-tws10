#ifndef EMBEDDING_SUB_VIZ_H
#define EMBEDDING_SUB_VIZ_H

#include <stdlib.h>
#include <vector_types.h>
#include <vector_functions.h>
#include <helper_math.h>
#include "shaders/scatter_plot_shader.h"

class EmbeddingSubViz {
public:

    EmbeddingSubViz(float aspectRatio, ScatterPlotShader & pointShader, float * selection, const float * sufficientlyLongArray) :
        aspectRatio_(aspectRatio), pointShader_(pointShader), selection_(selection), hoveredPointIndex_(-1),
        sufficientlyLongArray_(sufficientlyLongArray) { }

    void setEmbedding(const float * xCoords, const float * yCoords, const uchar3 * coloring, const int nEmbedded);

    void setEmbedding(const float * xCoords, const float * yCoords, const uchar3 * coloring, const int nEmbedded, const float2 maxViewportSize, const float2 maxViewportCenter);

    void render(const float2 window, const float2 viewportSize, const float2 viewportCenter, int maxPoints = -1);

    inline int getNumEmbeddedPoints() { return nEmbedded_; }

    inline int getHoveredOverPoint() { return hoveredPointIndex_; }

    void setHoveredOverPoint(const float2 viewportPoint, const float maxDist);

    inline void clearHover() { hoveredPointIndex_ = -1; }

    inline const float * getXCoords() { return xCoords_; }

    inline const float * getYCoords() { return yCoords_; }

    inline float2 getEmbeddedPoint(const int index) { return make_float2(getXCoords()[index],getYCoords()[index]); }

    inline float2 getMaxViewportSize() { return maxViewportSize_; }

    inline float2 getMaxViewportCenter() { return maxViewportCenter_; }

    inline float2 getNormalizedPoint(const float2 embeddingPoint) { return (embeddingPoint - getMaxViewportCenter())/getMaxViewportSize() + make_float2(0.5); }

    inline uchar3 getColoring(const int point) { return coloring_[point]; }

    void getEnclosedPoints(std::vector<int> & enclosedPoints, const std::vector<float2> & viewportLassoPoints);

    inline void setSufficientlyLongArray(const float * sufficientlyLongArray) { sufficientlyLongArray_ = sufficientlyLongArray; }

private:
    float aspectRatio_;

    float2 maxViewportSize_;
    float2 maxViewportCenter_;
    int nEmbedded_;

    const float * xCoords_;
    const float * yCoords_;
    const uchar3 * coloring_;
    const float * selection_;

    int hoveredPointIndex_;
    int nLastRendered_;

    const float * sufficientlyLongArray_;

    ScatterPlotShader & pointShader_;
};

#endif // EMBEDDING_SUB_VIZ_H
