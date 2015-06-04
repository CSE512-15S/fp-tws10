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

    void render(const float2 window);

    inline float2 getViewportSize() { return zoom_*subViz_.getMaxViewportSize(); }

    inline float2 getViewportCenter() { return subViz_.getMaxViewportCenter() + scroll_; }

    inline int getNumEmbeddedPoints() { return subViz_.getNumEmbeddedPoints(); }

    inline const float2 * getEmbedding() { return subViz_.getEmbedding(); }

    inline float getZoom() { return zoom_; }

    inline void setZoom(const float zoom) { zoom_ = zoom; clampZoom(); }

    inline void incrementScroll(const float2 increment) { scroll_ += increment; clampScroll(); }

    inline int getHoveredOverPoint() { return subViz_.getHoveredOverPoint(); }

    void setHoveredOverPoint(const float2 viewportPoint);

    inline void clearHover() { subViz_.clearHover(); }

private:
    // -=-=-=-=-=- methods -=-=-=-=-=-
    void clampZoom();
    void clampScroll();

    // -=-=-=-=-=- members -=-=-=-=-=-
    float aspectRatio_;
    float zoom_;
    float2 scroll_;

    EmbeddingSubViz subViz_;

    /////////////
//    float2 maxViewportSize_;
//    float2 maxViewportCenter_;

//    int nEmbedded_;

//    const float2 * embedding_;
//    uchar3 * coloring_;

//    int hoveredPointIndex_;
    ///////////////

    const float * images_;
    const int imageWidth_;
    const int imageHeight_;
//    const uchar3 * previewImg;
    pangolin::GlTexture & imageTex_;
//    pangolin::GlTexture & previewTex_;

};

#endif // SINGLE_EMBEDDING_VIZ_H
