#version 450 core
layout (location =0) in vec4 position;
layout (location =1) in vec4 color;
layout (location =2) in vec3 normal;
layout (location =3) in vec3 tangent;
layout (location =4) in vec2 uv;

layout ( location =0) uniform mat4 mvp;

out VS_OUT{
	vec4 color;
	vec3 normal;
	vec2 uv;
} vout;

void main(){
	gl_Position = mvp * position;
	vout.color = color;
	vout.uv = uv;
}