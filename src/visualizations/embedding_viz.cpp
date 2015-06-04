#include "embedding_viz.h"
#include "gl_helpers.h"

void EmbeddingViz::render(const float2 windowSize) {

    std::cout << "EmbeddingViz::render" << std::endl;

    if (zoom_ < overviewZoomThreshold_) {
        overviewTex_.Upload(overviewImage_.data(),GL_RGB,GL_UNSIGNED_BYTE);
        glColor3ub(255,255,255);
        renderTexture(overviewTex_,make_float2(0,0),make_float2(overviewWidth_,overviewHeight_),false);
    }

}
