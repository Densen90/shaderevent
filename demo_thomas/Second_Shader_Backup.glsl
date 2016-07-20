#include "func_collections.h"
#include "06_header_outro.h"
#include "07_header_outro.h"

uniform float iGlobalTime;

in vec2 uv;

uniform float rotatePX;
uniform float rotatePY;
uniform float rotatePZ;

//Camera Position
uniform float iCamPosX = 10.0;
uniform float iCamPosY = 25.0;
uniform float iCamPosZ = 10.0;

//Camera Rotation
uniform float iCamRotX = 90.0;
uniform float iCamRotY = 90.0;
uniform float iCamRotZ = 90.0;

const float toRadian = PI/180.0;
const float glowEpsiolon = 0.1;

#define SPECULAR 20

vec3 lightDir1 = vec3(0.0, 5.0, -5.0);
vec4 color = vec4(0.0);
vec3 glow = vec3(0);

vec4 ColorScene(vec3 p)
{
	vec3 color = 0.1 *(sin(abs(p)) - p);
	return vec4(color,1);
}
vec3 Repeat(vec3 P, vec3 b)
{
	return mod(P, b) - 0.5 * b;
}

float dist(vec3 p){
	float box2 = sdTorus88(Repeat(p + vec3(1), vec3(0.0, 1.0, 1.0)), vec2(5.0, 3.0));

	p = rotate(p, vec3(rotatePX, rotatePX, rotatePZ));
	color = ColorScene(p/2);
	float box = sdTorus88(Repeat(p, vec3(8, 5, 2)), vec2(4.0, 2.0));
	float box1 = distRoundBox(p, vec3(1, 5, 2), 2);

	float temp = fOpUnionChamfer(box, box2, 8);		
	float res = Union(temp, box1);	
	return res;
}

vec3 lighting(vec3 pos, vec3 rd, vec3 n){
	vec3 light = vec3(max(AMBIENT, dot(n, lightDir1)));	//lambert light with light Color

	//specular
	vec3 reflection = normalize(reflect(lightDir1, n));
	vec3 viewDirection = normalize(pos);
	float spec = max(AMBIENT, dot(reflection, viewDirection));

	light += pow(spec, SPECULAR);

	light *= shadow(pos, lightDir);
	light += ambientOcclusion(pos, n) * AMBIENT;
	return light;
} 

void main()
{
	vec2 p = getScreenPos(45);
	Camera cam;
	cam.pos = vec3(iCamPosX, iCamPosY, iCamPosZ);
	cam.dir = rotate(normalize(vec3(p.x, p.y, 1.0)), vec3(iCamRotX, iCamRotY, iCamRotZ));
	int steps = -1;

	vec4 res = raymarch(cam.pos, cam.dir, steps);
	vec4 currentCol = vec4(1);

	p = getScreenPos(90);
	Camera cam2;
	cam2.pos = vec3(0,0,-25);
	cam2.dir = normalize(vec3(p.x, p.y, 1.0));

	
	if(res.a ==1.0)	{
		currentCol = color;
		vec3 n = getNormal(res.xyz);

		currentCol.xyz *= lighting(res.xyz, cam.dir, n);
	}
	currentCol += getOutroColor(cam2, uv);
	currentCol += getOutroColor2(cam2, uv);	
	

	gl_FragColor = currentCol/3;
}