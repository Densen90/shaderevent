#include "func_collections.h"
#include "01_header_gridboxes.h"
#include "02_header_CubeSpirale.h"
#include "03_header_diamond.h"

#include "05_header_twistDice.h"
#include "06_header_knobs.h"
#include "07_header_pyramids.h"


/******************************************************
	THE CALCULATIONS FOR THE SINGLE CUBE // //#include "04_header_dices.h"
*******************************************************/

in vec2 uv;

//Camera uniforms
uniform float mainCamX;
uniform float mainCamY;
uniform float mainCamZ;

//Cube rotation
uniform float rotateCubeX;
uniform float rotateCubeY;
uniform float rotateCubeZ;

uniform float uCameraZ;

#define SPECULAR 20

const vec3 lightDir0 = normalize(vec3(0.8, 0.8, -1.0));

vec4 color = vec4(1);
bool sideHitShaderOne;
bool sideHitShaderTwo;
bool sideHitShaderThree;
bool sideHitShaderFour;
bool sideHitShaderFive;
bool sideHitShaderSix;

float dist(vec3 p)
{
	vec3 cubePos = p - vec3(0,0,0.0);
	//time = clamp(0, iGlobalTime, time);
	cubePos = rotate(cubePos, vec3(rotateCubeX, rotateCubeY, rotateCubeZ));
	float cube = distBox(cubePos, vec3(0.3));

	float intersShaderOne = distBox(cubePos, vec3(0.29, 0.29, 0.3));
	float intersShaderTwo = distBox(cubePos, vec3(0.3, 0.29, 0.29));
	float intersShaderThree = distBox(cubePos - vec3(0,0,0.201), vec3(0.29, 0.29, 0.1));
	float intersShaderFour = distBox(cubePos + vec3(0.201, 0, 0), vec3(0.1, 0.29, 0.29));
	float intersShaderFive = distBox(cubePos - vec3(0, 0.201, 0), vec3(0.29, 0.1, 0.29));
	float intersShaderSix = distBox(cubePos + vec3(0, 0.201, 0), vec3(0.29, 0.1, 0.29));

	float dist = min(cube, min(intersShaderOne, min(intersShaderTwo,min(intersShaderThree, min(intersShaderFour, min(intersShaderFive, intersShaderSix))))));

	sideHitShaderOne = (dist==intersShaderOne);
	sideHitShaderTwo = (dist==intersShaderTwo);
	sideHitShaderThree = (dist==intersShaderThree);
	sideHitShaderFour = (dist==intersShaderFour);
	sideHitShaderFive = (dist==intersShaderFive);
	sideHitShaderSix = (dist==intersShaderSix);

	return dist;
}

vec3 lighting(vec3 pos, vec3 rd, vec3 n)
{
	vec3 light = vec3(max(AMBIENT, dot(n, lightDir0)));	//lambert light with light Color

	//specular
	vec3 reflection = normalize(reflect(lightDir0, n));
	vec3 viewDirection = normalize(pos);
	float spec = max(AMBIENT, dot(reflection, viewDirection));

	light += pow(spec, SPECULAR);

	light *= shadow(pos, lightDir0);
	light += ambientOcclusion(pos, n) * AMBIENT;
	return light;	
} 

void main()
{
	vec2 p = getScreenPos(60.0);

	Camera cam;
	cam.pos = vec3(mainCamX, mainCamY, mainCamZ);
	cam.dir = normalize(vec3( p.x, p.y, 1 ));

	int steps = -1;
	vec3 cubeRotation = vec3(rotateCubeX, rotateCubeY, rotateCubeZ);
	
	vec4 res = raymarch(cam.pos, cam.dir, steps);
	vec4 currentCol = vec4(0.);
	cam.pos = vec3(0, 0, -25);

	//cam.pos = vec3(0,0,-10);
	//cam.dir = normalize(vec3(p.x, p.y, 1));
	if(res.a==1.0)	{
		currentCol = color;
		if(sideHitShaderOne){			
			currentCol =  getGridCubeColor(gl_FragCoord.xy, iResolution, iGlobalTime, res.xyz);			 
				
		}
		else if(sideHitShaderTwo){
			currentCol = getTwistDiceColor(cam, uv, cubeRotation);
			//currentCol = vec4(1,0,0,0);
		}
		else if(sideHitShaderThree){
			cam.pos = vec3(0,0,0);
			currentCol = getDiamondColor(cam, cubeRotation);
			//currentCol = vec4(1,0,0,0);
		}
		else if(sideHitShaderFour){
			cam.pos = vec3(0,0,-17.5);
			currentCol = getSpiralColor(cam, uv, cubeRotation);
			//currentCol = vec4(1,0,0,0);
		}
		else if(sideHitShaderFive){
			cam.pos = vec3(0,0,-10);
			currentCol = getKnobsColor(iResolution, cubeRotation);
			//currentCol = vec4(1,0,0,0);
		}
		else if(sideHitShaderSix){
			//cam.dir = normalize(vec4( p.x, p.y, 1,1 )*(rotationMatrix(vec3(1.0,1.0,0.0), 0.6))).xyz;
			currentCol = getShaderPyramidsColor(iResolution, iGlobalTime, gl_FragCoord.xy);
		}
		else
		{
			currentCol = vec4(lighting(res.xyz, cam.dir, getNormal(res.xyz)), 0);
		}
	}
	gl_FragColor = currentCol;
}