#ifndef SCATTER_PLOT_SHADER_H
#define SCATTER_PLOT_SHADER_H

#include <pangolin/pangolin.h>
#include <pangolin/gl/glsl.h>

class ScatterPlotShader {
public:

    ScatterPlotShader();
    ~ScatterPlotShader();

    inline void bind() {
        shaderProgram_.Bind();

//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_BUFFER, tboTex_);
//        glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, tbo_);


    }

    inline void unbind() {
        shaderProgram_.Unbind();
    }

    inline void setScale(float scale) { shaderProgram_.SetUniform("scale",scale); }

    inline GLint getSelectionLocation() { return selectionLocation_; }

    inline GLint getXCoordLocation() { return xCoordLocation_; }

    inline GLint getYCoordLocation() { return yCoordLocation_; }

private:

    pangolin::GlSlProgram shaderProgram_;
    GLint selectionLocation_;
    GLint xCoordLocation_;
    GLint yCoordLocation_;

//    GLuint tbo_;
//    GLuint tboTex_;

};

#endif // SCATTER_PLOT_SHADER_H
