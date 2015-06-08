varying vec3 N;
varying vec3 v;

uniform float scale;
//uniform samplerBuffer tboTex;

attribute float selected;
attribute float xCoord;
attribute float yCoord;

void main(){
    v = vec3(gl_ModelViewMatrix * gl_Vertex);
    N = normalize(gl_NormalMatrix * gl_Normal);

//    gl_Vertex.x = texelFetch(tboTex,2*gl_VertexID);
//    gl_Vertex.y = texelFetch(tboTex,2*gl_VertexID + 1);

//    gl_Vertex.x = xCoord;
//    gl_Vertex.y = yCoord;

    gl_Position = gl_ModelViewProjectionMatrix * vec4(xCoord,yCoord,gl_Vertex.z,gl_Vertex.w);
    if (selected > 0.f) {
        gl_FrontColor = gl_Color;
        gl_Position.z = -0.5;
        if (selected > 0.5001f) {
            gl_PointSize = scale*2.f;
        } else {
            gl_PointSize = scale;
        }
    } else {
        //gl_FrontColor = gl_Color + 0.75*(vec4(1,1,1,1) - gl_Color);
        gl_FrontColor = 0.334*gl_Color + 0.666*vec4(1.f,1.f,1.f,1.f);
        gl_Position.z = 0.5;
        gl_PointSize = scale*0.666f;
    }
}
