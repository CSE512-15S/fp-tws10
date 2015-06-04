#ifndef EMBEDDING_SUB_VIZ_H
#define EMBEDDING_SUB_VIZ_H

#include <vector_types.h>

class EmbeddingSubViz {
public:

    EmbeddingSubViz(float aspectRatio) : aspectRatio_(aspectRatio) { }

    void setEmbedding(const float2 * embedding, uchar3 * coloring, int nEmbedded);

    void render(const float2 window, const float2 viewportSize, const float2 viewportCenter);

    inline int getNumEmbeddedPoints() { return nEmbedded_; }

    inline const float2 * getEmbedding() { return embedding_; }

    inline int getHoveredOverPoint() { return hoveredPointIndex_; }

    void setHoveredOverPoint(const float2 viewportPoint);

    inline void clearHover() { hoveredPointIndex_ = -1; }

    inline float2 getMaxViewportSize() { return maxViewportSize_; }

    inline float2 getMaxViewportCenter() { return maxViewportCenter_; }

private:
    float aspectRatio_;

    float2 maxViewportSize_;
    float2 maxViewportCenter_;
    int nEmbedded_;

    const float2 * embedding_;
    uchar3 * coloring_;

    int hoveredPointIndex_;
};

#endif // EMBEDDING_SUB_VIZ_H
