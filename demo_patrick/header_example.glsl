#include "func_collections.h"

uniform float iGlobalTime;

#define SPECULAR 20
#define RAD PI / 180.0

const vec3 lightDir = normalize(vec3(0.8, 0.8, -1.0));

vec3 color = vec3(1);

/**
 * here go the calculation of the distance functions --> this method has to be called 'float dist(vec3 p)'
 * @param  p point
 * @return   distance to nearest object
 */

mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
            	oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}
mat4 translationMatrix(vec3 delta)
{
	return mat4(	1.0,	0.0,	0.0,	delta.x,
					0.0,	1.0,	0.0,	delta.y,
					0.0,	0.0,	1.0,	delta.z,
					0.0,	0.0,	0.0,	1.0 	);
}


 float sdPrismZ( vec3 p, float angleRads, float height, float depth ) 
{
    // sin 60 degrees = 0.866025
    // sin 45 degrees = 0.707107
    // sin 30 degrees = 0.5
    // sin 15 degrees = 0.258819
    vec3 q = abs( p );
    return max( q.z - depth, 
           max( q.x * angleRads + p.y * 0.5, -p.y ) - height * 0.5 );
}
//subtraction
float opS( float d1, float d2 )
{
    return max(-d1,d2);
}
float opI( float d1, float d2 )
{
    return max(d1,d2);
}

float sdPrismX( vec3 p, float angleRads, float height, float depth ) 
{
    vec3 q = abs( p );
    return max( q.x - depth, 
           max( q.z * angleRads + p.y * 0.5, -p.y ) - height * 0.5 );                                                         
}

float sdPyramid( vec3 p, float angleRads, float height, float depth )
{
    // Limited to range of 15 to 60 degrees ( for aesthetic reasons ).
    //0.65 ~ 45 degrees
    angleRads = clamp( angleRads, 0.5, 0.566025 );
    vec3 q = abs( p );
	
	// Intersection of prisms along two perpendicular axes.
	return max( sdPrismX( p, angleRads, height, depth ), 
	            sdPrismZ( p, angleRads, height, depth ) );

}
float udBox( vec3 p, vec3 b )
{
  return length(max(abs(p)-b,0.0));
}

float dist(vec3 p)
{
	//rotation around given axis aroud standard position, then translation
	float cubeSize = 3.5;
	float pyramid1 = sdPyramid((vec4(p,1.0)
		*translationMatrix(vec3(0.0,0.0,0.0))
		*rotationMatrix(vec3(1.0,0.0,0.0), iGlobalTime)).xyz
		,0.5, 3.5, 3.5);
	float cube = udBox((vec4(p,1.0)
		*translationMatrix(vec3(0.0,-3.5,0.0))
		*rotationMatrix(vec3(1.0,0.0,0.0), iGlobalTime)).xyz, 
		vec3(cubeSize));
	//float pyramid1Subtracted = opI(cube, pyramid1);
	/*float pyramid2 = sdPyramid((vec4(p,1.0)
		*translationMatrix(vec3(0.0,-7.0,0.0))
		*rotationMatrix(vec3(1.0,0.0,0.0), 180*RAD)).xyz
		,0.5, 3.5, 3.5);
	float pyramid3 = sdPyramid((vec4(p,1.0)
		*translationMatrix(vec3(3.5,-3.5,0.0))
		*rotationMatrix(vec3(0.0,0.0,1.0), 90*RAD)).xyz
		,0.5, 3.5, 3.5);
	float pyramid4 = sdPyramid((vec4(p,1.0)
		*translationMatrix(vec3(-3.5,-3.5,0.0))
		*rotationMatrix(vec3(0.0,0.0,1.0), -90*RAD)).xyz
		,0.5, 3.5, 3.5);*/
	//float sphere = distSphere(p - vec3(sin(iGlobalTime*0.8),-0.5,5.3), 0.5);

	//float dist = min(pyramid1Subtracted, cube);
	/*dist = min(pyramid2, dist);
	dist = min(pyramid3, dist);
	dist = min(pyramid4, dist);*/

	//color = (dist==pyramid1Subtracted) ? vec3(0.4, 0.5, 0.8) : vec3(0.8,0.5,0.4);
	// return dist;
	return min(pyramid1, cube);
	//return pyramid1Subtracted;

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
	cam.pos = vec3(0,4.0,-20.0);
	cam.dir = normalize(vec4( p.x, p.y, 1,1 )*(rotationMatrix(vec3(1.0,0.0,0.0), 0.0))).xyz;

	int steps = -1;

	vec4 res = raymarch(cam.pos, cam.dir, steps);
	vec3 currentCol = vec3(1);

	if(res.a==1.0)
	{
		currentCol = color;
		vec3 n = getNormal(res.xyz);

		currentCol *= lighting(res.xyz, cam.dir, n);
	}

	gl_FragColor = vec4(currentCol, 1.0);
}