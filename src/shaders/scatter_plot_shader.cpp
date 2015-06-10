#include "scatter_plot_shader.h"

#include <fstream>

ScatterPlotShader::ScatterPlotShader() {

    std::string compileDirectory = COMPILE_DIR;

//    std::cout << compileDirectory << std::endl;
    std::string fragFile = compileDirectory + "/src/shaders/scatter_plot.frag";
    std::string vertFile = compileDirectory + "/src/shaders/scatter_plot.vert";

    // -=-=-=- compile program -=-=-=-
    std::ifstream fragStream(fragFile);
    std::string fragSource( (std::istreambuf_iterator<char>(fragStream) ),
                            (std::istreambuf_iterator<char>()));
    fragStream.close();
    std::ifstream vertStream(vertFile);
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

    selectionLocation_ = shaderProgram_.GetAttributeHandle("selected");
    xCoordLocation_ = shaderProgram_.GetAttributeHandle("xCoord");
    yCoordLocation_ = shaderProgram_.GetAttributeHandle("yCoord");

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
