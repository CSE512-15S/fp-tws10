#include "embedding_viz.h"
#include "gl_helpers.h"

void EmbeddingViz::render(const float2 windowSize) {

    if (zoom_ < overviewZoomThreshold_) {
        overviewTex_.Upload(overviewImage_.data(),GL_RGB,GL_UNSIGNED_BYTE);
        glColor3ub(255,255,255);

        const float2 overviewLocation = make_float2(0,0);
        const float2 overviewSize = make_float2(overviewWidth_,overviewHeight_);
        renderTexture(overviewTex_,overviewLocation,overviewSize,false);

        glColor3ub(128,128,128);
        const float2 contextUpper = getWindowPoint(getViewportCenter() + 0.5*getViewportSize(),overviewSize);//overviewLocation + 0.75*overviewSize;
        const float2 contextLower = getWindowPoint(getViewportCenter() - 0.5*getViewportSize(),overviewSize); //overviewLocation + 0.25*overviewSize;

        glBegin(GL_LINE_LOOP);
        glVertex2f(contextUpper.x,contextUpper.y);
        glVertex2f(contextUpper.x,contextLower.y);
        glVertex2f(contextLower.x,contextLower.y);
        glVertex2f(contextLower.x,contextUpper.y);
        glEnd();

    }
}
