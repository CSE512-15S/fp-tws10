#include "embedding_sub_viz.h"

#include <limits>
#include <iostream>
#include <fstream>
#include <string>
#include "util/gl_helpers.h"

void EmbeddingSubViz::setEmbedding(const float * xCoords, const float * yCoords, const uchar3 * coloring, const int nEmbedded) {

    float2 minEmbedding = make_float2(std::numeric_limits<float>::infinity(),std::numeric_limits<float>::infinity());
    float2 maxEmbedding = -1*minEmbedding;
    for (int i=0; i<nEmbedded; ++i) {
        minEmbedding = fminf(minEmbedding,make_float2(xCoords[i],yCoords[i]));
        maxEmbedding = fmaxf(maxEmbedding,make_float2(xCoords[i],yCoords[i]));
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

    xCoords_ = xCoords;
    yCoords_ = yCoords;
    coloring_ = coloring;
    nEmbedded_ = nEmbedded;

}

void EmbeddingSubViz::setEmbedding(const float * xCoords, const float * yCoords, const uchar3 * coloring, const int nEmbedded, const float2 maxViewportSize, const float2 maxViewportCenter) {

    maxViewportSize_ = maxViewportSize;
    maxViewportCenter_ = maxViewportCenter;

    xCoords_ = xCoords;
    yCoords_ = yCoords;
    coloring_ = coloring;
    nEmbedded_ = nEmbedded;

}


void EmbeddingSubViz::render(const float2 windowSize, const float2 viewportSize, const float2 viewportCenter) {

    glPushMatrix();

    setUpViewport(windowSize,viewportSize,viewportCenter);

    glColor3ub(212,212,212);
    float axes[8] = {
        0.f                                        , maxViewportCenter_.y - maxViewportSize_.y*0.4f,
        0.f                                        , maxViewportCenter_.y + maxViewportSize_.y*0.4f,
        maxViewportCenter_.x - maxViewportSize_.x*0.4f, 0.f,
        maxViewportCenter_.x + maxViewportSize_.x*0.4f, 0.f
    };
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2,GL_FLOAT,0,axes);
    glDrawArrays(GL_LINES, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);

    pointShader_.bind();

    glPointSize(3);
    glColor3ub(0,0,0);
    glEnable(GL_DEPTH_TEST);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2,GL_FLOAT,0,xCoords_); // TODO

    glEnableVertexAttribArray(pointShader_.getXCoordLocation());
    glVertexAttribPointer(pointShader_.getXCoordLocation(),1,GL_FLOAT,false,0,xCoords_);

    glEnableVertexAttribArray(pointShader_.getYCoordLocation());
    glVertexAttribPointer(pointShader_.getYCoordLocation(),1,GL_FLOAT,false,0,yCoords_);


    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(3,GL_UNSIGNED_BYTE,0,coloring_);

//    glEnableClientState(GL_FOG_COORDINATE_ARRAY);
    //glFogCoordPointer(GL_FLOAT,0,selection_);
//    glVertexAttrib1fv(pointShader_.getSelectionLocation(),selection_);
    glEnableVertexAttribArray(pointShader_.getSelectionLocation());
    glVertexAttribPointer(pointShader_.getSelectionLocation(),1,GL_FLOAT,false,0,selection_);

    int maxArrayElements;
    glGetIntegerv(GL_MAX_ELEMENTS_VERTICES,&maxArrayElements);
    std::cout << "max array elements " << maxArrayElements << std::endl;

    glDrawArrays(GL_POINTS, 0, std::min(nEmbedded_,maxArrayElements));


    glDisableVertexAttribArray(pointShader_.getYCoordLocation());
    glDisableVertexAttribArray(pointShader_.getXCoordLocation());

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableVertexAttribArray(pointShader_.getSelectionLocation());
//    glDisableClientState(GL_FOG_COORDINATE_ARRAY);
    glDisable(GL_DEPTH_TEST);

    pointShader_.unbind();

    if (hoveredPointIndex_ >= 0 && hoveredPointIndex_ < nEmbedded_) {

        // draw bigger point
        glPointSize(12);
        glBegin(GL_POINTS);
        glColor3ub(255,255,255);
        glVertex2f(xCoords_[hoveredPointIndex_],yCoords_[hoveredPointIndex_]);
        glEnd();
        glPointSize(9);
        glBegin(GL_POINTS);
        glColor(coloring_[hoveredPointIndex_]);
        glVertex2f(xCoords_[hoveredPointIndex_],yCoords_[hoveredPointIndex_]);
        glEnd();
        glPointSize(1);
    }

    glPopMatrix();
}

void EmbeddingSubViz::setHoveredOverPoint(const float2 viewportPoint, const float maxDist) {

    int closestPoint = -1;
    float closestDist = std::numeric_limits<float>::infinity();
    for (int i=0; i<getNumEmbeddedPoints(); ++i) {
        float dist = length(viewportPoint - make_float2(xCoords_[i],yCoords_[i]));
        if (dist < closestDist) {
            closestDist = dist;
            closestPoint = i;
        }
    }

    hoveredPointIndex_ = closestDist < maxDist ? closestPoint : -1;

}
