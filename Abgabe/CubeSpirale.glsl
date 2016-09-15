#include "func_collections.h"

vec3 lightDir = vec3(0.5,0.5,-1);

in vec2 uv;

uniform float spiralRot; 
vec4 color = vec4(0.247, 0.278, 0.729, 1.0);
//vec4 color = vec4(0.878, 0.239, 0.659, 1.0);


vec3 repeat(vec3 p, vec3 c ){
    return mod(p, c) - 0.5 * c;
}

vec3 ColorScene(vec3 p){
	vec3 color = 0.1 *(sin(abs(p)) - p);
	return color;
}

float marchCubeFrame(vec3 p, float size){
	float cube = distRoundBox(p, vec3(size,size,0.01),0.001);
	float hole = distRoundBox(p, vec3(0.8*size,0.8*size,0.02),0.001);
	return max(cube, -hole);
}

float spirale(vec3 p, float size){
	float rot = spiralRot;	
	vec3 p0 = rotate(p, vec3(0,0,rot));
	float frame1 = marchCubeFrame(p0,3 * size);
	p0 = rotate(p, vec3(0,0,-rot));	
	frame1 = min(frame1, marchCubeFrame(p0,1.8 * size));
	//float frame2 = ;
	p0 = rotate(p, vec3(0,0,rot));
	frame1 = min(frame1, marchCubeFrame(p0,1.1 * size));
	p0 = rotate(p, vec3(0,0,-rot));
	frame1 = min(frame1, marchCubeFrame(p0,0.7 * size));	
	p0 = rotate(p, vec3(0,0,rot));
	frame1 = min(frame1, marchCubeFrame(p0,0.4 * size));	
	p0 = rotate(p, vec3(0,0,-rot));
	frame1 = min(frame1, marchCubeFrame(p0,0.23 * size));	
	p0 = rotate(p, vec3(0,0,rot));
	frame1 = min(frame1, marchCubeFrame(p0,0.16 * size));	
	p0 = rotate(p, vec3(0,0,-rot));
	frame1 = min(frame1, marchCubeFrame(p0,0.08 * size));	
	return frame1;
}
 
float dist(vec3 p){
	//float plane = distPlane(p + vec3(0,8,0), vec3(0,1,0));
	//color = vec4(ColorScene(p),1);
	float frame1 = spirale(repeat(p, vec3(2,2,0)), 0.5);	
	return frame1;
}

void main(){
	vec2 p = getScreenPos(90);
	Camera cam;
	cam.pos = vec3(0,0,-10);
	cam.dir = normalize(vec3(p.x, p.y, 1));
	int steps = -1;
	
	vec4 res = raymarch(cam.pos, cam.dir, steps);	
	if(res.a ==1){
		color = mix(vec4(0.608, 0.780, 0.0  , 1.0), vec4(0.247, 0.278, 0.729, 1.0), abs((uv.x-0.5) * 1.5)) ;
		//color = vec4(0.247, 0.278, 0.729, 1.0); 
		vec3 n = getNormal(res.xyz);
		color *= max(AMBIENT, dot(n, lightDir));
		//color *=shadow(res.xyz,n);
		color +=ambientOcclusion(res.xyz,n) * AMBIENT;
		color *=1;
	}
	gl_FragColor = color;

}