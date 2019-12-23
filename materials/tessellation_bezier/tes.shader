#version 450 core
layout(quads) in;

vec4 quadratic_bezier(vec4 a,vec4 b,vec4 c,float t){
	vec4 d = mix(a,b,t);
	vec4 e = mix(b,c,t);
	return mix(d,e,t);
}

vec4 cubic_bezier(vec4 a,vec4 b,vec4 c,vec4 d,float t){
	vec4 p1 = mix(a,b,t);
	vec4 p2 = mix(b,c,t);
	vec4 p3 = mix(c,d,t);
	return quadratic_bezier(p1,p2,p3,t);
}

vec4 evaluate_patch(vec2 at){
	vec4 npoints[4];
	for(int i=0;i<4;i++){
		npoints[i] = cubic_bezier(gl_in[i+0].gl_Position,gl_in[i+4].gl_Position,
		gl_in[i+8].gl_Position,gl_in[i+13].gl_Position,at.y);
	}
	return cubic_bezier(npoints[0],npoints[1],npoints[2],npoints[3],at.x);
}

void main(){
	gl_Position = evaluate_patch(gl_TessCoord.xy);
}