#include "func_collections.h"

uniform float iGlobalTime;

uniform float interpolate;

//Camera Position
uniform float cameraX = 10.0;
uniform float cameraY = 25.0;
uniform float cameraZ = 10.0;

//Camera Rotation
uniform float cameraAngleX = 90.0;
uniform float cameraAngleY = 90.0;
uniform float cameraAngleZ = 90.0;

const float toRadian = PI/180.0;
const float glowEpsiolon = 0.1;

#define SPECULAR 20

vec3 lightDir = vec3(0.0, 3.0, -5.0);
vec3 color = vec3(1.0);
vec3 glow = vec3(0);


//TODO: FIX REPEAT!!!!
vec3 Repeat(vec3 P, vec3 b)
{
	return mod(P, b) - 0.5 * b;
}

vec3 ColorScene(vec3 p)
{
	vec3 color = 0.1 *(sin(abs(p)) - p);
	return color;
}

float dist(vec3 p)
{
	//p = abs(sin(p));
	float boxes = distSphere(Repeat(p, vec3(5, 10, 1)), .8); //5.0
	//float boxes1 = distRoundBox(Repeat(p, vec3(2.5, 2.5, 5)), vec3(0.5, 0.5, 0.5), .0);
	//float temp = Difference(boxes, boxes1)  * interpolate  * 0.5;

	float boxes1 = distSphere(Repeat(p, vec3(2.5, 2.5, 2.5)), 0.8);
	color = ColorScene(p);// vec3(0.1, 0.3, 0.66);
	return boxes;// abs(sin(temp)) + cos(temp * 0.5);
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
	vec2 p = getScreenPos(60);

	Camera cam;
	cam.pos = vec3(0.0, 0.0, -.5);
	cam.dir = normalize(vec3(p.x, p.y, 1.0));

	int steps = -1;

	vec4 res = raymarch(cam.pos, cam.dir, steps);
	vec3 currentCol = vec3(1);

	if(res.a == 1.0)
	{
		currentCol = color;
		vec3 n = getNormal(res.xyz);

		currentCol *= lighting(res.xyz, cam.dir, n);
		glow += currentCol * lighting(res.xyz, cam.dir, n);
		currentCol += vec3(glow * 0.9);
		currentCol = ApplyFog(currentCol, currentCol * 0.7, 0.5);
	}

	gl_FragColor = vec4(currentCol, 1.0);
}