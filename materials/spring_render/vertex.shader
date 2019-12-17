#version 450 core
layout (location =0) in vec4 position;

layout ( location =0) uniform mat4 mvp;

out VS_OUT{
	vec4 color;
} vout;

void main(){
	gl_Position = mvp * vec4(position.xyz,1);
	vout.color = vec4(1,1,1,1);
}