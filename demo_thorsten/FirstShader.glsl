#include "func_collections.h"


uniform vec2 iResolution;
uniform float iGlobalTime;
in vec2 uv;
#define Pi 3.1415926535897932384626433832795

const float glowEpsiolon = 0.1;

//Textures
uniform sampler2D tex;

//Position
uniform float cameraX = 10.0;
uniform float cameraY = 25.0;
uniform float cameraZ = 10.0;

//Roation
uniform float angleX = 90.0;
uniform float angleY = 90.0;
uniform float angleZ = 90.0;

const float toRadian = Pi/180.0;
const float maxDistance = 50.0;


const float deltaEpsilon = 0.1;
const float maxIteration = 64.0;
const float minEpsiolon = 0.01;
float fov = 90.0;

vec3 pointLight = vec3(0.0, 5.0, -5.0);

/************************************************************
*	Structs
*************************************************************/
struct Ray
{
	vec3 origin;
	vec3 direction;
};

struct Intersection
{
	vec3 intersectP;
	vec3 glow ;
	vec4 color;
	vec3 normal;
	bool exists;
};



/************************************************************
*	Function Definitions
*************************************************************/
Intersection RayMarching(Ray cam);
Ray Init();
vec3 CalcNormal(vec3 p);
vec3 Rotate(vec3 object, vec3 angles);
float Box(float radius, vec3 position);
float Union(float object1, float object2);
float Difference(float object1, float object2);
vec3 CalcNormal(vec3 p);
vec3 Repeat(vec3 P, vec3 b);
float CalcLighting(vec3 p);
float DistScene(vec3 p);



/***********************************************************
*	Objects
************************************************************/
float Box(vec3 p, vec3 b) {
	vec3 d = abs(p) - b;
	return min(max(d.x, max(d.y, d.z)), 0.) + length(max(d, 0.));
}


/***********************************************************
*	Operations
************************************************************/

vec3 Rotate(vec3 object, vec3 angles)
{
	float anglex = angles.x * toRadian;
	float angley = angles.y * toRadian;
	float anglez = angles.z * toRadian;

	mat3 rx = mat3(1.0, 0.0, 		 0.0,
				   0.0, cos(anglex), -sin(anglex),
				   0.0, sin(anglex), cos(anglex));

	mat3 ry = mat3(cos(angley),  0.0, sin(angley),
				   0.0,			 1.0, 0.0,
				   -sin(angley), 0.0, cos(angley));

	mat3 rz = mat3(cos(anglez), -sin(anglez), 0.0,
				   sin(anglez), cos(anglez),  0.0,
				   0.0,			  0.0, 		  1.0);

	mat3 rm = rx * ry * rz;
	return object * rm;
}

float Union(float object1, float object2)
{
	return max(object1, object2);
}

float Difference(float object1, float object2)
{
	return min(object1, object2);
}

vec3 Repeat(vec3 P, vec3 b)
{
	return mod(P, b) - 0.5 * b;
}

float CalcLighting(vec3 p)
{
	vec3 normal = CalcNormal(p);
	vec3 lightDirection = normalize(pointLight - p);
	return max(0.2, dot(lightDirection, normal));
}

/************************************************************
*	DrawScene
*************************************************************/
vec3 CalcNormal(vec3 p)
{
	return normalize(vec3(DistScene(p + vec3(minEpsiolon, 0.0, 0.0)) - DistScene((p - vec3(minEpsiolon, 0.0, 0.0))),
						  DistScene(p + vec3(0.0, minEpsiolon, 0.0)) - DistScene((p - vec3(0.0, minEpsiolon, 0.0))),
						  DistScene(p + vec3(0.0, 0.0, minEpsiolon)) - DistScene((p - vec3(0.0, 0.0, minEpsiolon)))));
}

float DistScene(vec3 p)
{	
	//Draw the objects
	
	// p = abs(sin(texture(tex, p.xz).r * iGlobalTime * 0.05));
	// Rotate(p, vec3(iGlobalTime))
	float boxes = Box(Repeat(p, vec3(5, 5, 10)), vec3(0.5, 0.5, 0.5));
	float boxes1 = Box(Repeat(p, vec3(2.5, 2.5, 5)), vec3(0.5, 0.5, 0.5));
	float temp = Difference(boxes, boxes1)  * iGlobalTime * 0.1;

	return abs(sin(temp));
}

/************************************************************
*	Raymarch
*************************************************************/

Ray Init()
{
	float fov = 44.0;
	float tanFov = tan(fov / 2.0 * 3.14159 / 180.0) / iResolution.x;
	vec2 p = tanFov * (gl_FragCoord.xy * 2.0 - iResolution.xy);

	Ray r1; 
	// r1.origin = vec3(sin(iGlobalTime), cos(iGlobalTime), 5* iGlobalTime);
	r1.direction = Rotate(vec3(p.x, p.y, 1), vec3(0.0, 0.0, 0.0));
	r1.origin = vec3(-0.0, -0.0, -50.0 + iGlobalTime);// + sin(iGlobalTime / 2) * texture(tex, p.xy).xyz / 10;
	// r1.origin = vec3(0.0, 5.0, -8.0);
	// r1.direction = vec3(p.x, p.y, 1.0);
	r1.direction = normalize(r1.direction);

	return r1;
}


Intersection RayMarching(Ray cam)
{
	float t = 0.0;
	vec3 p = cam.origin;
	float distance = 0;
	Intersection intersect;
	intersect.exists = false;
	intersect.glow = vec3(0);
	for(int i = 0; i < maxIteration; ++i)
	{
		p = p + t * cam.direction;		
		t = DistScene(p);
		distance += t;
		
		if(t < minEpsiolon)
		{
			//Hit objects 
			intersect.intersectP = p;
			intersect.exists = true;
			intersect.normal = CalcNormal(intersect.intersectP);
			intersect.color = vec4(0.7, 0.0, 0.0, 1.0) * CalcLighting(p);
			return intersect;
		}
		// Glow
		if(t < glowEpsiolon)
		{
			intersect.glow += intersect.color * CalcLighting(p) ;
		}
	}

	// intersect.color = ApplyFog(vec4(0.0,0.0,0.0,0), min((distance/maxDistance),1));
 	return intersect;
}

void main()
{
	Ray cam = Init();
	Intersection obj = RayMarching(cam);
	if(obj.exists){
		// obj.color += Reflect(obj)  + vec4(obj.glow * 0.15, 1.0) ;
		gl_FragColor = obj.color;
	}else{
		// hit nothing, use bg color (fog/glow)
		gl_FragColor = vec4(0.0, 0.4, 0.0, 1.0);//obj.color + vec4(0.2, 0.4, 0.2, 1.0) * vec4(obj.glow, 1.0);
	}
}