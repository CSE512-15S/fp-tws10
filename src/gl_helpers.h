#ifndef GL_HELPERS_H
#define GL_HELPERS_H

#include <pangolin/pangolin.h>
#include <GL/gl.h>
#include <vector_types.h>
#include <vector_functions.h>

inline void glColor(const uchar3 color) { glColor3ubv(&color.x); }

inline void glVertex(const float2 vertex) { glVertex2fv(&vertex.x); }

void setUpViewport(const pangolin::View & v, const float2 viewportSize, const float2 viewportCenter);

void renderTexture(const pangolin::GlTexture & texture, const float2 location = make_float2(0,0), const float2 size = make_float2(1,1));

#endif // GL_HELPERS_H
