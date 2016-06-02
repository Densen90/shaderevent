uniform vec2 iResolution;
uniform float iGlobalTime;

#ifdef HIGH_QUALITY
	#define EPSILON 0.0001
	#define AOSAMPLES 8.0
#else
	#ifdef LOW_QUALITY
		#define EPSILON 1.0
		#define AOSAMPLES 3.0
	#else
		#define EPSILON 0.001
		#define AOSAMPLES 5.0
	#endif
#endif

#define MAXSTEPS 256
#define MAXDEPTH 60.0
#define SOFTSDHADOWFAC 2.0
#define AMBIENT 0.1
#define PI 3.14159

/**
 * [predefinition for distance function
 */
float dist(vec3 p);

/**
 * Camera structure which can be used for a representation of a camera in 3D space
 */
struct Camera
{
	vec3 pos;
	vec3 dir;
};

/**
 * Calculate the position on screen from the given resolution and fragcoord
 * @param  fov field of view of the camera
 * @return     the poinr on screen
 */
vec2 getScreenPos(float fov)
{
	float tanFov = tan(fov / 2.0 * 3.14159 / 180.0) / iResolution.x;
	vec2 p = tanFov * (gl_FragCoord.xy * 2.0 - iResolution.xy);
	return p;
}

/**
 * the raymarch function
 * @param  rayOrigin origin of ray
 * @param  rayDir    direction of ray
 * @param  steps     out parameter: how many steps were needed
 * @return           vec4: x,y,z = intersection point; w=0 -> no intersection, w=1 -> intersection
 */
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

/**
 * distance function for a plane
 * @param  p position of plane
 * @param  n normal of plane
 * @return   distance to plane
 */
float distPlane( vec3 p, vec3 n)
{
	return dot(p,n);
}

/**
 * distance function for sphere
 * @param  p   position of sphere
 * @param  rad radius
 * @return     distance to sphere
 */
float distSphere(vec3 p, float rad)
{
	return length(p) - rad;
}

/**
 * distance function for rounded box
 * @param  p position of box
 * @param  b bounds of box
 * @param  r radius of edges
 * @return   distance to box
 */
float distRoundBox(vec3 p, vec3 b, float r)
{
 	return length(max(abs(p)-b,0.0))-r;
}

/**
 * rotate method
 * @param  p point to rotate
 * @param  r rotation angle of the axes
 * @return   [description]
 */
vec3 rotate( vec3 p, vec3 r )
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

/**
 * get normal at a specific point
 * @param  p point
 * @return   normalized vector
 */
vec3 getNormal(vec3 p)
{
	float h = EPSILON;
	return normalize(vec3(
		dist(p + vec3(h, 0, 0)) - dist(p - vec3(h, 0, 0)),
		dist(p + vec3(0, h, 0)) - dist(p - vec3(0, h, 0)),
		dist(p + vec3(0, 0, h)) - dist(p - vec3(0, 0, h))));
}

/**
 * apply soft shadow to a point
 * @param  ro the point
 * @param  rd the direction to the light
 * @return    ambient shadow value
 */
float shadow(vec3 ro, vec3 rd)
{
	float res = 1.0;
    for( float t=0.01; t<32.0; )
    {
        float h = dist(ro + rd*t);
        if( h<EPSILON )
            return AMBIENT;
        res = min( res, SOFTSDHADOWFAC*h/t );
        t += h;
    }
    return res;
}

/**
 * apply ambient occlusion to a point
 * @param  p point
 * @param  n normal of point
 * @return   ao value
 */
float ambientOcclusion(vec3 p, vec3 n)
{
	float res = 0.0;
	float fac = 1.0;
	for(float i=0.0; i<AOSAMPLES; i++)
	{
		float distOut = i*0.3;	//go on normal ray AOSAMPLES times with factor 0.3
		res += fac * (distOut - dist(p + n*distOut));	//look for every step, how far the nearest object is
		fac *= 0.5;	//for every step taken on the normal ray, the fac decreases, so the shadow gets brighter
	}
	return 1.0 - clamp(res, 0.0, 1.0);
}

/**
 * apply some postprocessing screen effects to the rendered scene
 * @param  inCol      the current color of the fragment
 * @param  bright     amount of brightness
 * @param  saturation amount of saturation
 * @param  contrast   amount of contrast
 * @return            new calulculated color
 */
vec3 ScreenSettings(vec3 inCol, float bright, float saturation, float contrast)
{
	vec3 lumCoeff = vec3( 0.2126, 0.7152, 0.0722 );
	vec3 brightColor = inCol.rgb * bright;
	float intensFactor = dot( brightColor, lumCoeff );
	vec3 intensFactor3 = vec3( intensFactor );
	vec3 saturationColor = mix( intensFactor3, brightColor, saturation );
	vec3 contrastColor = mix( vec3(0.5), saturationColor, contrast );

	return contrastColor;
}