#include "gl_helpers.h"

void setUpViewport(pangolin::View & v, const float2 viewportSize, const float2 viewportCenter) {
    glScalef(v.GetBounds().w/viewportSize.x,v.GetBounds().h/viewportSize.y,1);
    glTranslatef(viewportSize.x/2 - viewportCenter.x,viewportSize.y/2 - viewportCenter.y,0);
}
