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
        viewportSize_ = make_float2(paddedEmbeddingSize.y * aspectRatio_, paddedEmbeddingSize.y);
    } else {
        //embedding width is limiting dimension
        viewportSize_ = make_float2(paddedEmbeddingSize.x, paddedEmbeddingSize.x / aspectRatio_ );
    }
    viewportCenter_ = minEmbedding + 0.5*embeddingSize;

    embedding_ = embedding;
    coloring_ = coloring;
    nEmbedded_ = nEmbedded;
    zoom_ = 1.f;
    scroll_ = make_float2(0.f,0.f);

    std::cout << "embedding spans " << minEmbedding.x << " -> " << maxEmbedding.x << ", " << minEmbedding.y << " -> " << maxEmbedding.y << std::endl;
    std::cout << "embedding size: " << embeddingSize.x << ", " << embeddingSize.y << std::endl;
    std::cout << "embedding center: " << viewportCenter_.x << ", " << viewportCenter_.y << std::endl;
    std::cout << "viewport size: " << viewportSize_.x << ", " << viewportSize_.y << std::endl;

}

void EmbeddingViz::render(const float2 window) {

    glPushMatrix();
    float2 zoomedSize = zoom_*viewportSize_;
    float2 scrolledCenter = viewportCenter_ + scroll_;

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

        const float2 quad1HoverExtent = hoveredPoint + hoverOffset + textureSize;

        int hoverDir = 0;
        if (quad1HoverExtent.x > viewportCenter_.x + viewportSize_.x/2) {
            hoverDir |= 1;
        }
        if (quad1HoverExtent.y > viewportCenter_.y + viewportSize_.y/2) {
            hoverDir |= 2;
        }

        float2 textureLocation;
        switch(hoverDir) {
            case 0:
                textureLocation = hoveredPoint + hoverOffset;
                break;
            case 1:
                textureLocation = make_float2(hoveredPoint.x - hoverOffset.x - textureSize.x, hoveredPoint.y + hoverOffset.y);
                break;
            case 2:
                textureLocation = make_float2(hoveredPoint.x + hoverOffset.x, hoveredPoint.y - hoverOffset.y - textureSize.y);
                break;
            case 3:
                textureLocation = hoveredPoint - hoverOffset - textureSize;
                break;
        }

        float linePoints[4][12] = {
            { textureLocation.x,                 textureLocation.y,
              textureLocation.x + textureSize.x, textureLocation.y,
              textureLocation.x + textureSize.x, textureLocation.y + textureSize.y,
              textureLocation.x,                 textureLocation.y + textureSize.y,
              textureLocation.x,                 textureLocation.y,
              hoveredPoint.x,                    hoveredPoint.y },
            { textureLocation.x + textureSize.x, textureLocation.y,
              textureLocation.x + textureSize.x, textureLocation.y + textureSize.y,
              textureLocation.x,                 textureLocation.y + textureSize.y,
              textureLocation.x,                 textureLocation.y,
              textureLocation.x + textureSize.x, textureLocation.y,
              hoveredPoint.x,                    hoveredPoint.y },
            { textureLocation.x,                 textureLocation.y + textureSize.y,
              textureLocation.x,                 textureLocation.y,
              textureLocation.x + textureSize.x, textureLocation.y,
              textureLocation.x + textureSize.x, textureLocation.y + textureSize.y,
              textureLocation.x,                 textureLocation.y + textureSize.y,
              hoveredPoint.x,                    hoveredPoint.y },
            { textureLocation.x + textureSize.x, textureLocation.y + textureSize.y,
              textureLocation.x,                 textureLocation.y + textureSize.y,
              textureLocation.x,                 textureLocation.y,
              textureLocation.x + textureSize.x, textureLocation.y,
              textureLocation.x + textureSize.x, textureLocation.y + textureSize.y,
              hoveredPoint.x,                    hoveredPoint.y }
        };

        glLineWidth(3);
        glColor3ub(196,196,196);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer( 2, GL_FLOAT, 0, linePoints[hoverDir]);

        glDrawArrays(GL_LINE_STRIP, 0, 6);

        glDisableClientState(GL_VERTEX_ARRAY);

        glColor3ub(255,255,255);
        renderTexture(imageTex_,
                      textureLocation,
                      textureSize);
        glLineWidth(1);
    }

    glPopMatrix();
}
