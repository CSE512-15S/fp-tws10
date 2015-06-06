#include "embedding_viz.h"
#include "gl_helpers.h"

void EmbeddingViz::render(const float2 windowSize) {

    if (zoom_ < overviewZoomThreshold_) {
        overviewTex_.Upload(overviewImage_.data(),GL_RGB,GL_UNSIGNED_BYTE);
        glColor3ub(255,255,255);

        const float2 overviewLocation = make_float2(0,0);
        const float2 overviewSize = overviewSizePercent_*windowSize; //make_float2(overviewWidth_,overviewHeight_);
        renderTexture(overviewTex_,overviewLocation,overviewSize,false);

        const float2 contextUpper = (getViewportCenter() + 0.5*getViewportSize() - (getMaxViewportCenter() - 0.5*getMaxViewportSize()))/getMaxViewportSize()*overviewSize;
        const float2 contextLower = (getViewportCenter() - 0.5*getViewportSize() - (getMaxViewportCenter() - 0.5*getMaxViewportSize()))/getMaxViewportSize()*overviewSize;

        float corners[8] = {
            contextUpper.x,contextUpper.y,
            contextUpper.x,contextLower.y,
            contextLower.x,contextLower.y,
            contextLower.x,contextUpper.y
        };

        glColor3ub(64,64,64);
        glLineWidth(4);
        glPointSize(4);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer( 2, GL_FLOAT, 0, corners);
        glDrawArrays(GL_LINE_LOOP, 0, 4);

        glVertexPointer( 2, GL_FLOAT, 0, corners);
        glDrawArrays(GL_POINTS, 0, 4);

        glDisableClientState(GL_VERTEX_ARRAY);

        glColor3ub(255,255,255);
        glLineWidth(2);
        glPointSize(2);
        glEnableClientState(GL_VERTEX_ARRAY);

        glVertexPointer( 2, GL_FLOAT, 0, corners);
        glDrawArrays(GL_LINE_LOOP, 0, 4);

        glVertexPointer( 2, GL_FLOAT, 0, corners);
        glDrawArrays(GL_POINTS, 0, 4);

        glDisableClientState(GL_VERTEX_ARRAY);

        glLineWidth(1);
    }
}
