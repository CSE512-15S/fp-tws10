#include "multi_embedding_viz.h"

#include "util/gl_helpers.h"
#include <vector_functions.h>
#include <limits>

MultiEmbeddingViz::MultiEmbeddingViz(const float aspectRatio, const float * images,
                                     const int imageWidth, const int imageHeight,
                                     pangolin::GlTexture & imageTex,
                                     ScatterPlotShader & pointShader,
                                     float * selection) :
    EmbeddingViz(aspectRatio,0.005f,images,imageWidth,imageHeight,imageTex),
    hoveredSubvizIndex_(0), pointShader_(pointShader), selection_(selection), colorCopies_(0), selectionCopies_(0) {

}

MultiEmbeddingViz::~MultiEmbeddingViz() {
    clear();
}

void MultiEmbeddingViz::setEmbedding(const float * embedding, const int embeddingDimensions,
                                     uchar3 * coloring, const int nEmbedded) {

    assert(embeddingDimensions > 0);
    assert(nEmbedded > 0);

    // -=-=-=- compute axis bounds -=-=-=-
    dims_ = embeddingDimensions;
    width_ = height_ = 1;
    nEmbedded_ = nEmbedded;

    std::vector<float> viewportSizeByDim(dims_);
    std::vector<float> viewportCenterByDim(dims_);
    for (int d=0; d<dims_; ++d) {
        float minEmbedding = std::numeric_limits<float>::infinity();
        float maxEmbedding = -minEmbedding;
        for (int i=0; i<nEmbedded; ++i) {
            minEmbedding = std::min(minEmbedding,embedding[i*dims_ + d]);
            maxEmbedding = std::max(maxEmbedding,embedding[i*dims_ + d]);
        }
        const float embeddingSize = maxEmbedding - minEmbedding;
        const float paddedEmbeddingSize = 1.02*embeddingSize;
        viewportSizeByDim[d] = paddedEmbeddingSize;
        viewportCenterByDim[d] = minEmbedding + 0.5*paddedEmbeddingSize;
    }
    float maxViewportSize = viewportSizeByDim[0];
    for (int d=1; d<dims_; ++d) {
        maxViewportSize = std::max(maxViewportSize,viewportSizeByDim[d]);
    }

    // -=-=-=- compute parallel coordinate arrays -=-=-=-
    clear();
    parallelCoordinateArrays_.resize(dims_);
    for (int d=0; d<dims_; ++d) {
        parallelCoordinateArrays_[d] = new float[nEmbedded];
        for (int i=0; i<nEmbedded; ++i) {
            parallelCoordinateArrays_[d][i] = embedding[i*dims_ + d];
        }
    }

    // -=-=-=- set up subvizs -=-=-=-
    for (int yDim = 0; yDim < dims_; ++yDim) {
        for (int xDim = 0; xDim < dims_; ++xDim) {
            EmbeddingSubViz * viz = new EmbeddingSubViz(aspectRatio_,pointShader_,selection_,embedding);
            viz->setEmbedding(parallelCoordinateArrays_[xDim],parallelCoordinateArrays_[yDim],coloring,nEmbedded,make_float2(maxViewportSize,maxViewportSize),make_float2(viewportCenterByDim[xDim],viewportCenterByDim[yDim]));
            embeddingVizs_.push_back(viz);
        }
    }

    adjustZoomLimits();
}

void MultiEmbeddingViz::setEmbedding(const float * embedding, const int embeddingDimensions,
                                     uchar3 * coloring, const int nEmbedded,
                                     const int width, const int height,
                                     const int2 receptiveField, const int stride) {

    assert(embeddingDimensions > 0);
    assert(width > 0);
    assert(height > 0);
    assert(receptiveField.x > 0);
    assert(receptiveField.y > 0);
    assert(stride > 0);
    assert(nEmbedded > 0);

    // -=-=-=- compute axis bounds -=-=-=-
    dims_ = embeddingDimensions;
    width_ = width;
    height_ = height;
    receptiveField_ = receptiveField;
    stride_ = stride;
    nEmbedded_ = nEmbedded;

    std::vector<float> viewportSizeByDim(dims_);
    std::vector<float> viewportCenterByDim(dims_);
    for (int d=0; d<dims_; ++d) {

        float minEmbedding = std::numeric_limits<float>::infinity();
        float maxEmbedding = -minEmbedding;
        for (int n=0; n<nEmbedded; ++n) {
            for (int h=0; h<height; ++h) {
                for (int w=0; w<width; ++w) {
                    const int i = w + width*(h + height*(d + dims_*n));
                    minEmbedding = std::min(minEmbedding,embedding[i]);
                    maxEmbedding = std::max(maxEmbedding,embedding[i]);
                }
            }
        }
        const float embeddingSize = maxEmbedding - minEmbedding;
        const float paddedEmbeddingSize = 1.02*embeddingSize;
        viewportSizeByDim[d] = paddedEmbeddingSize;
        viewportCenterByDim[d] = minEmbedding + 0.5*paddedEmbeddingSize;

    }
    float maxViewportSize = viewportSizeByDim[0];
    for (int d=1; d<dims_; ++d) {
        maxViewportSize = std::max(maxViewportSize,viewportSizeByDim[d]);
    }

    // -=-=-=- compute parallel coordinate arrays -=-=-=-
    clear();
    parallelCoordinateArrays_.resize(dims_);
    for (int d=0; d<dims_; ++d) {
        parallelCoordinateArrays_[d] = new float[nEmbedded*width_*height_];
        for (int i=0; i<nEmbedded; ++i) {
            for (int h=0; h<height_; ++h) {
                for (int w=0; w<width_; ++w) {
                    parallelCoordinateArrays_[d][w + width_*(h + height_*i)] = embedding[w + width_*(h + height_*(d + dims_*i))];
                }
            }
        }
    }

    // -=-=-=- copy the color and selection array -=-=-=-
    colorCopies_ = new uchar3[width_*height_*nEmbedded];
    selectionCopies_ = new float[width_*height_*nEmbedded];
    for (int i=0; i<nEmbedded_; ++i) {
        for (int j=0; j<width_*height_; ++j) {
            colorCopies_[j + i*width_*height_] = coloring[i];
            selectionCopies_[j + i*width_*height_] = selection_[i];
        }
    }

    // -=-=-=- set up subvizs -=-=-=-
    for (int yDim = 0; yDim < dims_; ++yDim) {
        for (int xDim = 0; xDim < dims_; ++xDim) {
            EmbeddingSubViz * viz = new EmbeddingSubViz(aspectRatio_,pointShader_,selectionCopies_,embedding);
            viz->setEmbedding(parallelCoordinateArrays_[xDim],parallelCoordinateArrays_[yDim],colorCopies_,nEmbedded_*width_*height_,make_float2(maxViewportSize,maxViewportSize),make_float2(viewportCenterByDim[xDim],viewportCenterByDim[yDim]));
            embeddingVizs_.push_back(viz);
        }
    }

    adjustZoomLimits();

}

void MultiEmbeddingViz::updateSelectionCoarseToFine() {
    if (width_ == 1 && height_ == 1) { return; }
    std::cout << "updating coarse to fine" << std::endl;
    for (int i=0; i<nEmbedded_; ++i) {
        for (int j=0; j<width_*height_; ++j) {
            selectionCopies_[j + i*width_*height_] = selection_[i];
        }
    }
}

void MultiEmbeddingViz::updateSelectionFineToCoarse() {
    if (width_ == 1 && height_ == 1) { return; }
    if (selectionCopies_[0] == 0.5f) { return; }
    std::cout << "updating fine to coarse" << std::endl;
    for (int i=0; i<nEmbedded_; ++i) {
        bool selected = false;
        for (int j=0; j<width_*height_; ++j) {
            if (selectionCopies_[j + i*width_*height_] == 1.f) {
                selected = true;
                break;
            }
        }
        selection_[i] = selected ? 1.f : 0.f;
    }
}

void MultiEmbeddingViz::setSubselection(const int selectedPoint) {

    std::memset(selectionCopies_,0,nEmbedded_*width_*height_*sizeof(float));
    selectionCopies_[selectedPoint] = 1.f;

}

void MultiEmbeddingViz::setSubselection(std::vector<int> selectedPoints) {

    std::memset(selectionCopies_,0,nEmbedded_*width_*height_*sizeof(float));
    for (int i=0; i<selectedPoints.size(); ++i) {
        selectionCopies_[selectedPoints[i]] = 1.f;
    }
}

void MultiEmbeddingViz::render(const float2 windowSize) {

//    std::cout << view.GetBounds().w << " x " << view.GetBounds().h << std::endl;

    glPushMatrix();
//    setUpViewport(windowSize,make_float2(dims_),make_float2(0.5*dims_));
    setUpViewport(windowSize,getViewportSize(),getViewportCenter());

    const float2 viewportMin = getViewportCenter() - 0.5*getViewportSize();
    const float2 viewportMax = getViewportCenter() + 0.5*getViewportSize();
    const int2 visibleAxisRangeX = make_int2(floor(viewportMin.x + subvizPaddingPercent_),ceil(viewportMax.x - subvizPaddingPercent_));
    const int2 visibleAxisRangeY = make_int2(floor(viewportMin.y + subvizPaddingPercent_),ceil(viewportMax.y - subvizPaddingPercent_));

    assert(visibleAxisRangeX.x >= 0 && visibleAxisRangeX.x <= dims_);
    assert(visibleAxisRangeX.y >= 0 && visibleAxisRangeX.y <= dims_);
    assert(visibleAxisRangeY.x >= 0 && visibleAxisRangeY.x <= dims_);
    assert(visibleAxisRangeY.y >= 0 && visibleAxisRangeY.y <= dims_);

//    const float vizHeight = view.GetBounds().h / (float)dims_;
//    const float vizWidth = view.GetBounds().w / (float)dims_;

//    glScalef(1.f/zoom_,1.f/zoom_,1.f/zoom_);
//    glTranslatef(-scroll_.x,-scroll_.y+subvizPaddingPercent_,0);

//    static int maxPointsOnScreen = 0;
//    const int pointsOnScreen = nEmbedded_*(visibleAxisRangeX.y - 1 - visibleAxisRangeX.x)*(visibleAxisRangeY.y - 1 - visibleAxisRangeY.x);
//    maxPointsOnScreen = std::max(pointsOnScreen,maxPointsOnScreen);
//    std::cout << "max points on screen = " << maxPointsOnScreen << std::endl;
    const int visibleXPlots = (visibleAxisRangeX.y - visibleAxisRangeX.x);
    const int visibleYPlots = (visibleAxisRangeY.y - visibleAxisRangeY.x);
    const int visiblePlots = std::max(1,visibleXPlots*visibleYPlots);
    const int maxPointsPerPlot = std::min(100000,maxPointsOnScreen_/visiblePlots);
//    std::cout << "points per plot: " << maxPointsPerPlot << std::endl;

    const float subvizMaxZoom = (1 - 2*subvizPaddingPercent_)/dims_;
//    std::cout << "subviz max zoom: " <<  subvizMaxZoom << std::endl;
    const float subvizMult = 1/(subvizMaxZoom*zoom_);
    const float pointSizeWindow = std::max(1.f,basePointSize_*pointSizeViewport_/embeddingVizs_[0]->getMaxViewportSize().x*subvizMaxZoom*windowSize.x*sqrtf(subvizMult));

    pointShader_.bind();
    pointShader_.setScale(pointSizeWindow);
    pointShader_.unbind();

    glTranslatef(0,visibleAxisRangeY.x,0);
    glTranslatef(0,subvizPaddingPercent_,0);
    for (int yDim = visibleAxisRangeY.x; yDim < visibleAxisRangeY.y; ++yDim) {
        glPushMatrix();
        glTranslatef(visibleAxisRangeX.x,0,0);
        glTranslatef(subvizPaddingPercent_,0,0);
        for (int xDim = visibleAxisRangeX.x; xDim < visibleAxisRangeX.y; ++xDim) {
            EmbeddingSubViz * viz = embeddingVizs_[xDim + yDim*dims_];
//            viz->render(make_float2(vizWidth,vizHeight));
            viz->render(make_float2(1.f-2*subvizPaddingPercent_),viz->getMaxViewportSize(),viz->getMaxViewportCenter(),maxPointsPerPlot);
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

        imageTex_.Upload(images_ + (hoveredPointIndex/(width_*height_))*imageWidth_*imageHeight_,GL_LUMINANCE,GL_FLOAT);
        const float2 hoveredViewportPoint = hoverViz->getEmbeddedPoint(hoveredPointIndex);
        const float2 hoveredWindowPoint = getWindowPoint(getViewportPointOfSubvizPoint(hoverViz->getNormalizedPoint(hoveredViewportPoint),hoveredSubvizRow,hoveredSubvizCol),windowSize);

        // draw bigger point
        glPointSize(2*pointSizeWindow);
        glBegin(GL_POINTS);
        glColor3ub(255,255,255);
        glVertex(hoveredWindowPoint);
        glEnd();
        glPointSize(1.5*pointSizeWindow);
        glBegin(GL_POINTS);
        glColor(hoverViz->getColoring(hoveredPointIndex));
        glVertex(hoveredWindowPoint);
        glEnd();
        glPointSize(1);

        static const float2 hoverOffset = make_float2(imageWidth_/4,imageHeight_/4);
        static const float2 textureSize = make_float2(4*imageWidth_,4*imageHeight_);

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

        if (width_ > 1 && height_ > 1) {
            const int hoverPointW = hoveredPointIndex % width_;
            const int hoverPointH = (hoveredPointIndex / width_) % height_;
//            std::cout << hoverPointW << ", " << hoverPointH << std::endl;

            const float2 receptiveFieldSize = textureSize/make_float2(imageWidth_,imageHeight_)*make_float2(receptiveField_.x,receptiveField_.y);
            const float2 receptiveFieldOffset = textureSize/make_float2(imageWidth_,imageHeight_)*make_float2(stride_*hoverPointW,stride_*(height_ - 1 - hoverPointH));

            glColor3ub(0x6f,0xb9,0xa5);
            glLineWidth(2);
            glBegin(GL_LINE_LOOP);
            glVertex2f(textureLocation.x + receptiveFieldOffset.x + receptiveFieldSize.x, textureLocation.y + receptiveFieldOffset.y + receptiveFieldSize.y);
            glVertex2f(textureLocation.x + receptiveFieldOffset.x                       , textureLocation.y + receptiveFieldOffset.y + receptiveFieldSize.y);
            glVertex2f(textureLocation.x + receptiveFieldOffset.x                       , textureLocation.y + receptiveFieldOffset.y                        );
            glVertex2f(textureLocation.x + receptiveFieldOffset.x + receptiveFieldSize.x, textureLocation.y + receptiveFieldOffset.y                        );
            glEnd();
        }
        glLineWidth(1);

    }

    EmbeddingViz::render(windowSize);
}

void MultiEmbeddingViz::setHoveredOverPoint(const float2 viewportPoint) {

    const int xAxis = viewportPoint.x;
    const int yAxis = viewportPoint.y;

    const int thisSubvizNum = xAxis + dims_*yAxis;
    embeddingVizs_[hoveredSubvizIndex_]->clearHover();

//    std::cout << xAxis << ", " << yAxis << std::endl;

    EmbeddingSubViz * subviz = embeddingVizs_[thisSubvizNum];
    const float2 subviewportPoint = getSubvizPointOfViewportPoint(viewportPoint,yAxis,xAxis,subviz);
//            ((viewportPoint - make_float2(xAxis,yAxis) - make_float2(subvizPaddingPercent_))/(1-2*subvizPaddingPercent_) - make_float2(0.5))*
//                                    subviz->getMaxViewportSize() + subviz->getMaxViewportCenter();

    const float maxDistViewport = pointSizeHoverMultiplier_*pointSizeViewport_*sqrtf(zoom_)*getMaxViewportSize().x/subviz->getMaxViewportSize().x; //maxHoverDistPixels_/windowSize.x*getViewportSize().x*(1-2*subvizPaddingPercent_/dims_)*subviz->getMaxViewportSize().x; //maxHoverDistPixels_/((windowSize.x/dims_)*(1-2*subvizPaddingPercent_))*subviz->getMaxViewportSize().x;
    subviz->setHoveredOverPoint(subviewportPoint,maxDistViewport);

    hoveredSubvizIndex_ = thisSubvizNum;
}

void MultiEmbeddingViz::clear() {
    for (EmbeddingSubViz * viz : embeddingVizs_) {
        delete viz;
    }
    for (float * coordinateArray : parallelCoordinateArrays_) {
        delete coordinateArray;
    }
    embeddingVizs_.clear();
    parallelCoordinateArrays_.clear();
    delete [] colorCopies_;
    colorCopies_ = 0;
    delete [] selectionCopies_;
    selectionCopies_ = 0;
}

void MultiEmbeddingViz::clearHover() {

    return embeddingVizs_[hoveredSubvizIndex_]->clearHover();

}

int MultiEmbeddingViz::getHoveredOverPoint() {

    return embeddingVizs_[hoveredSubvizIndex_]->getHoveredOverPoint();

}

void MultiEmbeddingViz::adjustZoomLimits() {

    minZoom_ = 0.01/dims_;
    const float maxDimsInViewZoom = maxDimsInView_/(float)dims_;
    maxZoom_ = std::min(1.f,maxDimsInViewZoom);

    std::cout << "zoom limits set to " << minZoom_ << " -> " << maxZoom_ << std::endl;
    clampZoom();

}

void MultiEmbeddingViz::getEnclosedPoints(std::vector<int> & enclosedPoints, const std::vector<float2> & viewportLassoPoints) {

    assert(viewportLassoPoints.size() > 0);

    const int xAxis = viewportLassoPoints[0].x;
    const int yAxis = viewportLassoPoints[0].y;

    const int thisSubvizNum = xAxis + dims_*yAxis;
    EmbeddingSubViz * subviz = embeddingVizs_[thisSubvizNum];
    std::vector<float2> subviewportLassoPoints(viewportLassoPoints.size());
    for (int i=0; i<viewportLassoPoints.size(); ++i) {
        subviewportLassoPoints[i] = getSubvizPointOfViewportPoint(viewportLassoPoints[i],yAxis,xAxis,subviz);
    }
    subviz->getEnclosedPoints(enclosedPoints,subviewportLassoPoints);

}

void MultiEmbeddingViz::centerOnFeature(const int unitNum) {

    int featureNum = unitNum / (width_*height_);

    setZoom(2.f/dims_);
    centerViewport(make_float2(featureNum+0.5,featureNum+0.5));

}
