#include "multi_embedding_viz.h"

#include "gl_helpers.h"
#include <vector_functions.h>

MultiEmbeddingViz::MultiEmbeddingViz(const float aspectRatio, const float * images,
                                     const int imageWidth, const int imageHeight,
                                     pangolin::GlTexture & imageTex,
                                     const int overviewWidth, const int overviewHeight,
                                     pangolin::GlTexture & overviewTex,
                                     pangolin::GlSlProgram & pointShader,
                                     float * selection) :
    EmbeddingViz(aspectRatio,0.005f,images,imageWidth,imageHeight,imageTex,overviewWidth,overviewHeight,overviewTex),
    hoveredSubvizIndex_(0), pointShader_(pointShader), selection_(selection) {

}

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
            EmbeddingSubViz * viz = new EmbeddingSubViz(aspectRatio_,pointShader_,selection_);
            viz->setEmbedding(partialEmbedding,coloring,nEmbedded);
            partialEmbeddings_.push_back(partialEmbedding);
            embeddingVizs_.push_back(viz);
        }
    }

}

void MultiEmbeddingViz::render(const float2 windowSize) {

//    std::cout << view.GetBounds().w << " x " << view.GetBounds().h << std::endl;

    glPushMatrix();
//    setUpViewport(windowSize,make_float2(dims_),make_float2(0.5*dims_));
    setUpViewport(windowSize,getViewportSize(),getViewportCenter());

//    const float vizHeight = view.GetBounds().h / (float)dims_;
//    const float vizWidth = view.GetBounds().w / (float)dims_;

//    glScalef(1.f/zoom_,1.f/zoom_,1.f/zoom_);
//    glTranslatef(-scroll_.x,-scroll_.y+subvizPaddingPercent_,0);
    glTranslatef(0,subvizPaddingPercent_,0);
    for (int yDim = 0; yDim < dims_; ++yDim) {
        glPushMatrix();
        glTranslatef(subvizPaddingPercent_,0,0);
        for (int xDim = 0; xDim < dims_; ++xDim) {
            EmbeddingSubViz * viz = embeddingVizs_[xDim + yDim*dims_];
//            viz->render(make_float2(vizWidth,vizHeight));
            viz->render(make_float2(1.f-2*subvizPaddingPercent_),viz->getMaxViewportSize(),viz->getMaxViewportCenter());
            //glTranslatef(vizWidth,0,0);
            glTranslatef(1,0,0);
        }
        glPopMatrix();
//        glTranslatef(0,vizHeight,0);
        glTranslatef(0,1,0);
    }
    glPopMatrix();

    EmbeddingSubViz * hoverViz = embeddingVizs_[hoveredSubvizIndex_];
    const int hoveredPointIndex = hoverViz->getHoveredOverPoint();
    if (hoveredPointIndex >= 0 && hoveredPointIndex < hoverViz->getNumEmbeddedPoints()) {

        const int hoveredSubvizCol = hoveredSubvizIndex_ % dims_;
        const int hoveredSubvizRow = hoveredSubvizIndex_ / dims_;

        imageTex_.Upload(images_ + hoveredPointIndex*imageWidth_*imageHeight_,GL_LUMINANCE,GL_FLOAT);
        const float2 hoveredViewportPoint = hoverViz->getEmbedding()[hoveredPointIndex];
        const float2 hoveredWindowPoint = getWindowPoint(getViewportPointOfSubvizPoint(hoverViz->getNormalizedPoint(hoveredViewportPoint),hoveredSubvizRow,hoveredSubvizCol),windowSize);

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

    EmbeddingViz::render(windowSize);
}

void MultiEmbeddingViz::setHoveredOverPoint(const float2 viewportPoint, const float2 windowSize) {

    const int xAxis = viewportPoint.x;
    const int yAxis = viewportPoint.y;

    const int thisSubvizNum = xAxis + dims_*yAxis;
    embeddingVizs_[hoveredSubvizIndex_]->clearHover();

//    std::cout << xAxis << ", " << yAxis << std::endl;

    EmbeddingSubViz * subviz = embeddingVizs_[thisSubvizNum];
    const float2 subviewportPoint = ((viewportPoint - make_float2(xAxis,yAxis) - make_float2(subvizPaddingPercent_))/(1-2*subvizPaddingPercent_) - make_float2(0.5))*
                                    subviz->getMaxViewportSize() + subviz->getMaxViewportCenter();

    const float maxDistViewport = maxHoverDistPixels_/windowSize.x*getViewportSize().x*(1-2*subvizPaddingPercent_/dims_)*subviz->getMaxViewportSize().x; //maxHoverDistPixels_/((windowSize.x/dims_)*(1-2*subvizPaddingPercent_))*subviz->getMaxViewportSize().x;
    subviz->setHoveredOverPoint(subviewportPoint,maxDistViewport);

    hoveredSubvizIndex_ = thisSubvizNum;
}

void MultiEmbeddingViz::clear() {
    for (EmbeddingSubViz * viz : embeddingVizs_) {
        delete viz;
    }
    for (float2 * partialEmbedding : partialEmbeddings_) {
        delete partialEmbedding;
    }
    embeddingVizs_.clear();
    partialEmbeddings_.clear();
}

void MultiEmbeddingViz::clearHover() {

    return embeddingVizs_[hoveredSubvizIndex_]->clearHover();

}

int MultiEmbeddingViz::getHoveredOverPoint() {

    return embeddingVizs_[hoveredSubvizIndex_]->getHoveredOverPoint();

}

//void MultiEmbeddingViz::clampScroll() {

//    scroll_ = fmaxf(getMinScroll(),fminf(scroll_,getMaxScroll()));

//}
