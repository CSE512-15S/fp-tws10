#include "embedding_viz.h"
#include "gl_helpers.h"

EmbeddingViz::EmbeddingViz(const float aspectRatio) :
   aspectRatio_(aspectRatio) {

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


    std::cout << "embedding spans " << minEmbedding.x << " -> " << maxEmbedding.x << ", " << minEmbedding.y << " -> " << maxEmbedding.y << std::endl;
    std::cout << "embedding size: " << embeddingSize.x << ", " << embeddingSize.y << std::endl;
    std::cout << "embedding center: " << viewportCenter_.x << ", " << viewportCenter_.y << std::endl;
    std::cout << "viewport size: " << viewportSize_.x << ", " << viewportSize_.y << std::endl;

}

void EmbeddingViz::render(const float2 window) {

    glPushMatrix();
    setUpViewport(window,viewportSize_,viewportCenter_);

    glPointSize(3);
    glColor3ub(0,0,0);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2,GL_FLOAT,0,embedding_);

    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(3,GL_UNSIGNED_BYTE,0,coloring_);

    glDrawArrays(GL_POINTS, 0, nEmbedded_);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    glPopMatrix();
}
