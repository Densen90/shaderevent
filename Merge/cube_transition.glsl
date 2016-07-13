#include "func_collections.h"
#include "01_header_intro.h"
#include "02_header_CubeSpirale.h"


/******************************************************
	THE CALCULATIONS FOR THE SINGLE CUBE
*******************************************************/
uniform float uCameraZ;
uniform float uStartRotationTime;

#define SPECULAR 20

const vec3 lightDirTrans = normalize(vec3(0.8, 0.8, -1.0));

vec4 color = vec4(1);
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
	vec3 light = vec3(max(AMBIENT, dot(n, lightDirTrans)));	//lambert light with light Color

	//specular
	vec3 reflection = normalize(reflect(lightDirTrans, n));
	vec3 viewDirection = normalize(pos);
	float spec = max(AMBIENT, dot(reflection, viewDirection));

	light += pow(spec, SPECULAR);

	light *= shadow(pos, lightDirTrans);
	light += ambientOcclusion(pos, n) * AMBIENT;
	return light;
}

void main()
{
	vec2 p = getScreenPos(60.0);

	cam.pos = vec3(0, 0, uCameraZ);
	cam.dir = normalize(vec3( p.x, p.y, 1 ));

	int steps = -1;

	vec4 res = raymarch(cam.pos, cam.dir, steps);
	vec4 currentCol = vec4(0);

	if(res.a==1.0)
	{
		currentCol = color;
		if(sideHitShaderOne) 
		{
			currentCol = getIntroColor(iResolution.xy, iGlobalTime-uStartRotationTime);
		}
		else if(sideHitShaderTwo)
		{
			currentCol = getSpiralColor(iResolution.xy, iGlobalTime);
		}
		else
		{
			vec3 n = getNormal(res.xyz);
			currentCol *= vec4(lighting(res.xyz, cam.dir, n),1);
		}
	}

	gl_FragColor = currentCol;
}