#ifndef SCATTER_PLOT_SHADER_H
#define SCATTER_PLOT_SHADER_H

#include <pangolin/pangolin.h>
#include <pangolin/glsl.h>

class ScatterPlotShader {
public:

    ScatterPlotShader();

    inline void bind() { shaderProgram_.Bind(); }

    inline void unbind() { shaderProgram_.Unbind(); }

private:

    pangolin::GlSlProgram shaderProgram_;

};

#endif // SCATTER_PLOT_SHADER_H
