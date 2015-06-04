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

    inline float getZoom() { return zoom_; }

    inline void setZoom(const float zoom) { zoom_ = zoom; clampZoom(); }

    inline void incrementScroll(const float2 increment) { scroll_ += increment; clampScroll(); }

    void setHoverPoint(const float2 viewportPoint);

    int getHoveredOverPoint();

    void clearHover();

    inline float2 getNormalizedPoint(const float2 normalizedSubvizPoint, const int subvizRow, const int subvizCol) {
        return (1.f/dims_)*(make_float2(subvizCol,subvizRow) + make_float2(subvizPaddingPercent_) + (1-2*subvizPaddingPercent_)*normalizedSubvizPoint);
    }

private:
    // -=-=-=-=-=- methods -=-=-=-=-=-
    void clear();

    void clampZoom();

    void clampScroll();

    // -=-=-=-=-=- members -=-=-=-=-=-
    float aspectRatio_;
    int dims_;
    std::vector<EmbeddingSubViz *> embeddingVizs_;
    std::vector<float2 *> partialEmbeddings_;

    const float * images_;
    const int imageWidth_;
    const int imageHeight_;
    pangolin::GlTexture & imageTex_;

    float zoom_;
    float2 scroll_;

    int hoveredSubvizIndex_;

    static constexpr float subvizPaddingPercent_ = 0.05f;

};

#endif // MULTI_EMBEDDING_VIZ_H
