#include "scatter_plot_shader.h"

#include <fstream>

ScatterPlotShader::ScatterPlotShader() {

    std::ifstream fragStream("../src/shaders/scatter_plot.frag"); // TODO
    std::string fragSource( (std::istreambuf_iterator<char>(fragStream) ),
                            (std::istreambuf_iterator<char>()));
    fragStream.close();
    std::ifstream vertStream("../src/shaders/scatter_plot.vert");
    std::string vertSource( (std::istreambuf_iterator<char>(vertStream) ),
                            (std::istreambuf_iterator<char>()));
    vertStream.close();
    shaderProgram_.AddShader(pangolin::GlSlVertexShader,vertSource);
    shaderProgram_.AddShader(pangolin::GlSlFragmentShader,fragSource);
    shaderProgram_.Link();

}
