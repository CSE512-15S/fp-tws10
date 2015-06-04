#ifndef MULTI_EMBEDDING_VIZ_H
#define MULTI_EMBEDDING_VIZ_H

#include <vector>
#include <vector_types.h>
#include "embedding_viz.h"
#include "embedding_sub_viz.h"

class MultiEmbeddingViz : public EmbeddingViz {
public:
    MultiEmbeddingViz(const float aspectRatio, const float * images,
                      const int imageWidth, const int imageHeight,
                      pangolin::GlTexture & imageTex);

    ~MultiEmbeddingViz();

    void setEmbedding(const float * embedding, const int embeddingDimensions,
                      uchar3 * coloring, const int nEmbedded);

    void render(pangolin::View & view);

    inline float2 getViewportSize() { return make_float2(zoom_*dims_); }

    inline float2 getViewportCenter() { return scroll_ + 0.5*getViewportSize(); }

    void setHoveredOverPoint(const float2 viewportPoint);

    int getHoveredOverPoint();

    void clearHover();

    inline float2 getViewportPointOfSubvizPoint(const float2 normalizedSubvizPoint, const int subvizRow, const int subvizCol) {
        return (make_float2(subvizCol,subvizRow) + make_float2(subvizPaddingPercent_) + (1-2*subvizPaddingPercent_)*normalizedSubvizPoint);
    }

protected:

    inline float2 getMinScroll() { return make_float2(0.f); }

    inline float2 getMaxScroll() { return make_float2(dims_) - getViewportSize(); }

private:
    // -=-=-=-=-=- methods -=-=-=-=-=-
    void clear();

    // -=-=-=-=-=- members -=-=-=-=-=-
    int dims_;
    std::vector<EmbeddingSubViz *> embeddingVizs_;
    std::vector<float2 *> partialEmbeddings_;

    int hoveredSubvizIndex_;

    static constexpr float subvizPaddingPercent_ = 0.05f;

};

#endif // MULTI_EMBEDDING_VIZ_H
