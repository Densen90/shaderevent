uniform float p1Rotation;
uniform float p16Translation;
uniform float p3Rotation;
uniform float p23Translation;
uniform float p4Rotation;
uniform float p45Translation;

#define SPECULAR 20
#define PI 3.14159
#define AMBIENT 0.1
#define EPSILON 0.001
#define MAXSTEPS 256
#define MAXDEPTH 60.0
#define AOSAMPLES 5.0
#define SOFTSDHADOWFAC 2.0
#define RAD PI / 180.0

const float glowEpsiolon = 0.2;

vec3 glowPyramids = vec3(0);
vec3 lightDirPyramids = normalize(vec3(0.8,0.8,-1));
vec3 globalColor = vec3(1);

struct Camera
{
    vec3 pos;
    vec3 dir;
} cam2;

vec3 rotatePyramids( vec3 p, vec3 r )
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

float distPyramids(vec3 p)
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

vec4 raymarchPyramids(vec3 rayOrigin, vec3 rayDir, out int steps)
{
    float totalDist = 0.0;
    glowPyramids = vec3(0);
    for(int j=0; j<MAXSTEPS; j++)
    {
        steps = j;
        vec3 p = rayOrigin + totalDist*rayDir;
        float d = distPyramids(p);
        if(abs(d)<EPSILON)  //if it is near the surface, return an intersection
        {
            return vec4(p, 1.0);
        }
        if(d < glowEpsiolon)
        {
            glowPyramids +=  globalColor;
        }
        totalDist += d;
        if(totalDist>=MAXDEPTH) break;
    }
    return vec4(0);
}

float ambientOcclusionPyramids(vec3 p, vec3 n)
{
    float res = 0.0;
    float fac = 1.0;
    for(float i=0.0; i<AOSAMPLES; i++)
    {
        float distOut = i*0.3;  //go on normal ray AOSAMPLES times with factor 0.3
        res += fac * (distOut - distPyramids(p + n*distOut));   //look for every step, how far the nearest object is
        fac *= 0.5; //for every step taken on the normal ray, the fac decreases, so the shadow gets brighter
    }
    return 1.0 - clamp(res, 0.0, 1.0);
}

float shadowPyramids(vec3 ro, vec3 rd)
{
    float res = 1.0;
    for( float t=0.01; t<32.0; )
    {
        float h = distPyramids(ro + rd*t);
        if( h<EPSILON )
            return AMBIENT;
        res = min( res, SOFTSDHADOWFAC*h/t );
        t += h;
    }
    return res;
}

vec3 getNormalPyramids(vec3 p)
{
    float h = EPSILON;
    return normalize(vec3(
        distPyramids(p + vec3(h, 0, 0)) - distPyramids(p - vec3(h, 0, 0)),
        distPyramids(p + vec3(0, h, 0)) - distPyramids(p - vec3(0, h, 0)),
        distPyramids(p + vec3(0, 0, h)) - distPyramids(p - vec3(0, 0, h))));
}

vec3 lightingPyramids(vec3 pos, vec3 rd, vec3 n)
{
    vec3 light = vec3(max(AMBIENT, dot(n, lightDirPyramids)));  //lambert light with light Color

    //specular
    vec3 reflection = normalize(reflect(lightDirPyramids, n));
    vec3 viewDirection = normalize(pos);
    float spec = max(AMBIENT, dot(reflection, viewDirection));

    light += pow(spec, SPECULAR);

    //light *= shadow(pos, lightDir2);
    light += ambientOcclusionPyramids(pos, n) * AMBIENT;
    return light;
}
/**************/

vec4 getShaderPyramidsColor(vec2 resolution, float time2, vec2 fragCoord)
{
    
 
    float tanFov = tan(60.0 / 2.0 * 3.14159 / 180.0) / resolution.x;
    vec2 p = tanFov * (gl_FragCoord.xy * 2.0 - resolution.xy);

    /*cam.pos = vec3(0,0.0,-20.0);
    cam.dir = normalize(vec4( p.x, p.y, 1,1 )*(rotationMatrix(vec3(1.0,0.0,0.0), 0.0))).xyz;*/
    vec4 tmp = vec4(40.0, 15.0, -1.0, 1.0)*rotationMatrix(vec3(0.0,1.0,0.0), time2)
        *translationMatrix(vec3(0,-3,0));
	cam2.pos = tmp.xyz;
    cam2.dir = normalize(vec3( p.x, p.y, 1));
    cam2.dir = (lookAt(cam2.pos, vec3(0,-3,0), vec3(0.0,1.0,0.0))*vec4(cam2.dir.xyz, 1.0)).xyz;

    // marching and shading
	int steps = -1;
    vec4 res = raymarchPyramids(cam2.pos, cam2.dir, steps);    
    vec3 currentCol = vec3(0.0);

    if(res.a==1.0)
    {
        currentCol = globalColor;//+glowPyramids*0.1 ;
        vec3 n = getNormalPyramids(res.xyz);

        currentCol *= lightingPyramids(res.xyz, cam2.dir, n)*2;
        return vec4(currentCol, 1.0);
    }
    else
    {
        vec2 uv = fragCoord / resolution.xy;
        vec2 coord = (uv - 0.5) * (resolution.x/resolution.y) * 2.0;
        float rf = sqrt(dot(coord, coord)) * 0.25;
        float rf2_1 = rf * rf + 1.0;
        float e = 1.0 / (rf2_1 * rf2_1);
        
        vec4 src = vec4(1.0,1.0,1.0,1.0);
        src.rgb.y = src.rgb.y*2;
        return vec4(src.rgb * e, 1.0);
    }
	
}

