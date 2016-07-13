#include "func_collections.h"

uniform float iGlobalTime;

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

vec3 lightDir = vec3(0.0, 5.0, -5.0);
vec3 color = vec3(0.0);
vec3 glow = vec3(0);

vec3 ColorScene(vec3 p)
{
	vec3 color = 0.1 *(sin(abs(p)) - p);
	return color;
}
vec3 Repeat(vec3 P, vec3 b)
{
	return mod(P, b) - 0.5 * b;
}

float dist(vec3 p)
{
	
	float box2 = sdTorus88(Repeat(p + vec3(1), vec3(0.0, 1.0, 1.0)), vec2(5.0, 3.0));

	p = rotate(p, vec3(rotatePX, rotatePY, rotatePZ));
	color = ColorScene(p/2);
	float box = sdTorus88(Repeat(p, vec3(8, 5, 2)), vec2(4.0, 2.0));
	float box1 = distRoundBox(p, vec3(1, 5, 2), 2);

	float temp = fOpUnionChamfer(box, box2, 8);
	return Union(temp, box1);
	
}

vec3 lighting(vec3 pos, vec3 rd, vec3 n)
{
	vec3 light = vec3(max(AMBIENT, dot(n, lightDir)));	//lambert light with light Color

	//specular
	vec3 reflection = normalize(reflect(lightDir, n));
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
	vec3 currentCol = vec3(1);

	if(res.a == 1.0)
	{
		currentCol = color;
		vec3 n = getNormal(res.xyz);

		currentCol *= lighting(res.xyz, cam.dir, n);


	}

	gl_FragColor = vec4(currentCol, 1.0);
}