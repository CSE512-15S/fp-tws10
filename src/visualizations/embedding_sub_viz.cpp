#include "embedding_sub_viz.h"

#include <limits>
#include <stdlib.h>
#include <vector_functions.h>
#include <helper_math.h>
#include <iostream>
#include "gl_helpers.h"

void EmbeddingSubViz::setEmbedding(const float2 * embedding, uchar3 * coloring, int nEmbedded) {

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

    std::cout << "embedding spans " << minEmbedding.x << " -> " << maxEmbedding.x << ", " << minEmbedding.y << " -> " << maxEmbedding.y << std::endl;
    std::cout << "embedding size: " << embeddingSize.x << ", " << embeddingSize.y << std::endl;
    std::cout << "embedding center: " << maxViewportCenter_.x << ", " << maxViewportCenter_.y << std::endl;
    std::cout << "viewport size: " << maxViewportSize_.x << ", " << maxViewportSize_.y << std::endl;

}

void EmbeddingSubViz::render(const float2 window, const float2 viewportSize, const float2 viewportCenter) {

    glPushMatrix();

    setUpViewport(window,viewportSize,viewportCenter);

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
    }

    glPopMatrix();
}

void EmbeddingSubViz::setHoveredOverPoint(const float2 viewportPoint) {

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
