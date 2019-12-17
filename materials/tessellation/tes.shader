#version 450 core
layout(quads) in;

void main(){
	vec4 p1 = mix(gl_in[0].gl_Position,gl_in[1].gl_Position,gl_TessCoord.x);
	vec4 p2 = mix(gl_in[3].gl_Position,gl_in[2].gl_Position,gl_TessCoord.x);
	gl_Position = mix(p2,p1,gl_TessCoord.y);
	
	//gl_Position = gl_TessCoord.x * gl_in[0].gl_Position
	//+ gl_TessCoord.y * gl_in[1].gl_Position
	//+ gl_TessCoord.z * gl_in[2].gl_Position;
	
	//float r = gl_TessCoord.y + gl_TessCoord.x / gl_TessLevelOuter[0];
	//float t = gl_TessCoord.x * 2 * 3.1415926;
	//gl_Position = vec4(sin(t) *r,cos(t)*r,0.5,1);
}