#ifndef EMBEDDING_VIZ_H
#define EMBEDDING_VIZ_H

#include <pangolin/pangolin.h>
#include <vector_types.h>

class EmbeddingViz {
public:

    EmbeddingViz(const float aspectRatio);

    void setEmbedding(const float2 * embedding, uchar3 * coloring, int nEmbedded);

    void render(const float2 window);

    inline float2 getViewportSize() { return viewportSize_; }

    inline float2 getViewportCenter() { return viewportCenter_; }

    inline int getNumEmbeddedPoints() { return nEmbedded_; }

    inline const float2 * getEmbedding() { return embedding_; }

private:
    float aspectRatio_;
    float2 viewportSize_;
    float2 viewportCenter_;

    int nEmbedded_;
    const float2 * embedding_;
    uchar3 * coloring_;

};

#endif // EMBEDDING_VIZ_H
