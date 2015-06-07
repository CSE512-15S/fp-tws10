#ifndef SINGLE_EMBEDDING_VIZ_H
#define SINGLE_EMBEDDING_VIZ_H

#include <pangolin/pangolin.h>
#include <pangolin/glsl.h>
#include <vector_types.h>
#include <vector_functions.h>
#include <helper_math.h>

#include "embedding_viz.h"
#include "embedding_sub_viz.h"

class SingleEmbeddingViz : public EmbeddingViz {
public:

    SingleEmbeddingViz(const float aspectRatio, const float * images,
                       const int imageWidth, const int imageHeight,
                       pangolin::GlTexture & imageTex,
                       const int overviewWidth, const int overviewHeight,
                       pangolin::GlTexture & overviewTex,
                       ScatterPlotShader & pointShader,
                       float * selection);

    ~SingleEmbeddingViz();

    void setEmbedding(const float2 * embedding, uchar3 * coloring, int nEmbedded);

    void render(const float2 windowSize);

    inline float2 getMaxViewportSize() { return subViz_.getMaxViewportSize(); }

    inline float2 getMaxViewportCenter() { return subViz_.getMaxViewportCenter(); }

    inline int getNumEmbeddedPoints() { return subViz_.getNumEmbeddedPoints(); }

    inline const float2 getEmbeddedPoint(const int index) { return subViz_.getEmbeddedPoint(index); }

    inline int getHoveredOverPoint() { return subViz_.getHoveredOverPoint(); }

    void setHoveredOverPoint(const float2 viewportPoint);

    inline void clearHover() { subViz_.clearHover(); }

    inline void getEnclosedPoints(std::vector<int> & enclosedPoints, const std::vector<float2> & viewportLassoPoints) {
        return subViz_.getEnclosedPoints(enclosedPoints, viewportLassoPoints);
    }

private:

    EmbeddingSubViz subViz_;

    ScatterPlotShader & pointShader_;

    float * xCoords_;
    float * yCoords_;

};

#endif // SINGLE_EMBEDDING_VIZ_H
