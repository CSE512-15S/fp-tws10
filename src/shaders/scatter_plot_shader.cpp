#include "scatter_plot_shader.h"

#include <fstream>

ScatterPlotShader::ScatterPlotShader(const int nPoints, const int nDims, const float * pointData) {

    // -=-=-=- compile program -=-=-=-
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

    // -=-=-=- set up uniforms -=-=-=-
    shaderProgram_.Bind();
    setScale(1.f);

    // -=-=-=- set up attributes -=-=-=-

    shaderProgram_.Unbind();

    // -=-=-=- set up texture buffer object -=-=-=-
    // TODO:
//    const int nPoints = 10000;
//    const int nDims = 10;
//    const float * pointData = 0;

//    glGenBuffers(1, &tbo_);
//    glBindBuffer(GL_TEXTURE_BUFFER, tbo_);
//    glBufferData(GL_TEXTURE_BUFFER, nPoints*nDims*sizeof(float), pointData,GL_STATIC_DRAW);

//    glGenTextures(1, &tboTex_);

//    glBindBuffer(GL_TEXTURE_BUFFER,0);

}

ScatterPlotShader::~ScatterPlotShader() {

//    glDeleteBuffers(1, &tbo_);
//    glDeleteTextures(1, &tboTex_);

}
