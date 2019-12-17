#version 450 core
out vec4 color;

in VS_OUT{
	vec4 color;
	vec3 normal;
	vec2 uv;
} vin;

void main(){
	color = vin.color;
}