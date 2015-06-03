#include "embedding_viz.h"
#include "gl_helpers.h"

EmbeddingViz::EmbeddingViz(const float aspectRatio, const float * images,
                           const int imageWidth, const int imageHeight,
                           pangolin::GlTexture & imageTex) :
   aspectRatio_(aspectRatio),
   images_(images),
   imageWidth_(imageWidth),
   imageHeight_(imageHeight),
   imageTex_(imageTex),
   hoveredPointIndex_(-1),
   zoom_(1.f),
   scroll_(make_float2(0,0)) {

}

void EmbeddingViz::setEmbedding(const float2 * embedding,
                                uchar3 * coloring,
                                int nEmbedded) {

    float2 minEmbedding = make_float2(std::numeric_limits<float>::infinity(),std::numeric_limits<float>::infinity());
    float2 maxEmbedding = -1*minEmbedding;
    for (int i=0; i<nEmbedded; ++i) {
        minEmbedding = fminf(minEmbedding,embedding[i]);
        maxEmbedding = fmaxf(maxEmbedding,embedding[i]);
    }

    float2 embeddingSize = maxEmbedding - minEmbedding;
    float2 paddedEmbeddingSize = 1.02*embeddingSize;

    if (paddedEmbeddingSize.x / aspectRatio_ < paddedEmbeddingSize.y) {
        //embedding height is limiting dimension
        maxViewportSize_ = make_float2(paddedEmbeddingSize.y * aspectRatio_, paddedEmbeddingSize.y);
    } else {
        //embedding width is limiting dimension
        maxViewportSize_ = make_float2(paddedEmbeddingSize.x, paddedEmbeddingSize.x / aspectRatio_ );
    }
    maxViewportCenter_ = minEmbedding + 0.5*embeddingSize;

    embedding_ = embedding;
    coloring_ = coloring;
    nEmbedded_ = nEmbedded;
    zoom_ = 1.f;
    scroll_ = make_float2(0.f,0.f);

    std::cout << "embedding spans " << minEmbedding.x << " -> " << maxEmbedding.x << ", " << minEmbedding.y << " -> " << maxEmbedding.y << std::endl;
    std::cout << "embedding size: " << embeddingSize.x << ", " << embeddingSize.y << std::endl;
    std::cout << "embedding center: " << maxViewportCenter_.x << ", " << maxViewportCenter_.y << std::endl;
    std::cout << "viewport size: " << maxViewportSize_.x << ", " << maxViewportSize_.y << std::endl;

}

void EmbeddingViz::render(const float2 window) {

    glPushMatrix();
    const float2 zoomedSize = getViewportSize(); //zoom_*maxViewportSize_;
    const float2 scrolledCenter = getViewportCenter(); //maxViewportCenter_ + scroll_;

    setUpViewport(window,zoomedSize,scrolledCenter);

    glPointSize(3);
    glColor3ub(0,0,0);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2,GL_FLOAT,0,embedding_);

    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(3,GL_UNSIGNED_BYTE,0,coloring_);

    glDrawArrays(GL_POINTS, 0, nEmbedded_);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    if (hoveredPointIndex_ >= 0 && hoveredPointIndex_ < nEmbedded_) {

        // draw bigger point
        glPointSize(12);
        glBegin(GL_POINTS);
        glColor3ub(255,255,255);
        glVertex(embedding_[hoveredPointIndex_]);
        glEnd();
        glPointSize(9);
        glBegin(GL_POINTS);
        glColor(coloring_[hoveredPointIndex_]);
        glVertex(embedding_[hoveredPointIndex_]);
        glEnd();
        glPointSize(1);

        imageTex_.Upload(images_ + hoveredPointIndex_*imageWidth_*imageHeight_,GL_LUMINANCE,GL_FLOAT);
        const float2 hoveredPoint = embedding_[hoveredPointIndex_];

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
    }

    glPopMatrix();
}

void EmbeddingViz::setHoveredOverPoint(const float2 viewportPoint) {

    int closestPoint = -1;
    float closestDist = std::numeric_limits<float>::infinity();
    for (int i=0; i<getNumEmbeddedPoints(); ++i) {
        float dist = length(viewportPoint - getEmbedding()[i]);
        if (dist < closestDist) {
            closestDist = dist;
            closestPoint = i;
        }
    }

    hoveredPointIndex_ = closestDist < 0.05 ? closestPoint : -1;

}

void EmbeddingViz::clampZoom() {
    zoom_ = std::max(std::min(1.f,zoom_),0.1f);
    clampScroll();
}

void EmbeddingViz::clampScroll() {

    const float2 maxScroll = maxViewportSize_ - getViewportSize();
    scroll_ = fmaxf(-0.5f*maxScroll,fminf(scroll_,0.5f*maxScroll));
}
