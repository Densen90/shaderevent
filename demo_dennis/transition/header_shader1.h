#define PI 3.14159
#define AMBIENT 0.1
#define EPSILON 0.001
#define MAXSTEPS 256
#define MAXDEPTH 60.0
#define AOSAMPLES 5.0
#define SOFTSDHADOWFAC 2.0

vec3 lightDir1 = normalize(vec3(0.5,0.5,-1));
float shaderTime1;

struct Camera
{
    vec3 pos;
    vec3 dir;
} cam1;

vec3 rotate1( vec3 p, vec3 r )
{
    r.x *= PI/180.0;
    r.y *= PI/180.0;
    r.z *= PI/180.0;

    mat3 xRot = mat3 (  1,  0,              0,
                        0,  cos(r.x),   -sin(r.x),
                        0,  sin(r.x),   cos(r.x) );
    mat3 yRot = mat3 (  cos(r.y),       0,  sin(r.y),
                        0,                  1,  0,
                        -sin(r.y),      0,  cos(r.y) );
    mat3 zRot = mat3 (  cos(r.z),   -sin(r.z),  0,
                        sin(r.z),   cos(r.z),   0,
                        0,              0,              1 );
    return xRot * yRot * zRot * p;
}

float dist1(vec3 p) 
{ 
    return 0;
}

vec3 getNormal1(vec3 p)
{
    float h = EPSILON;
    return normalize(vec3(
        dist1(p + vec3(h, 0, 0)) - dist1(p - vec3(h, 0, 0)),
        dist1(p + vec3(0, h, 0)) - dist1(p - vec3(0, h, 0)),
        dist1(p + vec3(0, 0, h)) - dist1(p - vec3(0, 0, h))));
}

vec4 raymarch1(vec3 rayOrigin, vec3 rayDir, out int steps)
{
    float totalDist = 0.0;
    for(int j=0; j<MAXSTEPS; j++)
    {
        steps = j;
        vec3 p = rayOrigin + totalDist*rayDir;
        float d = dist1(p);
        if(abs(d)<EPSILON)  //if it is near the surface, return an intersection
        {
            return vec4(p, 1.0);
        }
        totalDist += d;
        if(totalDist>=MAXDEPTH) break;
    }
    return vec4(0);
}

float ambientOcclusion1(vec3 p, vec3 n)
{
    float res = 0.0;
    float fac = 1.0;
    for(float i=0.0; i<AOSAMPLES; i++)
    {
        float distOut = i*0.3;  //go on normal ray AOSAMPLES times with factor 0.3
        res += fac * (distOut - dist1(p + n*distOut));   //look for every step, how far the nearest object is
        fac *= 0.5; //for every step taken on the normal ray, the fac decreases, so the shadow gets brighter
    }
    return 1.0 - clamp(res, 0.0, 1.0);
}

float shadow1(vec3 ro, vec3 rd)
{
    float res = 1.0;
    for( float t=0.01; t<32.0; )
    {
        float h = dist1(ro + rd*t);
        if( h<EPSILON )
            return AMBIENT;
        res = min( res, SOFTSDHADOWFAC*h/t );
        t += h;
    }
    return res;
}

vec4 getShader1Color(vec2 resolution, float time)
{
    shaderTime1 = time;
  
    float tanFov = tan(90.0 / 2.0 * 3.14159 / 180.0) / resolution.x;
    vec2 p = tanFov * (gl_FragCoord.xy * 2.0 - resolution.xy);

    cam1.pos = vec3(0 , 0 , 0);
    cam1.dir = normalize(vec3(p.x, p.y, 1.0));  

    // marching and shading
	int steps = -1;
    vec4 res = raymarch1(cam1.pos, cam1.dir, steps);    
	
	return vec4(1, 0, 0, 1);
}

