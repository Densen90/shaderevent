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

vec3 lightDir = vec3(0.0, 5.0, -5.0);
vec3 color = vec3(0.0);
vec3 glow = vec3(0);

vec3 ColorScene(vec3 p)
{
	vec3 color = 0.1 *(sin(abs(p)) - p);
	return color;
}

float dist(vec3 p)
{
	//p = abs(sin(p));
	float box = sdBox(p, vec3(5.0, 5.0, 5.0)); //5.0
	float box1 = sdBox(p, vec3(3.5, 3.5, 3.5));
	color = ColorScene(p);
	return max(-box1, box1);
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
	cam.pos = vec3(0.0, 0.0, -50.0);
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