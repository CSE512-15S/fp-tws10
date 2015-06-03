#include "multi_embedding_viz.h"

#include "gl_helpers.h"
#include <vector_functions.h>

MultiEmbeddingViz::MultiEmbeddingViz(const float aspectRatio, const float * images,
                                     const int imageWidth, const int imageHeight,
                                     pangolin::GlTexture & imageTex) :
    aspectRatio_(aspectRatio),images_(images),
    imageWidth_(imageWidth),
    imageHeight_(imageHeight),
    imageTex_(imageTex),
    zoom_(1.f),
    scroll_(make_float2(0.f,0.f)) { }

MultiEmbeddingViz::~MultiEmbeddingViz() {
    clear();
}

void MultiEmbeddingViz::setEmbedding(const float * embedding, const int embeddingDimensions,
                                     uchar3 * coloring, const int nEmbedded) {
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

//    std::cout << view.GetBounds().w << " x " << view.GetBounds().h << std::endl;

    setUpViewport(view,make_float2(dims_),make_float2(0.5*dims_));

//    const float vizHeight = view.GetBounds().h / (float)dims_;
//    const float vizWidth = view.GetBounds().w / (float)dims_;

    glPushMatrix();
    glScalef(1.f/zoom_,1.f/zoom_,1.f/zoom_);
    glTranslatef(-scroll_.x,-scroll_.y+subvizPaddingPercent_,0);
    for (int yDim = 0; yDim < dims_; ++yDim) {
        glPushMatrix();
        glTranslatef(subvizPaddingPercent_,0,0);
        for (int xDim = 0; xDim < dims_; ++xDim) {
            EmbeddingViz * viz = embeddingVizs_[xDim + yDim*dims_];
//            viz->render(make_float2(vizWidth,vizHeight));
            viz->render(make_float2(1.f-2*subvizPaddingPercent_));
            //glTranslatef(vizWidth,0,0);
            glTranslatef(1,0,0);
        }
        glPopMatrix();
//        glTranslatef(0,vizHeight,0);
        glTranslatef(0,1,0);
    }
    glPopMatrix();

}

void MultiEmbeddingViz::setHoverPoint(const float2 viewportPoint) {

    const int xAxis = viewportPoint.x;
    const int yAxis = viewportPoint.y;

    static int lastSubvizNum = xAxis + dims_*yAxis;
    const int thisSubvizNum = xAxis + dims_*yAxis;
    embeddingVizs_[lastSubvizNum]->clearHover();

    std::cout << xAxis << ", " << yAxis << std::endl;

    EmbeddingViz * subviz = embeddingVizs_[thisSubvizNum];
    const float2 subviewportPoint = ((viewportPoint - make_float2(xAxis,yAxis) - make_float2(subvizPaddingPercent_))/(1-2*subvizPaddingPercent_) - make_float2(0.5))*
                                    subviz->getViewportSize() + subviz->getViewportCenter();
    subviz->setHoveredOverPoint(subviewportPoint);

    lastSubvizNum = thisSubvizNum;
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

void MultiEmbeddingViz::clampZoom() {

    zoom_ = std::max(std::min(1.f,zoom_),0.005f);
    clampScroll();

}

void MultiEmbeddingViz::clampScroll() {

    scroll_ = fmaxf(make_float2(0.f),fminf(scroll_,make_float2(dims_) - getViewportSize()));
//    std::cout << scroll_.x << ", " << scroll_.y << std::endl;

}
