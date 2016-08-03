#include "func_collections.h"

in vec2 uv;

uniform float iGlobalTime;
uniform float p1Rotation;
uniform float p16Translation;
uniform float p3Rotation;
uniform float p23Translation;
uniform float p4Rotation;
uniform float p45Translation;

#define SPECULAR 20
#define RAD PI / 180.0

const vec3 lightDir = normalize(vec3(0.8, 0.8, -1.0));
const float glowEpsiolon = 0.2;

vec3 glow = vec3(0);
vec3 globalColor = vec3(1);
vec3 color = vec3(1.0,0.5,0.5);

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
mat4 lookAt(vec3 eye, vec3 center, vec3 up)
{
    vec3 zaxis = normalize(center - eye);
    vec3 xaxis = normalize(cross(up, zaxis));
    vec3 yaxis = cross(zaxis, xaxis);

    mat4 matrix;
    //Column Major
    matrix[0][0] = xaxis.x;
    matrix[1][0] = yaxis.x;
    matrix[2][0] = zaxis.x;
    matrix[3][0] = 0;

    matrix[0][1] = xaxis.y;
    matrix[1][1] = yaxis.y;
    matrix[2][1] = zaxis.y;
    matrix[3][1] = 0;

    matrix[0][2] = xaxis.z;
    matrix[1][2] = yaxis.z;
    matrix[2][2] = zaxis.z;
    matrix[3][2] = 0;

    matrix[0][3] = -dot(xaxis, eye);
    matrix[1][3] = -dot(yaxis, eye);
    matrix[2][3] = -dot(zaxis, eye);
    matrix[3][3] = 1;

    return matrix;
}
mat4 translationMatrix(vec3 delta)
{
	return mat4(	1.0,	0.0,	0.0,	delta.x,
					0.0,	1.0,	0.0,	delta.y,
					0.0,	0.0,	1.0,	delta.z,
					0.0,	0.0,	0.0,	1.0 	);
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
vec4 raymarchPyramids(vec3 rayOrigin, vec3 rayDir, out int steps)
{
	float totalDist = 0.0;
	glow = vec3(0);
	for(int j=0; j<MAXSTEPS; j++)
	{
		steps = j;
		vec3 p = rayOrigin + totalDist*rayDir;
		float d = dist(p);
		if(abs(d)<EPSILON)	//if it is near the surface, return an intersection
		{
			return vec4(p, 1.0);
		}
		if(d < glowEpsiolon)
		{
			glow +=  globalColor;
		}
		totalDist += d;
		if(totalDist>=MAXDEPTH) break;
	}
	return vec4(0);
}
vec3 getNormalPyramids(vec3 p)
{
	float h = EPSILON;
	return normalize(vec3(
		dist(p + vec3(h, 0, 0)) - dist(p - vec3(h, 0, 0)),
		dist(p + vec3(0, h, 0)) - dist(p - vec3(0, h, 0)),
		dist(p + vec3(0, 0, h)) - dist(p - vec3(0, 0, h))));
}

float dist(vec3 p)
{
	//rotation around given axis aroud standard position, then translation
	float cubeSize = 3.5;
	
	float pyramid1 = sdPyramid((vec4(p,1.0) //back pyramid
		*translationMatrix(vec3(0.0,2.0,p16Translation)) //line a
		*rotationMatrix(vec3(1.0,0.0,0.0), p1Rotation*RAD)).xyz //line b
		,0.5, 3.5, 3.5); //height is 3.5 - 0.5 (because of block under pyramid)
	float cube1 = udBox(((vec4(p,1.0)
		*translationMatrix(vec3(0.0,2.0,p16Translation))) //line a
		*rotationMatrix(vec3(1.0,0.0,0.0), p1Rotation*RAD) //line b
		*translationMatrix(vec3(0.0,-3.5,0.0))
		).xyz, vec3(cubeSize));
	float pyramid6 = sdPyramid((vec4(p,1.0) //front pyramid
		*translationMatrix(vec3(0.0,2.0,-p16Translation+7)) //line a
		*rotationMatrix(vec3(1.0,0.0,0.0), -p1Rotation*RAD)).xyz //line b
		,0.5, 3.5, 3.5);
	float cube6 = udBox(((vec4(p,1.0)
		*translationMatrix(vec3(0.0,2.0,-p16Translation+7))) //line a
		*rotationMatrix(vec3(1.0,0.0,0.0), -p1Rotation*RAD) //line b
		*translationMatrix(vec3(0.0,-3.5,0.0))
		).xyz, vec3(cubeSize));
	
	float pyramid2 = sdPyramid((vec4(p,1.0) //upper pyramid
		*translationMatrix(vec3(0.0,p23Translation,3.5)) //line a
		*rotationMatrix(vec3(1.0,0.0,0.0), (p3Rotation+180)*RAD)).xyz //line b
		,0.5, 3.5, 3.5);
	float cube2 = udBox(((vec4(p,1.0)
		*translationMatrix(vec3(0.0,p23Translation,3.5))) //line a
		*rotationMatrix(vec3(1.0,0.0,0.0), (p3Rotation+180)*RAD) //line b
		*translationMatrix(vec3(0.0,-3.5,0.0))
		).xyz, vec3(cubeSize));
	float pyramid3 = sdPyramid((vec4(p,1.0) //lower pyramid
		*translationMatrix(vec3(0.0,-p23Translation+4,3.5)) //line a
		*rotationMatrix(vec3(1.0,0.0,0.0), p3Rotation*RAD)).xyz //line b
		,0.5, 3.5, 3.5);
	float cube3 = udBox(((vec4(p,1.0)
		*translationMatrix(vec3(0.0,-p23Translation+4,3.5))) //line a
		*rotationMatrix(vec3(1.0,0.0,0.0), p3Rotation*RAD) //line b
		*translationMatrix(vec3(0.0,-3.5,0.0))
		).xyz, vec3(cubeSize));
	
	float pyramid4 = sdPyramid((vec4(p,1.0) //left pyramid
		*translationMatrix(vec3(p45Translation,2.0,3.5)) //line a
		*rotationMatrix(vec3(0.0,0.0,1.0), p4Rotation*RAD)).xyz //line b
		,0.5, 3.5, 3.5);
	float cube4 = udBox(((vec4(p,1.0)
		*translationMatrix(vec3(p45Translation,2.0,3.5))) //line a
		*rotationMatrix(vec3(0.0,0.0,1.0), p4Rotation*RAD) //line b
		*translationMatrix(vec3(0.0,-3.5,0.0))
		).xyz, vec3(cubeSize));
	float pyramid5 = sdPyramid((vec4(p,1.0) //right pyramid
		*translationMatrix(vec3(-p45Translation,2.0,3.5)) //line a
		*rotationMatrix(vec3(0.0,0.0,1.0), -p4Rotation*RAD)).xyz //line b
		,0.5, 3.5, 3.5);
	float cube5 = udBox(((vec4(p,1.0)
		*translationMatrix(vec3(-p45Translation,2.0,3.5))) //line a
		*rotationMatrix(vec3(0.0,0.0,1.0), -p4Rotation*RAD) //line b
		*translationMatrix(vec3(0.0,-3.5,0.0))
		).xyz, vec3(cubeSize));

	float pyramid1Subtracted = opI(cube1, pyramid1);
	float pyramid2Subtracted = opI(cube2, pyramid2);
	float pyramid3Subtracted = opI(cube3, pyramid3);
	float pyramid4Subtracted = opI(cube4, pyramid4);
	float pyramid6Subtracted = opI(cube6, pyramid6);
	float pyramid5Subtracted = opI(cube5, pyramid5);

	float a = min(pyramid1Subtracted, pyramid6Subtracted);
	globalColor = vec3(0,0.69,0.55);
	float b = min(a, pyramid2Subtracted);
	globalColor = b < a ? vec3(0,0.56,0.67) : globalColor;
	float c = min(b, pyramid3Subtracted);
	globalColor = c < b ? vec3(0,0.56,0.67) : globalColor;
	float d = min(c, pyramid4Subtracted);
	globalColor = d < c ?    vec3(0.61, 0.78, 0) : globalColor;
	float e = min(d, pyramid5Subtracted);
	globalColor = e < d ? vec3(0.61, 0.78, 0) : globalColor;

	return e;
}


vec3 lighting(vec3 pos, vec3 rd, vec3 n)
{
	vec3 light = vec3(max(AMBIENT, dot(n, lightDir)));	//lambert light with light Color

	//specular
	vec3 reflection = normalize(reflect(lightDir, n));
	vec3 viewDirection = normalize(pos);
	float spec = max(AMBIENT, dot(reflection, viewDirection));

	light += pow(spec, SPECULAR);

	//light *= shadow(pos, lightDir);
	light += ambientOcclusion(pos, n) * AMBIENT;
	return light;
}

void main()
{
	vec2 p = getScreenPos(60.0);

	Camera cam;
	/*cam.pos = vec3(0,0.0,-20.0);
	cam.dir = normalize(vec4( p.x, p.y, 1,1 )*(rotationMatrix(vec3(1.0,0.0,0.0), 0.0))).xyz;*/
	
	//cam.pos = vec3(-20,20.0,-25.0);
	cam.pos = vec4(40.0, 15.0, -1.0, 1.0)*rotationMatrix(vec3(0.0,1.0,0.0), iGlobalTime)
		*translationMatrix(vec3(0,-3,0));
	cam.dir = normalize(vec3( p.x, p.y, 1));
	cam.dir = (lookAt(cam.pos, vec3(0,-3,0), vec3(0.0,1.0,0.0))*vec4(cam.dir.xyz, 1.0)).xyz;
	int steps = -1;

	vec4 res = raymarchPyramids(cam.pos, cam.dir, steps);
	vec3 currentCol = vec3(0.0);

	if(res.a==1.0)
	{
		currentCol = globalColor;//+glow*0.1 ;
		vec3 n = getNormalPyramids(res.xyz);

		currentCol *= lighting(res.xyz, cam.dir, n)*2;
		gl_FragColor = vec4(currentCol, 1.0);
	}
	else
	{
		vec2 uv = gl_FragCoord.xy / iResolution.xy;
	    vec2 coord = (uv - 0.5) * (iResolution.x/iResolution.y) * 2.0;
	    float rf = sqrt(dot(coord, coord)) * 0.25;
	    float rf2_1 = rf * rf + 1.0;
	    float e = 1.0 / (rf2_1 * rf2_1);
	    
	    vec4 src = vec4(1.0,1.0,1.0,1.0);
		gl_FragColor = vec4(src.rgb * e, 1.0);
		gl_FragColor.y  = gl_FragColor.y*2;
	}
}