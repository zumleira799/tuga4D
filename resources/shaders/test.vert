#version 450

vec4[] pos = vec4[](
    vec4(0,0,0,1),
    vec4(0,1,0,1),
    vec4(1,0,0,1)
);

void main() {
    gl_Position = pos[gl_VertexIndex];    
}