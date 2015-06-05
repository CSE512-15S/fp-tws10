varying vec3 N;
varying vec3 v;

uniform float scale;
attribute float selected;

void main(){
    v = vec3(gl_ModelViewMatrix * gl_Vertex);
    N = normalize(gl_NormalMatrix * gl_Normal);

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
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
        gl_FrontColor = 0.1*gl_Color + 0.9*vec4(0.9,0.9,0.9,0.9);
        gl_Position.z = 0.5;
        gl_PointSize = scale*0.666f;
    }
}
