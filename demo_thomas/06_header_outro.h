uniform float shake;
uniform float floating;

uniform sampler2D tex0;

vec2 uv6;

#define RAD PI / 180.0
#define PI 3.14159
#define AMBIENT 0.1
#define EPSILON 0.001
#define MAXSTEPS 256
#define MAXDEPTH 60.0
#define AOSAMPLES 5.0
#define SOFTSDHADOWFAC 2.0

vec3 lightDir = normalize(vec3(0.5,0.5,-1));
float introTime;

struct Camera{
    vec3 pos;
    vec3 dir;
};

vec3 rotate6( vec3 p, vec3 r )
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

/*
 * idea: let the letters appear from the right floating and docking to their place
 *       maybe let the camera shake on impact
 *       add some particles / damage to the letters **??**
 */

float noise6(){
    return texture2D(tex0,uv6).r - 0.25;
}


float distQuad(vec3 point, vec3 whd){
	return length(max(abs(point)-whd,0.0))-0.1;
}

float createC(vec3 p){
    float q1 = distQuad(p + vec3(0.5,0,0), vec3(1.5,2.5,0.5));
    float q2 = distQuad(p, vec3(1.05,1.5,0.6));    
    return max(q1,-q2);
}

float createR(vec3 p){
    float q1 = distQuad(p, vec3(0.5,2.5,0.5));
    float q2 = distQuad(p - vec3(1.0,2,0), vec3(0.5,0.5,0.5));    
    return min(q1,q2);
}

float createE(vec3 p){
    float q1 = distQuad(p, vec3(1.25,2.5,0.5));
    float q2 = distQuad(p - vec3(1,1,0), vec3(1.2,0.5,0.6));
    float q3 = distQuad(p - vec3(1,-1,0), vec3(1.2,0.5,0.6));
    float res = max(q1,-q2);
    return max(res,-q3);
}

float createA(vec3 p){
    float q1 = distQuad(p, vec3(1.5,2.5,0.5));
    float q2 = distQuad(p - vec3(0,1,0), vec3(0.5,0.5,0.6));
    float q3 = distQuad(p - vec3(0,-1.5,0), vec3(0.5,1.05,0.6));
    float res = max(q1,-q2);
    return max(res,-q3);
}
float createT(vec3 p){
    float q1 = distQuad(p, vec3(0.5,2.5,0.5));
    float q2 = distQuad(p - vec3(0,2,0), vec3(1.5,0.5,0.5));    
    return min(q1,q2);
}

float createD(vec3 p){
    float q1 = distQuad(p, vec3(1.5,2.5,0.5));
    float q3 = distQuad(p - vec3(-1,1.5,0), vec3(1.5,1.05,0.6));
    float q2 = distQuad(p - vec3(0,-1,0), vec3(0.5,0.5,0.6));
    float res = max(q1,-q2);
    return max(res,-q3);
}

float createB(vec3 p){
    float q1 = distQuad(p - vec3(0,0,0), vec3(1.5,2.5,0.5));
    float q3 = distQuad(p - vec3(1,1.5,0), vec3(1.5,1.05,0.6));
    float q2 = distQuad(p - vec3(0,-1,0), vec3(0.5,0.5,0.6));
    float res = max(q1,-q2);
    return max(res,-q3);
}

float createY(vec3 p){
    float q1 = distQuad(p - vec3(0,-2,0), vec3(1.5,2.5,0.5));
    float q3 = distQuad(p - vec3(0,0,0), vec3(0.75,1.0,0.6));
    float q2 = distQuad(p - vec3(-.5,-3,0), vec3(1.25,0.5,0.6));
    float res = max(q1,-q2);
    return max(res,-q3);
}

float createL(vec3 p){
    float q1 = distQuad(p , vec3(0.5,2.5,0.5));
    return q1;
}

float createO(vec3 p){
    float q1 = distQuad(p, vec3(1.5,2.5,0.5));
    float q2 = distQuad(p, vec3(0.75,1.5,0.6));    
    return max(q1,-q2);
}

float createX(vec3 p){  
    vec3 pl = rotate6(p, vec3(0,0,20));  
    vec3 pr = rotate6(p, vec3(0,0,-20)); 
    float q1 = distQuad(pl, vec3(0.5,2.5,0.5));
    float q2 = distQuad(pr, vec3(0.5,2.5,0.5));   
    return min(q1,q2);
}

float createG(vec3 p){
    float q1 = distQuad(p - vec3(0,-2,0), vec3(1.5,2.5,0.5));
    float q3 = distQuad(p - vec3(0,-1,0), vec3(0.75,0.75,0.6));
    float q2 = distQuad(p - vec3(-0.5,-3,0), vec3(1.25,0.5,0.6));
    float res = max(q1,-q2);
    return max(res,-q3);
}

float dist6(vec3 p) { 
    p.x-= floating;
	p = rotate6(p, vec3(0,-30,0));  	

    //vec3 ori = p;
    p+=vec3(5,-5,0);  
    float c = createC(p);
 
    p-=vec3(3,0,0);
    float r = createR(p);
    p-=vec3(4,0,0);
    float e = createE(p);
    p-=vec3(4,0,0);
    float a = createA(p);
    p-=vec3(4,0,0);
    float t = createT(p);
    p-=vec3(4,0,0);
    float e2 = createE(p);
    p-=vec3(4,0,0);
    float d = createD(p);
    p-=vec3(6,0,0);
    float b = createB(p);
    p-=vec3(4,0,0);
    float y = createY(p);
    float str =  min(c,min(r,min(e,min(a,min(t,min(e2,min(d, min(b,y))))))));

    //p+=vec3(30,8,0);  
    p-=vec3(8,0,5);

    float b2 = createB(p);
    p-=vec3(3,0,0);  
    float l = createL(p);
    p-=vec3(3,0,0);  
    float o = createO(p);
    p-=vec3(3.5,0,0);  
    float x = createX(p);   
    p-=vec3(3.5,0,0);  
    float b3 = createB(p);
    p-=vec3(3.5,0,0);  
    float e3 = createE(p);  
    p-=vec3(3,0,0);  
    float r2 = createR(p);
    p-=vec3(3.5,0,0);  
    float g = createG(p);

    float name = min(b2,min(l,min(o,min(x,min(b3,min(e3,min(r2,g)))))));    
    
    return min(str, name); 

}


vec3 getNormal1(vec3 p)
{
    float h = EPSILON;
    return normalize(vec3(
        dist6(p + vec3(h, 0, 0)) - dist6(p - vec3(h, 0, 0)),
        dist6(p + vec3(0, h, 0)) - dist6(p - vec3(0, h, 0)),
        dist6(p + vec3(0, 0, h)) - dist6(p - vec3(0, 0, h))));
}

float calcLight(vec3 point){
    vec3 normal = getNormal1(point);  
    return max(AMBIENT, dot(normal, lightDir));
}

vec4 raymarch6(vec3 rayOrigin, vec3 rayDir, out int steps)
{
    float totalDist = 0.0;
    for(int j=0; j<MAXSTEPS; j++)
    {
        steps = j;
        vec3 p = rayOrigin + totalDist*rayDir;
        float d = dist6(p);
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
        res += fac * (distOut - dist6(p + n*distOut));   //look for every step, how far the nearest object is
        fac *= 0.5; //for every step taken on the normal ray, the fac decreases, so the shadow gets brighter
    }
    return 1.0 - clamp(res, 0.0, 1.0);
}

float shadow1(vec3 ro, vec3 rd)
{
    float res = 1.0;
    for( float t=0.01; t<32.0; )
    {
        float h = dist6(ro + rd*t);
        if( h<EPSILON )
            return AMBIENT;
        res = min( res, SOFTSDHADOWFAC*h/t );
        t += h;
    }
    return res;
}

vec4 getOutroColor(Camera cam, vec2 uv){
    
  
	uv6 = uv;	

    // marching and shading
	int steps = -1;
    vec4 res = raymarch6(cam.pos, cam.dir, steps);  


	vec3 color;
	if(res.a == 1){										// not sure why this is needed ...
		color = mix(vec3(0.2,0.2,0.8), vec3(0,0.35,0.3), (res.y + EPSILON));
		vec3 diffuse =  vec3(calcLight(res.xyz));	
		diffuse *= shadow1(res.xyz, lightDir);
		diffuse += ambientOcclusion1(res.xyz, getNormal1(res.xyz)) * AMBIENT;
		color *= diffuse;
	}   
	
	return vec4(color,1);
}

