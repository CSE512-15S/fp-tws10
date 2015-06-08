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
                      ScatterPlotShader & pointShader,
                      float * selection);

    ~MultiEmbeddingViz();

    void setEmbedding(const float * embedding, const int embeddingDimensions,
                      uchar3 * coloring, const int nEmbedded);

    void setEmbedding(const float * embedding, const int embeddingDimensions,
                      uchar3 * coloring, const int nEmbedded,
                      const int width, const int height,
                      const int2 receptiveField_, const int stride);

    void render(const float2 windowSize);

    inline float2 getMaxViewportSize() { return make_float2(dims_); }

    inline float2 getMaxViewportCenter() { return make_float2(0.5*dims_); }

    void setHoveredOverPoint(const float2 viewportPoint);

    int getHoveredOverPoint();

    void clearHover();

    void getEnclosedPoints(std::vector<int> & enclosedPoints, const std::vector<float2> & viewportLassoPoints);

    inline float2 getViewportPointOfSubvizPoint(const float2 normalizedSubvizPoint, const int subvizRow, const int subvizCol) {
        return (make_float2(subvizCol,subvizRow) + make_float2(subvizPaddingPercent_) + (1-2*subvizPaddingPercent_)*normalizedSubvizPoint);
    }

    inline float2 getSubvizPointOfViewportPoint(const float2 viewportPoint, const int subvizRow, const int subvizCol, EmbeddingSubViz * subviz) {
        return ((viewportPoint - make_float2(subvizCol,subvizRow) - make_float2(subvizPaddingPercent_))/(1-2*subvizPaddingPercent_) - make_float2(0.5))*
                subviz->getMaxViewportSize() + subviz->getMaxViewportCenter();
    }

    void centerOnFeature(const int unitNum);

    void updateSelectionCoarseToFine();

    void updateSelectionFineToCoarse();

    void setSubselection(const int selectedPoint);

    void setSubselection(std::vector<int> selectedPoints);

    inline int getPointImage(const int point) { return point/(width_*height_); }

    inline bool hasSubselection() { return width_ != 1 || height_ != 1; }

private:
    // -=-=-=-=-=- methods -=-=-=-=-=-
    void clear();

    void adjustZoomLimits();

    // -=-=-=-=-=- members -=-=-=-=-=-
    int dims_;
    int width_;
    int height_;
    int nEmbedded_;
    int2 receptiveField_;
    int stride_;

    std::vector<EmbeddingSubViz *> embeddingVizs_;
    std::vector<float2 *> partialEmbeddings_;

    int hoveredSubvizIndex_;

    ScatterPlotShader & pointShader_;
    float * selection_;
    uchar3 * colorCopies_;
    float * selectionCopies_;

    std::vector<float *> parallelCoordinateArrays_;

    static constexpr float subvizPaddingPercent_ = 0.05f;
    static const int maxDimsInView_ = 40;

};

#endif // MULTI_EMBEDDING_VIZ_H
