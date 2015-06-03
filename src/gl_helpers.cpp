#include "gl_helpers.h"

void setUpViewport(const float2 windowSize, const float2 viewportSize, const float2 viewportCenter) {
    glScalef(windowSize.x/viewportSize.x,windowSize.y/viewportSize.y,1);
    glTranslatef(viewportSize.x/2 - viewportCenter.x,viewportSize.y/2 - viewportCenter.y,0);
}

void renderTexture(const pangolin::GlTexture & texture, const float2 location, const float2 size ) {

    GLfloat sq_vert[] = { location.x,          location.y,
                          location.x + size.x, location.y,
                          location.x + size.x, location.y + size.y,
                          location.x,          location.y + size.y };
    glVertexPointer(2, GL_FLOAT, 0, sq_vert);
    glEnableClientState(GL_VERTEX_ARRAY);

    static GLfloat sq_tex[]  = { 0,1,  1,1,  1,0,  0,0  };
    glTexCoordPointer(2, GL_FLOAT, 0, sq_tex);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glEnable(GL_TEXTURE_2D);
    texture.Bind();

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glDisable(GL_TEXTURE_2D);

    texture.Unbind();
}
