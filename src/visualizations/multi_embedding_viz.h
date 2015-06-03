#ifndef MULTI_EMBEDDING_VIZ_H
#define MULTI_EMBEDDING_VIZ_H

#include <vector>
#include <vector_types.h>
#include "embedding_viz.h"

class MultiEmbeddingViz {
public:
    MultiEmbeddingViz(const float aspectRatio, const float * images,
                      const int imageWidth, const int imageHeight,
                      pangolin::GlTexture & imageTex);

    ~MultiEmbeddingViz();

    void setEmbedding(const float * embedding, const int embeddingDimensions,
                      uchar3 * coloring, int nEmbedded);

    void render(pangolin::View & view);

private:
    // -=-=-=-=-=- methods -=-=-=-=-=-
    void clear();

    // -=-=-=-=-=- members -=-=-=-=-=-
    float aspectRatio_;
    int dims_;
    std::vector<EmbeddingViz *> embeddingVizs_;
    std::vector<float2 *> partialEmbeddings_;

    const float * images_;
    const int imageWidth_;
    const int imageHeight_;
    pangolin::GlTexture & imageTex_;

};

#endif // MULTI_EMBEDDING_VIZ_H
