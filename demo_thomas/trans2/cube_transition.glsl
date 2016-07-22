#include "func_collections.h"
#include "01_header_intro.h"
#include "05_header_twistDice.h"


/******************************************************
	THE CALCULATIONS FOR THE SINGLE CUBE
*******************************************************/
uniform float uCameraZ;
uniform float uStartRotationTime;
uniform float camPosZ;

#define SPECULAR 20

const vec3 lightDirTrans = normalize(vec3(0.8, 0.8, -1.0));

vec4 transitionColor = vec4(1);
bool sideHitShaderOne;
bool sideHitShaderTwo;
bool sideHitShaderThree;
bool sideHitShaderFour;
bool sideHitShaderFive;
bool sideHitShaderSix;

float dist(vec3 p)
{
	vec3 cubePos = p - vec3(0.0,0.0,0.0);
	float time = iGlobalTime-uStartRotationTime;
	//time = clamp(0, iGlobalTime, time);
	cubePos = rotate(cubePos, vec3(time*20, time*20, time*20));
	float cube = distBox(cubePos, vec3(0.3));

	float intersShaderOne = distBox(cubePos + vec3(0,0,0.201), vec3(0.29, 0.29, 0.1));
	float intersShaderTwo = distBox(cubePos - vec3(0.201, 0, 0), vec3(0.1, 0.29, 0.29));
	float intersShaderThree = distBox(cubePos - vec3(0,0,0.201), vec3(0.29, 0.29, 0.1));
	float intersShaderFour = distBox(cubePos + vec3(0.201, 0, 0), vec3(0.1, 0.29, 0.29));
	float intersShaderFive = distBox(cubePos - vec3(0, 0.201, 0), vec3(0.29, 0.1, 0.29));
	float intersShaderSix = distBox(cubePos + vec3(0, 0.201, 0), vec3(0.29, 0.1, 0.29));

	float dist = min(cube, min(intersShaderOne, min(intersShaderTwo, min(intersShaderThree, min(intersShaderFour, min(intersShaderFive, intersShaderSix))))));

	sideHitShaderOne = (dist==intersShaderOne);
	sideHitShaderTwo = (dist==intersShaderTwo);
	sideHitShaderThree = (dist==intersShaderThree);
	sideHitShaderFour = (dist==intersShaderFour);
	sideHitShaderFive = (dist==intersShaderFive);
	sideHitShaderSix = (dist==intersShaderSix);

	return dist;
}

vec3 transitionLighting(vec3 pos, vec3 rd, vec3 n)
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
	vec2 p = getScreenPos(90.0);

	Camera cam;
	cam.pos = vec3(0, 0, uCameraZ);
	cam.dir = normalize(vec3( p.x, p.y, 1 ));

	int steps = -1;

	vec4 res = raymarch(cam.pos, cam.dir, steps);
	vec4 currentCol = vec4(0);
	cam.pos = vec3(0, -1, camPosZ);
	if(res.a==1.0)
	{
		currentCol = transitionColor;
		if(sideHitShaderOne) 
		{
			currentCol = getIntroColor(iResolution.xy, iGlobalTime-uStartRotationTime);
		}
		else if(sideHitShaderTwo)
		{
			currentCol = getTwistDiceColor(cam, uv);
			//currentCol = vec4(1,0,0,0);
		}
		/*
		else if(sideHitShaderThree)
		{
			currentCol = getShader3Color(iResolution.xy, iGlobalTime-uStartRotationTime);
		}
		else if(sideHitShaderFour)
		{
			currentCol = getShader4Color(iResolution.xy, iGlobalTime-uStartRotationTime);
		}
		else if(sideHitShaderFive)
		{
			currentCol = getShader5Color(iResolution.xy, iGlobalTime-uStartRotationTime);
		}
		else if(sideHitShaderSix)
		{
			currentCol = getShader6Color(iResolution.xy, iGlobalTime-uStartRotationTime);
		}
		else
		{
			vec3 n = getNormal(res.xyz);
			currentCol *= vec4(transitionLighting(res.xyz, cam.dir, n), 1.0);
		}
		*/
	}


	gl_FragColor = currentCol;
}