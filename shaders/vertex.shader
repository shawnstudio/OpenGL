#version 450 core
layout (location =0) in vec4 position;
layout (location =1) in vec4 color;
layout (location =2) in vec4 instance_offset;
layout (location =3) in vec4 instance_color;

struct vertex{
	vec4 position;
	vec4 color;
};

layout(std140) uniform TransformBlock{
	vec4 modelpos;
	vec4 modelscale;
	vec4 modelrote;
	mat4 viewmatrix;
} transform;

layout(std430,binding =1) buffer my_vertices{
	int counter;
	vertex vertices[];
} mys;

out VS_OUT{
	vec4 color;
	vec4 position;
} vs_out;

void main(void){
	//gl_Position = mys.vertices[gl_VertexID].position;
	gl_Position = position;// + instance_offset;
	//gl_Position.z -= gl_InstanceID;
	gl_Position*= transform.modelscale;
	
	float rZ = transform.modelrote.z;
	float sinZ = sin(rZ);
	float cosZ = cos(rZ);
	mat4 mRoteZ = mat4(cosZ,-sinZ,0,0,
			sinZ,cosZ,0,0,
			0,0,1,0,
			0,0,0,1);
	mat4 mRoteX = mat4(1,0,0,0,
			0,cosZ,-sinZ,0,
			0,sinZ,cosZ,0,
			0,0,0,1
			);
	mat4 mRote = mRoteX * mRoteZ;
	gl_Position = mRote * gl_Position;
	gl_Position += transform.modelpos;
	gl_Position = transform.viewmatrix * gl_Position;
	//vs_out.color = mys.vertices[gl_VertexID].color;
	vs_out.position = gl_Position;
	//mys.counter+=1;
	//memoryBarrier();
	//atomicAdd(mys.counter,1);
	vs_out.color = color;// * instance_color;
}