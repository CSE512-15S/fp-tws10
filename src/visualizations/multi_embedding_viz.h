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
                      pangolin::GlTexture & imageTex,
                      const int overviewWidth, const int overviewHeight,
                      pangolin::GlTexture & overviewTex,
                      ScatterPlotShader & pointShader,
                      float * selection);

    ~MultiEmbeddingViz();

    void setEmbedding(const float * embedding, const int embeddingDimensions,
                      uchar3 * coloring, const int nEmbedded);

    void render(const float2 windowSize);

    inline float2 getMaxViewportSize() { return make_float2(dims_); }

    inline float2 getMaxViewportCenter() { return make_float2(0.5*dims_); }

    void setHoveredOverPoint(const float2 viewportPoint, const float2 windowSize);

    int getHoveredOverPoint();

    void clearHover();

    inline float2 getViewportPointOfSubvizPoint(const float2 normalizedSubvizPoint, const int subvizRow, const int subvizCol) {
        return (make_float2(subvizCol,subvizRow) + make_float2(subvizPaddingPercent_) + (1-2*subvizPaddingPercent_)*normalizedSubvizPoint);
    }

private:
    // -=-=-=-=-=- methods -=-=-=-=-=-
    void clear();

    // -=-=-=-=-=- members -=-=-=-=-=-
    int dims_;
    std::vector<EmbeddingSubViz *> embeddingVizs_;
    std::vector<float2 *> partialEmbeddings_;

    int hoveredSubvizIndex_;

    ScatterPlotShader & pointShader_;
    float * selection_;

    static constexpr float subvizPaddingPercent_ = 0.05f;

};

#endif // MULTI_EMBEDDING_VIZ_H
