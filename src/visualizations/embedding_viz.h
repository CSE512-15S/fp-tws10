#ifndef EMBEDDING_VIZ_H
#define EMBEDDING_VIZ_H

#include <pangolin/pangolin.h>
#include <vector_types.h>

class EmbeddingViz {
public:

    EmbeddingViz(const float aspectRatio, const float * images,
                 const int imageWidth, const int imageHeight,
                 pangolin::GlTexture & imageTex);

    void setEmbedding(const float2 * embedding, uchar3 * coloring, int nEmbedded);

    void render(const float2 window);

    inline float2 getViewportSize() { return viewportSize_; }

    inline float2 getViewportCenter() { return viewportCenter_; }

    inline int getNumEmbeddedPoints() { return nEmbedded_; }

    inline const float2 * getEmbedding() { return embedding_; }

    inline float getZoom() { return zoom_; }

    inline void setZoom(const float zoom) { zoom_ = zoom; }

    inline void setHoveredOverPoint(const int hoveredPointIndex) { hoveredPointIndex_ = hoveredPointIndex; }

private:
    float aspectRatio_;
    float2 viewportSize_;
    float2 viewportCenter_;
    float zoom_;
    float2 scroll_;

    int nEmbedded_;
    const float2 * embedding_;
    uchar3 * coloring_;

    int hoveredPointIndex_;
    const float * images_;
    const int imageWidth_;
    const int imageHeight_;
    pangolin::GlTexture & imageTex_;


};

#endif // EMBEDDING_VIZ_H
