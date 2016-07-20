vec3 lightDirAbstract = vec3(0.0, 5.0, -5.0);
vec3 colorAbstract = vec3(0.0);
vec3 glowAbstract = vec3(0);


#define SPECULAR 20
#define EPSILON 0.0001
#define AOSAMPLES 8.0
#define MAXSTEPS 256
#define MAXDEPTH 60.0
#define SOFTSDHADOWFAC 2.0
#define AMBIENT 0.1
#define PI 3.14159

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

struct Camera
{
    vec3 pos;
    vec3 dir;
} CamAbstract;


vec3 ColorSceneAbstract(vec3 p)
{
	vec3 color = 0.1 *(sin(abs(p)) - p);
	return color;
}
vec3 RepeatAbstract(vec3 P, vec3 b)
{
	return mod(P, b) - 0.5 * b;
}

float ambientOcclusionAbstract(vec3 p, vec3 n)
{
    float res = 0.0;
    float fac = 1.0;
    for(float i=0.0; i<AOSAMPLES; i++)
    {
        float distOut = i*0.3;  //go on normal ray AOSAMPLES times with factor 0.3
        res += fac * (distOut - distAbstract(p + n*distOut));   //look for every step, how far the nearest object is
        fac *= 0.5; //for every step taken on the normal ray, the fac decreases, so the shadow gets brighter
    }
    return 1.0 - clamp(res, 0.0, 1.0);
}

float UnionAbtract(float object1, float object2)
{
	return min(object1, object2);
}

float fOpUnionChamferAbstract(float a, float b, float r) 
{
	return min(min(a, b), (a - r + b)*sqrt(0.5));
}

float distAbstract(vec3 p)
{
	
	float box2 = sdTorus88(Repeat(p + vec3(1), vec3(0.0, 1.0, 1.0)), vec2(5.0, 3.0));

	p = rotate(p, vec3(rotatePX, rotatePX, rotatePZ));
	color = ColorScene(p/2);
	float box = sdTorus88(Repeat(p, vec3(8, 5, 2)), vec2(4.0, 2.0));
	float box1 = distRoundBox(p, vec3(1, 5, 2), 2);

	float temp = fOpUnionChamfer(box, box2, 8);
	return Union(temp, box1);
	
}

vec3 lightingAbstract(vec3 pos, vec3 rd, vec3 n)
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

/**
 * Calculate the position on screen from the given resolution and fragcoord
 * @param  fov field of view of the camera
 * @return     the poinr on screen
 */
vec2 getScreenPosAbstract(float fov)
{
	float tanFov = tan(fov / 2.0 * 3.14159 / 180.0) / iResolution.x;
	vec2 p = tanFov * (gl_FragCoord.xy * 2.0 - iResolution.xy);
	return p;
}


/**
 * rotate method
 * @param  p point to rotate
 * @param  r rotation angle of the axes
 * @return   [description]
 */
vec3 rotateAbstract( vec3 p, vec3 r )
{
	r.x *= PI/180.0;
	r.y *= PI/180.0;
	r.z *= PI/180.0;

	mat3 xRot = mat3 (	1,	0,				0,
						0,	cos(r.x),	-sin(r.x),
						0,	sin(r.x),	cos(r.x) );
	mat3 yRot = mat3 ( 	cos(r.y),		0,	sin(r.y),
						0,					1,	0,
						-sin(r.y),		0,	cos(r.y) );
	mat3 zRot = mat3 (	cos(r.z),	-sin(r.z),	0,
						sin(r.z),	cos(r.z),	0,
						0,				0,				1 );
	return xRot * yRot * zRot * p;
}

vec3 getNormalAbstract(vec3 p)
{
    float h = EPSILON;
    return normalize(vec3(
        distAbstract(p + vec3(h, 0, 0)) - distAbstract(p - vec3(h, 0, 0)),
        distAbstract(p + vec3(0, h, 0)) - distAbstract(p - vec3(0, h, 0)),
        distAbstract(p + vec3(0, 0, h)) - distAbstract(p - vec3(0, 0, h))));
}

vec4 raymarch(vec3 rayOrigin, vec3 rayDir, out int steps)
{
	float totalDist = 0.0;
	for(int j=0; j<MAXSTEPS; j++)
	{
		steps = j;
		vec3 p = rayOrigin + totalDist*rayDir;
		float d = dist(p);
		if(abs(d)<EPSILON)	//if it is near the surface, return an intersection
		{
			return vec4(p, 1.0);
		}
		totalDist += d;
		if(totalDist>=MAXDEPTH) break;
	}
	return vec4(0);
}

void getAbstractColor()
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