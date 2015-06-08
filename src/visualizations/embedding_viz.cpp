#include "embedding_viz.h"
#include "util/gl_helpers.h"

void EmbeddingViz::render(const float2 windowSize) {

//    if (zoom_ < overviewZoomThreshold_ && showOverview_) {
//        overviewTex_.Upload(overviewImage_.data(),GL_RGB,GL_UNSIGNED_BYTE);
//        glColor3ub(255,255,255);

//        const float2 overviewLocation = make_float2(0,0);
//        const float2 overviewSize = overviewSizePercent_*windowSize; //make_float2(overviewWidth_,overviewHeight_);
//        renderTexture(overviewTex_,overviewLocation,overviewSize,false);

//        const float2 contextUpper = (getViewportCenter() + 0.5*getViewportSize() - (getMaxViewportCenter() - 0.5*getMaxViewportSize()))/getMaxViewportSize()*overviewSize;
//        const float2 contextLower = (getViewportCenter() - 0.5*getViewportSize() - (getMaxViewportCenter() - 0.5*getMaxViewportSize()))/getMaxViewportSize()*overviewSize;

//        if (contextUpper.x - contextLower.x < contextBoxArrowThreshold_) {
//            // draw context arrow
//            const float2 contextCenter = 0.5*(contextUpper+contextLower);
//            float arrowPoints[12] = {
//                contextCenter.x + 20, contextCenter.y + 20,
//                contextCenter.x     , contextCenter.y     ,
//                contextCenter.x     , contextCenter.y     ,
//                contextCenter.x + 8 , contextCenter.y     ,
//                contextCenter.x     , contextCenter.y     ,
//                contextCenter.x     , contextCenter.y + 8
//            };
//            glColor3ub(0,0,255);
//            glLineWidth(3);
//            glPointSize(3);
//            glEnableClientState(GL_VERTEX_ARRAY);
//            glVertexPointer(2,GL_FLOAT,0,arrowPoints);
//            glDrawArrays(GL_LINES,0,6);
//            glVertexPointer(2,GL_FLOAT,0,arrowPoints);
//            glDrawArrays(GL_POINTS,0,6);
//            glDisableClientState(GL_VERTEX_ARRAY);
//            glLineWidth(1);
//            glPointSize(1);

//        } else {

//            // draw context box
//            float contextCorners[8] = {
//                contextUpper.x,contextUpper.y,
//                contextUpper.x,contextLower.y,
//                contextLower.x,contextLower.y,
//                contextLower.x,contextUpper.y
//            };

//            glColor3ub(64,64,64);
//            glLineWidth(4);
//            glPointSize(4);
//            glEnableClientState(GL_VERTEX_ARRAY);
//            glVertexPointer( 2, GL_FLOAT, 0, contextCorners);
//            glDrawArrays(GL_LINE_LOOP, 0, 4);

//            glVertexPointer( 2, GL_FLOAT, 0, contextCorners);
//            glDrawArrays(GL_POINTS, 0, 4);

//            glDisableClientState(GL_VERTEX_ARRAY);

//            glColor3ub(255,255,255);
//            glLineWidth(2);
//            glPointSize(2);
//            glEnableClientState(GL_VERTEX_ARRAY);

//            glVertexPointer( 2, GL_FLOAT, 0, contextCorners);
//            glDrawArrays(GL_LINE_LOOP, 0, 4);

//            glVertexPointer( 2, GL_FLOAT, 0, contextCorners);
//            glDrawArrays(GL_POINTS, 0, 4);

//            glDisableClientState(GL_VERTEX_ARRAY);
//        }

//        // put a border around the overview
//        glColor3ub(128,128,128);
//        glEnableClientState(GL_VERTEX_ARRAY);
//        float overviewCorners[8] = {
//            overviewLocation.x + overviewSize.x, overviewLocation.y + overviewSize.y,
//            overviewLocation.x + overviewSize.x, overviewLocation.y                 ,
//            overviewLocation.x                 , overviewLocation.y                 ,
//            overviewLocation.x                 , overviewLocation.y + overviewSize.y
//        };
//        glVertexPointer( 2, GL_FLOAT, 0, overviewCorners);
//        glDrawArrays(GL_LINE_LOOP, 0, 4);
//        glDisableClientState(GL_VERTEX_ARRAY);

//        glLineWidth(1);
//    }

}
