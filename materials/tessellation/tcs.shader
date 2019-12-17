#version 450 core
layout (vertices =4) out;

void main(){
	if(gl_InvocationID == 0){
		gl_TessLevelInner[0] = 8;
		gl_TessLevelInner[1] = 7;
		
		gl_TessLevelOuter[0] = 5;
		gl_TessLevelOuter[1] =6;
		gl_TessLevelOuter[2] =4;
		gl_TessLevelOuter[3] =5;
	}
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

}