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

void SingleEmbeddingViz::render(const float2 window) {

    const float2 zoomedSize = getViewportSize(); //zoom_*maxViewportSize_;
    const float2 scrolledCenter = getViewportCenter(); //maxViewportCenter_ + scroll_;

    subViz_.render(window,zoomedSize,scrolledCenter);

    int hoveredPointIndex = subViz_.getHoveredOverPoint();
    if (hoveredPointIndex >= 0 && hoveredPointIndex < subViz_.getNumEmbeddedPoints()) {

        glPushMatrix();
        setUpViewport(window,zoomedSize,scrolledCenter);

        imageTex_.Upload(images_ + hoveredPointIndex*imageWidth_*imageHeight_,GL_LUMINANCE,GL_FLOAT);
        const float2 hoveredPoint = subViz_.getEmbedding()[hoveredPointIndex];

        static const float2 hoverOffset = make_float2(0.075,0.075);
        static const float2 textureSize = make_float2(0.5,0.5);
        const float2 zoomedHoverOffset = zoom_*hoverOffset;
        const float2 zoomedTextureSize = zoom_*textureSize;

        const float2 quad1HoverExtent = hoveredPoint + zoomedHoverOffset + zoomedTextureSize;

        int hoverDir = 0;
        if (quad1HoverExtent.x > scrolledCenter.x + zoomedSize.x/2) {
            hoverDir |= 1;
        }
        if (quad1HoverExtent.y > scrolledCenter.y + zoomedSize.y/2) {
            hoverDir |= 2;
        }

        float2 textureLocation;
        switch(hoverDir) {
            case 0:
                textureLocation = hoveredPoint + zoomedHoverOffset;
                break;
            case 1:
                textureLocation = make_float2(hoveredPoint.x - zoomedHoverOffset.x - zoomedTextureSize.x, hoveredPoint.y + zoomedHoverOffset.y);
                break;
            case 2:
                textureLocation = make_float2(hoveredPoint.x + zoomedHoverOffset.x, hoveredPoint.y - zoomedHoverOffset.y - zoomedTextureSize.y);
                break;
            case 3:
                textureLocation = hoveredPoint - zoomedHoverOffset - zoomedTextureSize;
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
            linePoints[2*i]     = textureLocation.x + zoom_*staticLinePoints[hoverDir][2*i];
            linePoints[2*i + 1] = textureLocation.y + zoom_*staticLinePoints[hoverDir][2*i + 1];
        }
        linePoints[10] = hoveredPoint.x;
        linePoints[11] = hoveredPoint.y;

        glLineWidth(3);
        glColor3ub(196,196,196);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer( 2, GL_FLOAT, 0, linePoints);

        glDrawArrays(GL_LINE_STRIP, 0, 6);

        glDisableClientState(GL_VERTEX_ARRAY);

        glColor3ub(255,255,255);
        renderTexture(imageTex_,
                      textureLocation,
                      zoomedTextureSize);
        glLineWidth(1);

        glPopMatrix();
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
