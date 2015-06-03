#include "multi_embedding_viz.h"

#include <vector_functions.h>

MultiEmbeddingViz::MultiEmbeddingViz(const float aspectRatio, const float * images,
                                     const int imageWidth, const int imageHeight,
                                     pangolin::GlTexture & imageTex) :
    aspectRatio_(aspectRatio),images_(images),
    imageWidth_(imageWidth),
    imageHeight_(imageHeight),
    imageTex_(imageTex) { }

MultiEmbeddingViz::~MultiEmbeddingViz() {
    clear();
}

void MultiEmbeddingViz::setEmbedding(const float * embedding, const int embeddingDimensions,
                                     uchar3 * coloring, int nEmbedded) {
    clear();
    dims_ = embeddingDimensions;
    for (int yDim = 0; yDim < dims_; ++yDim) {
        for (int xDim = 0; xDim < dims_; ++xDim) {
            float2 * partialEmbedding = new float2[nEmbedded];
            for (int i=0; i<nEmbedded; ++i) {
                partialEmbedding[i] = make_float2(embedding[i*dims_ + xDim],
                                                  embedding[i*dims_ + yDim]);
            }
            EmbeddingViz * viz = new EmbeddingViz(aspectRatio_,images_,
                                                  imageWidth_,imageHeight_,
                                                  imageTex_);
            viz->setEmbedding(partialEmbedding,coloring,nEmbedded);
            partialEmbeddings_.push_back(partialEmbedding);
            embeddingVizs_.push_back(viz);
        }
    }

}

void MultiEmbeddingViz::render(pangolin::View & view) {

    const float vizHeight = view.GetBounds().h / (float)dims_;
    const float vizWidth = view.GetBounds().w / (float)dims_;

    glPushMatrix();
    for (int yDim = 0; yDim < dims_; ++yDim) {
        glPushMatrix();
        for (int xDim = 0; xDim < dims_; ++xDim) {
            EmbeddingViz * viz = embeddingVizs_[xDim + yDim*dims_];
            viz->render(make_float2(vizWidth,vizHeight));
            glTranslatef(vizWidth,0,0);
        }
        glPopMatrix();
        glTranslatef(0,vizHeight,0);
    }
    glPopMatrix();

}

void MultiEmbeddingViz::clear() {
    for (EmbeddingViz * viz : embeddingVizs_) {
        delete viz;
    }
    for (float2 * partialEmbedding : partialEmbeddings_) {
        delete partialEmbedding;
    }
    embeddingVizs_.clear();
    partialEmbeddings_.clear();
}
