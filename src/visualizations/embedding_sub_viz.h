#ifndef EMBEDDING_SUB_VIZ_H
#define EMBEDDING_SUB_VIZ_H

#include <stdlib.h>
#include <vector_types.h>
#include <vector_functions.h>
#include <helper_math.h>
#include <pangolin/glsl.h>

class EmbeddingSubViz {
public:

    EmbeddingSubViz(float aspectRatio, pangolin::GlSlProgram & pointShader, float * selection) :
        aspectRatio_(aspectRatio), pointShader_(pointShader), selection_(selection), hoveredPointIndex_(-1) { }

    void setEmbedding(const float2 * embedding, uchar3 * coloring, int nEmbedded);

    void render(const float2 window, const float2 viewportSize, const float2 viewportCenter);

    inline int getNumEmbeddedPoints() { return nEmbedded_; }

    inline const float2 * getEmbedding() { return embedding_; }

    inline int getHoveredOverPoint() { return hoveredPointIndex_; }

    void setHoveredOverPoint(const float2 viewportPoint, const float maxDist);

    inline void clearHover() { hoveredPointIndex_ = -1; }

    inline float2 getMaxViewportSize() { return maxViewportSize_; }

    inline float2 getMaxViewportCenter() { return maxViewportCenter_; }

    inline float2 getNormalizedPoint(const float2 embeddingPoint) { return (embeddingPoint - getMaxViewportCenter())/getMaxViewportSize() + make_float2(0.5); }

private:
    float aspectRatio_;

    float2 maxViewportSize_;
    float2 maxViewportCenter_;
    int nEmbedded_;

    const float2 * embedding_;
    uchar3 * coloring_;
    float * selection_;

    int hoveredPointIndex_;

    pangolin::GlSlProgram & pointShader_;
};

#endif // EMBEDDING_SUB_VIZ_H
