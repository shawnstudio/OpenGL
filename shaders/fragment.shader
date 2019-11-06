#version 430 core
out vec4 color;

in VS_OUT{
	vec4 color;
} fs_in;

void main(void){
	//color = fs_in.color;
	color = vec4(0.0,0.8,1,1);
	//color = vec4(sin(gl_FragCoord.x)*0.5 + 0.5,cos(gl_FragCoord.y)*0.5+0.5,0,1);
}