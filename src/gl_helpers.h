#ifndef GL_HELPERS_H
#define GL_HELPERS_H

#include <pangolin/pangolin.h>
#include <GL/gl.h>
#include <vector_types.h>

void setUpViewport(pangolin::View & v, const float2 viewportSize, const float2 viewportCenter);

inline void glColor(const uchar3 color) { glColor3ubv(&color.x); }

inline void glVertex(const float2 vertex) { glVertex2fv(&vertex.x); }

#endif // GL_HELPERS_H
