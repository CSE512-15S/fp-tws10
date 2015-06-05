varying vec3 N;
varying vec3 v;
void main(){
    v = vec3(gl_ModelViewMatrix * gl_Vertex);
    N = normalize(gl_NormalMatrix * gl_Normal);

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    if (gl_FogCoord > 0) {
        gl_FrontColor = gl_Color;
        if (gl_FogCoord > 0.5001f) {
            gl_PointSize = 6;
        } else {
            gl_PointSize = 3;
        }
    } else {
        gl_FrontColor = gl_Color + 0.75*(vec4(1,1,1,1) - gl_Color);
        gl_PointSize = 2;
    }
};
