#version 450 core
layout (location =0) in vec4 position;
layout (location =1) in vec3 velocity;
layout (location =2) in ivec4 connection;

layout (location =0) uniform vec3 gravity;
layout (location =1) uniform float damping;
layout (location =2) uniform float spring;
layout (location =3) uniform float time;
layout (location =4) uniform float spring_rest_length;

layout (binding =0) uniform samplerBuffer tx_position;
layout (std140,binding=1) buffer PositionBlock{
	vec4[] uni_positions;
};

out vec4 out_position;
out vec3 out_velocity;

void main(){
	float mass = position.w;
	vec3 vpos = position.xyz;
	vec3 force = gravity * mass - velocity * damping;
	for(int i=0;i<4;i++)
	{
		if(connection[i]<0){
			force = vec3(0,0,0);break;
		}
		vec3 v = texelFetch(tx_position,connection[i]).xyz;
		vec3 dir = v-vpos;
		float len = length(dir);
		force += normalize(dir) * (len - spring_rest_length) * spring;
	}
	
	vec3 ac = force / mass;
	vec3 finalMove = velocity * time + 0.5 * ac * time * time;
	vec3 finalVelocity = velocity + ac * time;
	//vec3 xxxv = texelFetch(tx_position,connection[1]).xyz;
	//vec3 dddd = xxxv - vpos;
	//out_position = vec4(vpos + dddd / 100.0,mass);
	//out_position = position;
	//out_position.y+=time/10;
	out_position = vec4(vpos + finalMove,mass);
	out_velocity = finalVelocity;
}