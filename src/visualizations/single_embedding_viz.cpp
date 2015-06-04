#include "single_embedding_viz.h"
#include "gl_helpers.h"

SingleEmbeddingViz::SingleEmbeddingViz(const float aspectRatio, const float * images,
                                       const int imageWidth, const int imageHeight,
                                       pangolin::GlTexture & imageTex) :
    aspectRatio_(aspectRatio),
    images_(images),
    imageWidth_(imageWidth),
    imageHeight_(imageHeight),
    imageTex_(imageTex),
    //   previewTex_(previewTex),
    zoom_(1.f),
    scroll_(make_float2(0,0)),
    subViz_(aspectRatio) {

}

void SingleEmbeddingViz::setEmbedding(const float2 * embedding,
                                      uchar3 * coloring,
                                      int nEmbedded) {

    subViz_.setEmbedding(embedding,coloring,nEmbedded);

}

void SingleEmbeddingViz::render(const float2 windowSize) {

    const float2 zoomedSize = getViewportSize(); //zoom_*maxViewportSize_;
    const float2 scrolledCenter = getViewportCenter(); //maxViewportCenter_ + scroll_;

    subViz_.render(windowSize,zoomedSize,scrolledCenter);

    int hoveredPointIndex = subViz_.getHoveredOverPoint();
    if (hoveredPointIndex >= 0 && hoveredPointIndex < subViz_.getNumEmbeddedPoints()) {

        imageTex_.Upload(images_ + hoveredPointIndex*imageWidth_*imageHeight_,GL_LUMINANCE,GL_FLOAT);
        const float2 hoveredViewportPoint = subViz_.getEmbedding()[hoveredPointIndex];
        const float2 hoveredWindowPoint = getWindowPoint(hoveredViewportPoint,windowSize);
//        std::cout << hoveredViewportPoint.x << ", " << hoveredViewportPoint.y << " -> " <<  hoveredWindowPoint.x << ", " << hoveredWindowPoint.y << std::endl;

        static const float2 hoverOffset = make_float2(imageWidth_/4,imageHeight_/4);
        static const float2 textureSize = make_float2(2*imageWidth_,2*imageHeight_);

        const float2 quad1HoverExtent = hoveredWindowPoint + hoverOffset + textureSize;

        int hoverDir = 0;
        if (quad1HoverExtent.x > windowSize.x) {
            hoverDir |= 1;
        }
        if (quad1HoverExtent.y > windowSize.y) {
            hoverDir |= 2;
        }

        float2 textureLocation;
        switch(hoverDir) {
            case 0:
                textureLocation = hoveredWindowPoint + hoverOffset;
                break;
            case 1:
                textureLocation = make_float2(hoveredWindowPoint.x - hoverOffset.x - textureSize.x, hoveredWindowPoint.y + hoverOffset.y);
                break;
            case 2:
                textureLocation = make_float2(hoveredWindowPoint.x + hoverOffset.x, hoveredWindowPoint.y - hoverOffset.y - textureSize.y);
                break;
            case 3:
                textureLocation = hoveredWindowPoint - hoverOffset - textureSize;
                break;
        }

        static float staticLinePoints[4][10] = {
            { 0,                 0,
              0 + textureSize.x, 0,
              0 + textureSize.x, 0 + textureSize.y,
              0,                 0 + textureSize.y,
              0,                 0
            },
            { 0 + textureSize.x, 0,
              0 + textureSize.x, 0 + textureSize.y,
              0,                 0 + textureSize.y,
              0,                 0,
              0 + textureSize.x, 0
            },
            { 0,                 0 + textureSize.y,
              0,                 0,
              0 + textureSize.x, 0,
              0 + textureSize.x, 0 + textureSize.y,
              0,                 0 + textureSize.y
            },
            { 0 + textureSize.x, 0 + textureSize.y,
              0,                 0 + textureSize.y,
              0,                 0,
              0 + textureSize.x, 0,
              0 + textureSize.x, 0 + textureSize.y
            }
        };

        float linePoints[12];
        for (int i=0; i<5; ++i) {
            linePoints[2*i]     = textureLocation.x + staticLinePoints[hoverDir][2*i];
            linePoints[2*i + 1] = textureLocation.y + staticLinePoints[hoverDir][2*i + 1];
        }
        linePoints[10] = hoveredWindowPoint.x;
        linePoints[11] = hoveredWindowPoint.y;

        glLineWidth(3);
        glColor3ub(196,196,196);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer( 2, GL_FLOAT, 0, linePoints);

        glDrawArrays(GL_LINE_STRIP, 0, 6);

        glDisableClientState(GL_VERTEX_ARRAY);

        glColor3ub(255,255,255);
        renderTexture(imageTex_,
                      textureLocation,
                      textureSize);
        glLineWidth(1);

    }

}

void SingleEmbeddingViz::setHoveredOverPoint(const float2 viewportPoint) {

    subViz_.setHoveredOverPoint(viewportPoint);
}

void SingleEmbeddingViz::clampZoom() {
    zoom_ = std::max(std::min(1.f,zoom_),0.1f);
    clampScroll();
}

void SingleEmbeddingViz::clampScroll() {

    const float2 maxScroll = subViz_.getMaxViewportSize() - getViewportSize();
    scroll_ = fmaxf(-0.5f*maxScroll,fminf(scroll_,0.5f*maxScroll));
}
