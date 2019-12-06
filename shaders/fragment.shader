#version 450 core
out vec4 color;

in VS_OUT{
	vec4 color;
	vec4 position;
} fs_in;
uniform sampler2D s;
uniform sampler2D s2;

layout(std140) uniform TransformBlock{
	vec4 modelpos;
	vec4 modelscale;
	vec4 modelrote;
	mat4 viewmatrix;
} transform;

/*layout(std430,binding =1) buffer my_vertices{
	int counter;
	vertex vertices[];
} mys;*/

void main(void){
	//int oc = counter;
	//counter = counter+1;
	color = fs_in.color * transform.modelscale;
	
	vec4 cpos = fs_in.position / fs_in.position.w / 2;
	vec2 uv = cpos.xy + vec2(0.5,0.5);
	
	ivec2 tsize = textureSize(s,0);
	color = texelFetch(s2,ivec2(uv.x*tsize.x,uv.y*tsize.y),0);// + color * 0.1;
	//color = vec4(tsize.x/255.0,tsize.y/255.0,0,1);
	//color = vec4(0.0,0.8,1,1);
	//color = vec4(sin(gl_FragCoord.x)*0.5 + 0.5,cos(gl_FragCoord.y)*0.5+0.5,0,1);
	color = fs_in.color;
}