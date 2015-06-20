#include "single_embedding_viz.h"
#include "util/gl_helpers.h"

SingleEmbeddingViz::SingleEmbeddingViz(const float aspectRatio, const float * images, const int imageChannels,
                                       const int imageWidth, const int imageHeight,
                                       pangolin::GlTexture & imageTex,
                                       ScatterPlotShader & pointShader,
                                       float * selection) :
    EmbeddingViz(aspectRatio,0.1f,images,imageChannels,imageWidth,imageHeight,imageTex),
    subViz_(aspectRatio,pointShader,selection,0), pointShader_(pointShader), xCoords_(0), yCoords_(0) {

}

SingleEmbeddingViz::~SingleEmbeddingViz() {
    delete [] xCoords_;
    delete [] yCoords_;
}

void SingleEmbeddingViz::setEmbedding(const float2 * embedding,
                                      uchar3 * coloring,
                                      int nEmbedded) {

    delete [] xCoords_;
    delete [] yCoords_;
    xCoords_ = new float[nEmbedded];
    yCoords_ = new float[nEmbedded];
    for (int i=0; i<nEmbedded; ++i) {
        xCoords_[i] = embedding[i].x;
        yCoords_[i] = embedding[i].y;
    }

    subViz_.setSufficientlyLongArray((const float*)embedding);
    subViz_.setEmbedding(xCoords_,yCoords_,coloring,nEmbedded);

}

void SingleEmbeddingViz::render(const float2 windowSize) {

    const float2 zoomedSize = getViewportSize(); //zoom_*maxViewportSize_;
    const float2 scrolledCenter = getViewportCenter(); //maxViewportCenter_ + scroll_;

    const float subvizMult = 1.f/zoom_;
    const float pointSizeWindow = std::max(1.f,basePointSize_*pointSizeViewport_/subViz_.getMaxViewportSize().x*windowSize.x*sqrtf(subvizMult));

    pointShader_.bind();
    pointShader_.setScale(pointSizeWindow);
    pointShader_.unbind();

    subViz_.render(windowSize,zoomedSize,scrolledCenter);

    int hoveredPointIndex = subViz_.getHoveredOverPoint();
    if (hoveredPointIndex >= 0 && hoveredPointIndex < subViz_.getNumEmbeddedPoints()) {

        imageTex_.Upload(images_ + hoveredPointIndex*imageChannels_*imageWidth_*imageHeight_,GL_LUMINANCE,GL_FLOAT);
        const float2 hoveredViewportPoint = subViz_.getEmbeddedPoint(hoveredPointIndex);
        const float2 hoveredWindowPoint = getWindowPoint(hoveredViewportPoint,windowSize);
//        std::cout << hoveredViewportPoint.x << ", " << hoveredViewportPoint.y << " -> " <<  hoveredWindowPoint.x << ", " << hoveredWindowPoint.y << std::endl;

        // draw bigger point
        glPointSize(2*pointSizeWindow);
        glBegin(GL_POINTS);
        glColor3ub(255,255,255);
        glVertex(hoveredWindowPoint);
        glEnd();
        glPointSize(1.5*pointSizeWindow);
        glBegin(GL_POINTS);
        glColor(subViz_.getColoring(hoveredPointIndex));
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
        glLineWidth(1);

    }

    EmbeddingViz::render(windowSize);

}

void SingleEmbeddingViz::setHoveredOverPoint(const float2 viewportPoint) {

//    const float pointSizeWindow = pointSizeViewport_/subViz_.getMaxViewportSize().x*windowSize.x*sqrtf(1.f/zoom_);

    const float maxDistViewport = pointSizeHoverMultiplier_*pointSizeViewport_*sqrtf(zoom_); //maxHoverDistPixels_/windowSize.x*getViewportSize().x;

    subViz_.setHoveredOverPoint(viewportPoint,maxDistViewport);
}
