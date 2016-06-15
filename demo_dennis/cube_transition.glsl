#include "func_collections.h"
#include "gridboxes_include.h"
#include "testshader_include.h"

/******************************************************
	THE CALCULATIONS FOR THE SINGLE CUBE
*******************************************************/
uniform float iGlobalTime;
uniform float uCameraZ;
uniform float uStartRotationTime;

#define SPECULAR 20

const vec3 lightDir = normalize(vec3(0.8, 0.8, -1.0));

vec3 color = vec3(1);
bool sideHitShaderOne;
bool sideHitShaderTwo;

float dist(vec3 p)
{
	vec3 cubePos = p - vec3(0,0,0.0);
	float time = iGlobalTime-uStartRotationTime;
	//time = clamp(0, iGlobalTime, time);
	cubePos = rotate(cubePos, vec3(time*20, time*20, time*20));
	float cube = distBox(cubePos, vec3(0.3));

	float intersShaderOne = distBox(cubePos, vec3(0.29, 0.29, 0.3));
	float intersShaderTwo = distBox(cubePos, vec3(0.3, 0.29, 0.29));

	float dist = min(cube, intersShaderOne);

	sideHitShaderOne = (dist==intersShaderOne);
	sideHitShaderTwo = (dist==intersShaderTwo);

	return dist;
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
	vec2 p = getScreenPos(60.0);

	Camera cam;
	cam.pos = vec3(0, 0, uCameraZ);
	cam.dir = normalize(vec3( p.x, p.y, 1 ));

	int steps = -1;

	vec4 res = raymarch(cam.pos, cam.dir, steps);
	vec3 currentCol = vec3(0);

	if(res.a==1.0)
	{
		currentCol = color;
		if(sideHitShaderOne) 
		{
			currentCol = getGridCubeColor(gl_FragCoord, iResolution, iGlobalTime, cam.pos*3);
		}
		else if(sideHitShaderTwo)
		{
			currentCol = getShader2Color(gl_FragCoord, iResolution, iGlobalTime, cam.pos);
		}
		else
		{
			vec3 n = getNormal(res.xyz);
			currentCol *= lighting(res.xyz, cam.dir, n);
		}
	}

	gl_FragColor = vec4(currentCol, 1.0);
}