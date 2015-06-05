#ifndef SCATTER_PLOT_SHADER_H
#define SCATTER_PLOT_SHADER_H

#include <pangolin/pangolin.h>
#include <pangolin/glsl.h>

class ScatterPlotShader {
public:

    ScatterPlotShader();

    inline void bind() { shaderProgram_.Bind(); }

    inline void unbind() { shaderProgram_.Unbind(); }

    inline void setScale(float scale) { shaderProgram_.SetUniform("scale",scale); }

    inline GLint getSelectionLocation() {
        return shaderProgram_.GetAttributeHandle("selected");
    }

private:

    pangolin::GlSlProgram shaderProgram_;
    GLint scaleLocation_;

};

#endif // SCATTER_PLOT_SHADER_H
