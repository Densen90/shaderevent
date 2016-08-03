uniform float floating2;

#define RAD PI / 180.0
#define PI 3.14159
#define AMBIENT 0.1
#define EPSILON 0.001
#define MAXSTEPS 256
#define MAXDEPTH 60.0
#define AOSAMPLES 5.0
#define SOFTSDHADOWFAC 2.0

vec3 lightDir2 = normalize(vec3(0.5,0.5,-1));
vec2 uv7;

struct Camera{
    vec3 pos;
    vec3 dir;
};

vec3 rotate7( vec3 p, vec3 r )
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

vec3 repeat(vec3 p, vec3 c ){
    return mod(p, c) - 0.5 * c;
}

float distQuad7(vec3 point, vec3 whd){
	return length(max(abs(point)-whd,0.0))-0.1;
}

float createM(vec3 p){
    float q1 = distQuad7(p, vec3(2,2.5,0.5));
    float q2 = distQuad7(p - vec3(0,-1,0), vec3(1.0,2,0.6));    
    return max(q1,-q2);
}

float createU(vec3 p){
    float q1 = distQuad7(p, vec3(2,2.5,0.5));
    float q2 = distQuad7(p - vec3(0,1,0), vec3(1.0,2,0.6));    
    return max(q1,-q2);
}

float createS(vec3 p){
    float q1 = distQuad7(p, vec3(1.5,2.5,0.5));
    float q2 = distQuad7(p - vec3(0.5,1,0), vec3(1.2,0.5,0.6));
    float q3 = distQuad7(p - vec3(-0.5,-1,0), vec3(1.2,0.5,0.6));
    float res = max(q1,-q2);
    return max(res,-q3);
}

float createI(vec3 p){
    float q1 = distQuad7(p, vec3(0.5,2.5,0.5));
    return q1;
}
float createC2(vec3 p){
    float q1 = distQuad7(p, vec3(1.5,2.5,0.5));
    float q2 = distQuad7(p - vec3(0.5,0,0), vec3(1.05,1.5,0.6));    
    return max(q1,-q2);
}

float createB2(vec3 p){
    float q1 = distQuad7(p, vec3(1.5,2.5,0.5));
    float q3 = distQuad7(p - vec3(1,1.5,0), vec3(1.5,1.05,0.6));
    float q2 = distQuad7(p - vec3(0,-1,0), vec3(0.5,0.5,0.6));
    float res = max(q1,-q2);
    return max(res,-q3);
}

float createY2(vec3 p){
    float q1 = distQuad7(p - vec3(0,-2.25,0), vec3(1.5,2.5,0.5));
    float q3 = distQuad7(p , vec3(0.75,1.0,0.6));
    float q2 = distQuad7(p - vec3(-0.5,-3,0), vec3(1.25,0.5,0.6));
    float res = max(q1,-q2);
    return max(res,-q3);
}

float createD2(vec3 p){
    float q1 = distQuad7(p - vec3(0,0,0), vec3(1.5,2.5,0.5));
    float q2 = distQuad7(p - vec3(0,0,0), vec3(0.75,1.5,0.6));    
    return max(q1,-q2);
}

float createJ(vec3 p){
    float q1 = distQuad7(p - vec3(0,0,0), vec3(1.5,2.5,0.5));
    float q2 = distQuad7(p - vec3(-.5,0,0), vec3(1.05,1.5,0.6));    
    return max(q1,-q2);
}

float createN(vec3 p){
    float q1 = distQuad7(p - vec3(0,0,0), vec3(0.5,2.5,0.5));
    float q2 = distQuad7(p - vec3(2.,0,0), vec3(0.5,2.5,0.5));
    p = rotate7(p, vec3(0,0,15));
    float q3 = distQuad7(p-vec3(1,-0.3,0), vec3(0.5,2.5,0.5));
    return min(q1,min(q2,q3));
}

float createO2(vec3 p){
    float q1 = distQuad7(p, vec3(1.5,2.5,0.5));
    float q2 = distQuad7(p, vec3(0.75,1.5,0.6));    
    return max(q1,-q2);
}

float dist7(vec3 p) { 
	p.x-= floating2;
	p = rotate7(p, vec3(0,-30,0));  
	
    p+=vec3(12,8,0); 
	float music = createM(p);
    p-=vec3(5,0,0); 
	music = min(music,createU(p));
    p-=vec3(4.5,0,0); 
	music = min(music,createS(p));
    p-=vec3(3,0,0); 
	music = min(music,createI(p));
    p-=vec3(3,0,0); 
    music = min(music,createC2(p));

	p-=vec3(6,0,0); 
    float by = createB2(p);
    p-=vec3(4,0,0); 
    by = min(by,createY2(p));    
    vec3 p2 = p;


    p-=vec3(6,0,5);  

	float DJ_Innos = createD2(p);
    p-=vec3(4,0,0); 
	DJ_Innos = min(DJ_Innos,createJ(p));

    p-=vec3(5,0,0); 
	DJ_Innos = min(DJ_Innos,createI(p));
    p-=vec3(1.5,0,0);  
	DJ_Innos = min(DJ_Innos,createN(p));
    p-=vec3(3.5,0,0);  
	DJ_Innos = min(DJ_Innos,createN(p));
	p-=vec3(4.5,0,0);  
    DJ_Innos = min(DJ_Innos,createO2(p));
	p-=vec3(3.5,0,0);  
    DJ_Innos = min(DJ_Innos,createS(p));
    
	return min(music, min(by, DJ_Innos));
}


vec3 getNormal7(vec3 p){
    float h = EPSILON;
    return normalize(vec3(
        dist7(p + vec3(h, 0, 0)) - dist7(p - vec3(h, 0, 0)),
        dist7(p + vec3(0, h, 0)) - dist7(p - vec3(0, h, 0)),
        dist7(p + vec3(0, 0, h)) - dist7(p - vec3(0, 0, h))));
}

float calcLight7(vec3 point){
    vec3 normal = getNormal7(point);  
    return max(AMBIENT, dot(normal, lightDir2));
}

vec4 raymarch7(vec3 rayOrigin, vec3 rayDir, out int steps){
    float totalDist = 0.0;
    for(int j=0; j<MAXSTEPS; j++)
    {
        steps = j;
        vec3 p = rayOrigin + totalDist*rayDir;
        float d = dist7(p);
        if(abs(d)<EPSILON)  //if it is near the surface, return an intersection
        {
            return vec4(p, 1.0);
        }
        totalDist += d;
        if(totalDist>=MAXDEPTH) break;
    }
    return vec4(0);
}

float ambientOcclusion7(vec3 p, vec3 n)
{
    float res = 0.0;
    float fac = 1.0;
    for(float i=0.0; i<AOSAMPLES; i++)
    {
        float distOut = i*0.3;  //go on normal ray AOSAMPLES times with factor 0.3
        res += fac * (distOut - dist7(p + n*distOut));   //look for every step, how far the nearest object is
        fac *= 0.5; //for every step taken on the normal ray, the fac decreases, so the shadow gets brighter
    }
    return 1.0 - clamp(res, 0.0, 1.0);
}

float shadow7(vec3 ro, vec3 rd)
{
    float res = 1.0;
    for( float t=0.01; t<32.0; )
    {
        float h = dist7(ro + rd*t);
        if( h<EPSILON )
            return AMBIENT;
        res = min( res, SOFTSDHADOWFAC*h/t );
        t += h;
    }
    return res;
}

vec4 getOutroColor2(Camera cam){		 	

    // marching and shading
	int steps = -1;
    vec4 res = raymarch7(cam.pos, cam.dir, steps);  

	vec3 color;
	if(res.a == 1){										// not sure why this is needed ...
		color = mix(vec3(0.2,0.2,0.8), vec3(0,0.35,0.3), (res.y + EPSILON));
		vec3 diffuse =  vec3(calcLight7(res.xyz));	
		diffuse *= shadow7(res.xyz, lightDir2);
		diffuse += ambientOcclusion7(res.xyz, getNormal7(res.xyz)) * AMBIENT;
		color *= diffuse;
	}   
	
	return vec4(color,1);
}

