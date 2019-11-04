#version 410 core
layout (triangles) in;
layout (points,max_vertices = 1) out;

void main(void){
    vec4 pos;
    for(int i=0;i<gl_in.length();i++){
        pos += gl_in[i].gl_Position;
    }
    gl_Position = pos/gl_in.length();
    EmitVertex();
}