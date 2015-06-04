#ifndef SINGLE_EMBEDDING_VIZ_H
#define SINGLE_EMBEDDING_VIZ_H

#include <pangolin/pangolin.h>
#include <vector_types.h>
#include <vector_functions.h>
#include <helper_math.h>

#include "embedding_viz.h"
#include "embedding_sub_viz.h"

class SingleEmbeddingViz : public EmbeddingViz {
public:

    SingleEmbeddingViz(const float aspectRatio, const float * images,
                       const int imageWidth, const int imageHeight,
                       pangolin::GlTexture & imageTex
//                 ,
//                 pangolin::GlTexture & previewTex
                 );

    void setEmbedding(const float2 * embedding, uchar3 * coloring, int nEmbedded);

    void render(const float2 windowSize);

    inline float2 getViewportSize() { return zoom_*subViz_.getMaxViewportSize(); }

    inline float2 getViewportCenter() { return subViz_.getMaxViewportCenter() + scroll_; }

    inline int getNumEmbeddedPoints() { return subViz_.getNumEmbeddedPoints(); }

    inline const float2 * getEmbedding() { return subViz_.getEmbedding(); }

    inline int getHoveredOverPoint() { return subViz_.getHoveredOverPoint(); }

    void setHoveredOverPoint(const float2 viewportPoint);

    inline void clearHover() { subViz_.clearHover(); }

protected:

    inline float2 getMinScroll() { return -1.f*getMaxScroll(); }

    inline float2 getMaxScroll() { return 0.5f*(subViz_.getMaxViewportSize() - getViewportSize()); }

private:

    EmbeddingSubViz subViz_;

};

#endif // SINGLE_EMBEDDING_VIZ_H
