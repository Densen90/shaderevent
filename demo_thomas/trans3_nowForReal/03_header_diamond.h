uniform float iGlobalTime;

#define PI 3.14159
#define AMBIENT 0.1
#define EPSILON 0.001
#define MAXSTEPS 256
#define MAXDEPTH 60.0
#define AOSAMPLES 5.0
#define SOFTSDHADOWFAC 2.0
#define SPECULAR 20

vec3 lightDir3 = normalize(vec3(0.5,0.5,-1));
float shaderTime1;

vec3 color4 = vec3(1.0);
vec3 glow = vec3(0);
vec3 rot3;

struct Camera
{
    vec3 pos;
    vec3 dir;
} cam1;

vec3 Repeat(vec3 P, vec3 b)
{
    return mod(P, b) - 0.5 * b;
}

vec3 ColorScene(vec3 p)
{
    vec3 color = 0.1 *(sin(abs(p)) - p);
    return color;
}

float fOpUnionStairs1(float a, float b, float r, float n) {
    float s = r/n;
    float u = b-r;
    return min(min(a,b), 0.5 * (u + a + abs ((mod (u - a + s, 2 * s)) - s)));
}

// We can just call Union since stairs are symmetric.
float fOpIntersectionStairs1(float a, float b, float r, float n) {
    return -fOpUnionStairs1(-a, -b, r, n);
}

float distRoundBox3(vec3 p, vec3 b, float r)
{
    return length(max(abs(p)-b,0.0))-r;
}

vec3 rotate3( vec3 p, vec3 r )
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

float dist3(vec3 p) 
{ 
    p = rotate3(p, rot3);
    float interpolate = (iGlobalTime/2 +10);	
    float boxes = distRoundBox3(Repeat(p, vec3(5, 5, 5)), vec3(0.5, 0.5, 0.5), 5.0); //5.0
    float boxes1 = distRoundBox3(Repeat(p, vec3(10, 10, 10)), vec3(0.5, 0.5, 0.5), .0);
    float temp = fOpIntersectionStairs1(boxes, boxes1, 0, .5 )  * interpolate  * 0.5;

    color4 = ColorScene(p);// vec3(0.1, 0.3, 0.66);
    return abs(sin(temp)) + cos(temp );
}

vec3 getNormal3(vec3 p)
{
    float h = EPSILON;
    return normalize(vec3(
        dist3(p + vec3(h, 0, 0)) - dist3(p - vec3(h, 0, 0)),
        dist3(p + vec3(0, h, 0)) - dist3(p - vec3(0, h, 0)),
        dist3(p + vec3(0, 0, h)) - dist3(p - vec3(0, 0, h))));
}

vec4 raymarch3(vec3 rayOrigin, vec3 rayDir, out int steps)
{
    float totalDist = 0.0;
    for(int j=0; j<MAXSTEPS; j++)
    {
        steps = j;
        vec3 p = rayOrigin + totalDist*rayDir;
        float d = dist3(p);
        if(abs(d)<EPSILON)  //if it is near the surface, return an intersection
        {
            return vec4(p, 1.0);
        }
        totalDist += d;
        if(totalDist>=MAXDEPTH) break;
    }
    return vec4(0);
}

float ambientOcclusion3(vec3 p, vec3 n)
{
    float res = 0.0;
    float fac = 1.0;
    for(float i=0.0; i<AOSAMPLES; i++)
    {
        float distOut = i*0.3;  //go on normal ray AOSAMPLES times with factor 0.3
        res += fac * (distOut - dist3(p + n*distOut));   //look for every step, how far the nearest object is
        fac *= 0.5; //for every step taken on the normal ray, the fac decreases, so the shadow gets brighter
    }
    return 1.0 - clamp(res, 0.0, 1.0);
}

float shadow3(vec3 ro, vec3 rd)
{
    float res = 1.0;
    for( float t=0.01; t<32.0; )
    {
        float h = dist3(ro + rd*t);
        if( h<EPSILON )
            return AMBIENT;
        res = min( res, SOFTSDHADOWFAC*h/t );
        t += h;
    }
    return res;
}

vec3 lighting3(vec3 pos, vec3 rd, vec3 n)
{
    vec3 light = vec3(max(AMBIENT, dot(n, lightDir3)));  //lambert light with light Color

    //specular
    vec3 reflection = normalize(reflect(lightDir3, n));
    vec3 viewDirection = normalize(pos);
    float spec = max(AMBIENT, dot(reflection, viewDirection));

    light += pow(spec, SPECULAR);

    light *= shadow3(pos, lightDir3);
    light += ambientOcclusion3(pos, n) * AMBIENT;
    return light;
}

vec3 ApplyFog3(vec3 originalColor, vec3 fogColor, float fogAmount)
{
    return mix( originalColor, fogColor, fogAmount );
}

vec4 getDiamondColor(Camera cam, vec3 cubeRotation)
{
    // marching and shading
	int steps = -1;
    rot3 = cubeRotation;
    vec4 res = raymarch3(cam.pos, cam.dir, steps);   
    vec3 currentCol = vec3(1);
    if(res.a == 1.0)
    {
        currentCol = color4;
        vec3 n = getNormal3(res.xyz);

        currentCol *= lighting3(res.xyz, cam1.dir, n);
        glow += currentCol * lighting3(res.xyz, cam1.dir, n);
        currentCol += vec3(glow * 0.9);
        currentCol = ApplyFog3(currentCol, currentCol * 0.7, 0.5);
    } 
	
	return vec4(currentCol, 1);
}

