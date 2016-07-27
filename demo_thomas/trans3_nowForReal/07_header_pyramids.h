uniform float p1Rotation;
uniform float p1XRotation;
uniform float p1ZTranslation;
uniform float p2Rotation;
uniform float p2XRotation;
uniform float p2YTranslation;
uniform float p3Rotation;
uniform float p3XRotation;
uniform float p3YTranslation;
uniform float p4Rotation;
uniform float p4ZRotation;
uniform float p4XTranslation;
uniform float p5Rotation;
uniform float p5ZRotation;
uniform float p5XTranslation;
uniform float p6Rotation;
uniform float p6XRotation;
uniform float p6ZTranslation;

uniform float p16YZRotation;
uniform float p23XYRotation;
uniform float p45XYRotation;

#define SPECULAR 20
#define PI 3.14159
#define AMBIENT 0.1
#define EPSILON 0.001
#define MAXSTEPS 256
#define MAXDEPTH 60.0
#define AOSAMPLES 5.0
#define SOFTSDHADOWFAC 2.0
#define RAD PI / 180.0

vec3 lightDir2 = normalize(vec3(0.8,0.8,-1));
vec3 color2 = vec3(1.0,0.5,0.5);
float shaderTime2;

struct Camera
{
    vec3 pos;
    vec3 dir;
} cam2;

vec3 rotate2( vec3 p, vec3 r )
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

/**************/
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
    return mat4(    1.0,    0.0,    0.0,    delta.x,
                    0.0,    1.0,    0.0,    delta.y,
                    0.0,    0.0,    1.0,    delta.z,
                    0.0,    0.0,    0.0,    1.0     );
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

float dist2(vec3 p)
{
    //rotation around given axis aroud standard position, then translation
    float cubeSize = 3.5;
    
    float pyramid1 = sdPyramid((vec4(p,1.0) //front pyramid
        *translationMatrix(vec3(0.0,2.0,p1ZTranslation)) //line a
        *rotationMatrix(vec3(p1XRotation,0.0,p16YZRotation), p1Rotation*RAD)).xyz //line b
        ,0.5, 3.5, 3.5); //height is 3.5 - 0.5 (because of block under pyramid)
    float cube1 = udBox(((vec4(p,1.0)
        *translationMatrix(vec3(0.0,2.0,p1ZTranslation))) //line a
        *rotationMatrix(vec3(p1XRotation,0.0,p16YZRotation), p1Rotation*RAD) //line b
        *translationMatrix(vec3(0.0,-3.5,0.0))
        ).xyz, vec3(cubeSize));

    float pyramid6 = sdPyramid((vec4(p,1.0) //back pyramid
        *translationMatrix(vec3(0.0,2.0,p6ZTranslation)) //line a
        *rotationMatrix(vec3(p6XRotation,p16YZRotation,0.0), p6Rotation*RAD)).xyz //line b
        ,0.5, 3.5, 3.5);
    float cube6 = udBox(((vec4(p,1.0)
        *translationMatrix(vec3(0.0,2.0,p6ZTranslation))) //line a
        *rotationMatrix(vec3(p6XRotation,p16YZRotation,0.0), p6Rotation*RAD) //line b
        *translationMatrix(vec3(0.0,-3.5,0.0))
        ).xyz, vec3(cubeSize));
    
    float pyramid2 = sdPyramid((vec4(p,1.0) //upper pyramid
        *translationMatrix(vec3(0.0,p2YTranslation,3.5)) //line a
        *rotationMatrix(vec3(p2XRotation,p23XYRotation,0.0), p2Rotation*RAD)).xyz //line b
        ,0.5, 3.5, 3.5);
    float cube2 = udBox(((vec4(p,1.0)
        *translationMatrix(vec3(0.0,p2YTranslation,3.5))) //line a
        *rotationMatrix(vec3(p2XRotation,p23XYRotation,0.0), p2Rotation*RAD) //line b
        *translationMatrix(vec3(0.0,-3.5,0.0))
        ).xyz, vec3(cubeSize));
    
    float pyramid3 = sdPyramid((vec4(p,1.0) //lower pyramid
        *translationMatrix(vec3(0.0,p3YTranslation,3.5)) //line a
        *rotationMatrix(vec3(p3XRotation,0.0,p23XYRotation), p3Rotation*RAD)).xyz //line b
        ,0.5, 3.5, 3.5);
    float cube3 = udBox(((vec4(p,1.0)
        *translationMatrix(vec3(0.0,p3YTranslation,3.5))) //line a
        *rotationMatrix(vec3(p3XRotation,0.0,p23XYRotation), p3Rotation*RAD) //line b
        *translationMatrix(vec3(0.0,-3.5,0.0))
        ).xyz, vec3(cubeSize));
    
    float pyramid4 = sdPyramid((vec4(p,1.0) //left pyramid
        *translationMatrix(vec3(p4XTranslation,2.0,3.5)) //line a
        *rotationMatrix(vec3(p45XYRotation,0.0,p4ZRotation), p4Rotation*RAD)).xyz //line b
        ,0.5, 3.5, 3.5);
    float cube4 = udBox(((vec4(p,1.0)
        *translationMatrix(vec3(p4XTranslation,2.0,3.5))) //line a
        *rotationMatrix(vec3(p45XYRotation,0.0,p4ZRotation), p4Rotation*RAD) //line b
        *translationMatrix(vec3(0.0,-3.5,0.0))
        ).xyz, vec3(cubeSize));

    float pyramid5 = sdPyramid((vec4(p,1.0) //right pyramid
        *translationMatrix(vec3(p5XTranslation,2.0,3.5)) //line a
        *rotationMatrix(vec3(0.0,p45XYRotation,p5ZRotation), p5Rotation*RAD)).xyz //line b
        ,0.5, 3.5, 3.5);
    float cube5 = udBox(((vec4(p,1.0)
        *translationMatrix(vec3(p5XTranslation,2.0,3.5))) //line a
        *rotationMatrix(vec3(0.0,p45XYRotation,p5ZRotation), p5Rotation*RAD) //line b
        *translationMatrix(vec3(0.0,-3.5,0.0))
        ).xyz, vec3(cubeSize));

    float pyramid1Subtracted = opI(cube1, pyramid1);
    float pyramid2Subtracted = opI(cube2, pyramid2);
    float pyramid3Subtracted = opI(cube3, pyramid3);
    float pyramid4Subtracted = opI(cube4, pyramid4);
    float pyramid6Subtracted = opI(cube6, pyramid6);
    float pyramid5Subtracted = opI(cube5, pyramid5);

    float a = min(pyramid1Subtracted, pyramid2Subtracted);
    float b = min(a, pyramid3Subtracted);
    float c = min(b, pyramid4Subtracted);
    float d = min(c, pyramid5Subtracted);
    float e = min(d, pyramid6Subtracted);

    return e;
}

vec4 raymarch2(vec3 rayOrigin, vec3 rayDir, out int steps)
{
    float totalDist = 0.0;
    for(int j=0; j<MAXSTEPS; j++)
    {
        steps = j;
        vec3 p = rayOrigin + totalDist*rayDir;
        float d = dist2(p);
        if(abs(d)<EPSILON)  //if it is near the surface, return an intersection
        {
            return vec4(p, 1.0);
        }
        totalDist += d;
        if(totalDist>=MAXDEPTH) break;
    }
    return vec4(0);
}

float ambientOcclusion2(vec3 p, vec3 n)
{
    float res = 0.0;
    float fac = 1.0;
    for(float i=0.0; i<AOSAMPLES; i++)
    {
        float distOut = i*0.3;  //go on normal ray AOSAMPLES times with factor 0.3
        res += fac * (distOut - dist2(p + n*distOut));   //look for every step, how far the nearest object is
        fac *= 0.5; //for every step taken on the normal ray, the fac decreases, so the shadow gets brighter
    }
    return 1.0 - clamp(res, 0.0, 1.0);
}

float shadow2(vec3 ro, vec3 rd)
{
    float res = 1.0;
    for( float t=0.01; t<32.0; )
    {
        float h = dist2(ro + rd*t);
        if( h<EPSILON )
            return AMBIENT;
        res = min( res, SOFTSDHADOWFAC*h/t );
        t += h;
    }
    return res;
}

vec3 getNormal2(vec3 p)
{
    float h = EPSILON;
    return normalize(vec3(
        dist2(p + vec3(h, 0, 0)) - dist2(p - vec3(h, 0, 0)),
        dist2(p + vec3(0, h, 0)) - dist2(p - vec3(0, h, 0)),
        dist2(p + vec3(0, 0, h)) - dist2(p - vec3(0, 0, h))));
}

vec3 lighting2(vec3 pos, vec3 rd, vec3 n)
{
    vec3 light = vec3(max(AMBIENT, dot(n, lightDir2)));  //lambert light with light Color

    //specular
    vec3 reflection = normalize(reflect(lightDir2, n));
    vec3 viewDirection = normalize(pos);
    float spec = max(AMBIENT, dot(reflection, viewDirection));

    light += pow(spec, SPECULAR);

    //light *= shadow(pos, lightDir2);
    light += ambientOcclusion2(pos, n) * AMBIENT;
    return light;
}
/**************/

vec4 getShader2Color(vec2 resolution, float time)
{
    shaderTime2 = time;
 
    float tanFov = tan(90.0 / 2.0 * 3.14159 / 180.0) / resolution.x;
    vec2 p = tanFov * (gl_FragCoord.xy * 2.0 - resolution.xy);

    /*cam.pos = vec3(0,0.0,-20.0);
    cam.dir = normalize(vec4( p.x, p.y, 1,1 )*(rotationMatrix(vec3(1.0,0.0,0.0), 0.0))).xyz;*/
    cam2.pos = vec3(-2,8.0,-20.0);
    cam2.dir = normalize(vec4( p.x, p.y, 1,1 )*(rotationMatrix(vec3(1.0,0.0,0.0), 0.6))).xyz;

    // marching and shading
	int steps = -1;
    vec4 res = raymarch2(cam2.pos, cam2.dir, steps);    
    vec3 currentCol = vec3(1);

    if(res.a==1.0)
    {
        currentCol = color2;
        vec3 n = getNormal2(res.xyz);

        currentCol *= lighting2(res.xyz, cam2.dir, n);
    }
//    return vec4(1.0);

   return vec4(currentCol, 1.0);
	
}

