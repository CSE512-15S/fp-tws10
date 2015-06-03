#ifndef GL_HELPERS_H
#define GL_HELPERS_H

#include <pangolin/pangolin.h>
#include <GL/gl.h>
#include <vector_types.h>
#include <vector_functions.h>
#include <helper_math.h>

inline void glColor(const uchar3 color) { glColor3ubv(&color.x); }

inline void glVertex(const float2 vertex) { glVertex2fv(&vertex.x); }

void setUpViewport(const float2 windowSize, const float2 viewportSize, const float2 viewportCenter);

inline void setUpViewport(pangolin::View & window, const float2 viewportSize, const float2 viewportCenter) { setUpViewport(make_float2(window.GetBounds().w,window.GetBounds().h),viewportSize,viewportCenter); }

void renderTexture(const pangolin::GlTexture & texture, const float2 location = make_float2(0,0), const float2 size = make_float2(1,1));

inline static void glScalePixels(const float3 scale = make_float3(1.f), const float3 bias = make_float3(0.f)){
    glPixelTransferf(GL_RED_SCALE, scale.x);
    glPixelTransferf(GL_GREEN_SCALE, scale.y);
    glPixelTransferf(GL_BLUE_SCALE, scale.z);
    glPixelTransferf(GL_RED_BIAS, bias.x);
    glPixelTransferf(GL_GREEN_BIAS, bias.y);
    glPixelTransferf(GL_BLUE_BIAS, bias.z);
}

#endif // GL_HELPERS_H
