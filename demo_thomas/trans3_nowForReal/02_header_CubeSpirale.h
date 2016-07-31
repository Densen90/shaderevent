vec3 lightDirSpiral = vec3(0.5,0.5,-1);

#define PI 3.14159
#define AMBIENT 0.1
#define EPSILON 0.001
#define MAXSTEPS 256
#define MAXDEPTH 60.0
#define AOSAMPLES 5.0

uniform float spiralRot;
float spiralTime;
vec3 rot2;
vec2 uv2;

struct Camera
{
    vec3 pos;
    vec3 dir;
};

vec3 repeatSpiral(vec3 p, vec3 c ){
    return mod(p, c) - 0.5 * c;
}

float distRoundBox2(vec3 p, vec3 b, float r)
{
 	return length(max(abs(p)-b,0.0))-r;
}

vec3 rotate2( vec3 p, vec3 r ){
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

float marchCubeFrame(vec3 p, float size){
	float cube = distRoundBox2(p, vec3(size,size,0.01),0.001);
	float hole = distRoundBox2(p, vec3(0.8*size,0.8*size,0.02),0.001);
	return max(cube, -hole);
}

float spirale(vec3 p, float size){
	float rot = spiralRot;		
	vec3 p0 = rotate2(p, vec3(0,0,rot));
	float frame1 = marchCubeFrame(p0,3 * size);
	p0 = rotate2(p, vec3(0,0,-rot));	
	frame1 = min(frame1, marchCubeFrame(p0,1.8 * size));
	//float frame2 = ;
	p0 = rotate2(p, vec3(0,0,rot));
	frame1 = min(frame1, marchCubeFrame(p0,1.1 * size));
	p0 = rotate2(p, vec3(0,0,-rot));
	frame1 = min(frame1, marchCubeFrame(p0,0.7 * size));	
	p0 = rotate2(p, vec3(0,0,rot));
	frame1 = min(frame1, marchCubeFrame(p0,0.4 * size));	
	p0 = rotate2(p, vec3(0,0,-rot));
	frame1 = min(frame1, marchCubeFrame(p0,0.23 * size));	
	p0 = rotate2(p, vec3(0,0,rot));
	frame1 = min(frame1, marchCubeFrame(p0,0.16 * size));	
	p0 = rotate2(p, vec3(0,0,-rot));
	frame1 = min(frame1, marchCubeFrame(p0,0.08 * size));	
	return frame1;
}
 
float dist2(vec3 p){
	p = rotate2(p, rot2);
	p = rotate2(p, vec3(0,90,0));
	//float plane = distPlane(p + vec3(0,8,0), vec3(0,1,0));
	float frame1 = spirale(repeatSpiral(p, vec3(2,2,0)), 0.5);	
	return frame1;
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

vec3 getNormal2(vec3 p)
{
    float h = EPSILON;
    return normalize(vec3(
        dist2(p + vec3(h, 0, 0)) - dist2(p - vec3(h, 0, 0)),
        dist2(p + vec3(0, h, 0)) - dist2(p - vec3(0, h, 0)),
        dist2(p + vec3(0, 0, h)) - dist2(p - vec3(0, 0, h))));
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

vec4 getSpiralColor(Camera cam, vec2 uv, vec3 cubeRotation){
    rot2 = cubeRotation;

	int steps = -1;
	vec4 res = raymarch2(cam.pos, cam.dir, steps);
	vec4 color = vec4(0.247, 0.278, 0.729, 1.0);	
	uv2 = uv-0.5;

	if(res.a ==1){
		color = color = mix(vec4(0.608, 0.780, 0.0  , 1.0), vec4(0.247, 0.278, 0.729, 1.0), abs(uv2.x)) ;
		vec3 n = getNormal2(res.xyz);
		color *= max(AMBIENT, dot(n, lightDirSpiral));
		//color *=shadow(res.xyz,n);
		color +=ambientOcclusion2(res.xyz,n) * AMBIENT;
		color *=1;
	}
	return color;
}