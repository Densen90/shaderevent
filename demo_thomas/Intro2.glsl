#include "func_collections.h"

uniform float sat;
uniform float shake;

uniform sampler2D tex;

in vec2 uv;


#define RAD PI / 180.0

float iterations = 128.0;
float epsilon = 0.01;

vec3 lightDir = normalize(vec3(0.5,0.5,-1));


vec3 repeat(vec3 p, vec3 c ){
    return mod(p, c) - 0.5 * c;
}


float noise(){
    return texture2D(tex,uv).r - 0.25;
}


// width, height, depth
float distQuad2(vec3 point, vec3 whd){   
  vec3 d = abs(point) - whd;
  return min(max(d.x,max(d.y,d.z)),0.0) +
         length(max(d,0.0));
}

float distQuad(vec3 point, vec3 whd){
	return distRoundBox(point, whd, 0.1);
}

float createM(vec3 p){
    float q1 = distQuad(p, vec3(2,2.5,0.5));
    float q2 = distQuad(p - vec3(0,-1,0), vec3(1.0,2,0.6));    
    return max(q1,-q2);
}

float createU(vec3 p){
    float q1 = distQuad(p, vec3(2,2.5,0.5));
    float q2 = distQuad(p - vec3(0,1,0), vec3(1.0,2,0.6));    
    return max(q1,-q2);
}

float createS(vec3 p){
    float q1 = distQuad(p, vec3(1.5,2.5,0.5));
    float q2 = distQuad(p - vec3(0.5,1,0), vec3(1.2,0.5,0.6));
    float q3 = distQuad(p - vec3(-0.5,-1,0), vec3(1.2,0.5,0.6));
    float res = max(q1,-q2);
    return max(res,-q3);
}

float createI(vec3 p){
    float q1 = distQuad(p, vec3(0.5,2.5,0.5));
    return q1;
}
float createC(vec3 p){
    float q1 = distQuad(p, vec3(1.5,2.5,0.5));
    float q2 = distQuad(p - vec3(0.5,0,0), vec3(1.05,1.5,0.6));    
    return max(q1,-q2);
}

float createB(vec3 p){
    float q1 = distQuad(p, vec3(1.5,2.5,0.5));
    float q3 = distQuad(p - vec3(1,1.5,0), vec3(1.5,1.05,0.6));
    float q2 = distQuad(p - vec3(0,-1,0), vec3(0.5,0.5,0.6));
    float res = max(q1,-q2);
    return max(res,-q3);
}

float createY(vec3 p){
    float q1 = distQuad(p - vec3(0,-2.25,0), vec3(1.5,2.5,0.5));
    float q3 = distQuad(p , vec3(0.75,1.0,0.6));
    float q2 = distQuad(p - vec3(-0.5,-3,0), vec3(1.25,0.5,0.6));
    float res = max(q1,-q2);
    return max(res,-q3);
}

float createD(vec3 p){
    float q1 = distQuad(p - vec3(0,0,0), vec3(1.5,2.5,0.5));
    float q2 = distQuad(p - vec3(0,0,0), vec3(0.75,1.5,0.6));    
    return max(q1,-q2);
}

float createJ(vec3 p){
    float q1 = distQuad(p - vec3(0,0,0), vec3(1.5,2.5,0.5));
    float q2 = distQuad(p - vec3(-.5,0,0), vec3(1.05,1.5,0.6));    
    return max(q1,-q2);
}

float createN(vec3 p){
    float q1 = distQuad(p - vec3(0,0,0), vec3(0.5,2.5,0.5));
    float q2 = distQuad(p - vec3(2.,0,0), vec3(0.5,2.5,0.5));
    p = rotate(p, vec3(0,0,15));
    float q3 = distQuad(p-vec3(1,-0.3,0), vec3(0.5,2.5,0.5));
    return min(q1,min(q2,q3));
}

float createO(vec3 p){
    float q1 = distQuad(p, vec3(1.5,2.5,0.5));
    float q2 = distQuad(p, vec3(0.75,1.5,0.6));    
    return max(q1,-q2);
}

float dist(vec3 p) { 
	//vec3 ori = p;
    p+=vec3(12,-5,0); 
	float music = createM(p);
    p-=vec3(5,0,0); 
	music = min(music,createU(p));
    p-=vec3(4.5,0,0); 
	music = min(music,createS(p));
    p-=vec3(3,0,0); 
	music = min(music,createI(p));
    p-=vec3(3,0,0); 
    music = min(music,createC(p));

	p-=vec3(6,0,0); 
    float by = createB(p);
    p-=vec3(4,0,0); 
    by = min(by,createY(p));    
    vec3 p2 = p;


    p-=vec3(6,0,0);  

	float DJ_Innos = createD(p);
    p-=vec3(4,0,0); 
	DJ_Innos = min(DJ_Innos,createJ(p));

    p-=vec3(5,0,0); 
	DJ_Innos = min(DJ_Innos,createI(p));
    p-=vec3(1.5,0,0);  
	DJ_Innos = min(DJ_Innos,createN(p));
    p-=vec3(3.5,0,0);  
	DJ_Innos = min(DJ_Innos,createN(p));
	p-=vec3(4.5,0,0);  
    DJ_Innos = min(DJ_Innos,createO(p));
	p-=vec3(3.5,0,0);  
    DJ_Innos = min(DJ_Innos,createS(p));
    /*

    
    
    return name;
    //return min(l, b2);*/
	//return x;*/
	return min(music, min(by, DJ_Innos));
}

float calcLight(vec3 point){
    vec3 normal = getNormal(point);  
    return max(AMBIENT, dot(normal, lightDir));
}


void main(){
    
    // init cam, add shake    
    vec2 p = getScreenPos(90);
    float factor = noise() * 2;
	Camera cam;
    cam.pos = vec3(0 , 0 , -25) ;
    //vec3 camDir = normalize(vec3(p.x,p.y,1));
    cam.dir = normalize(vec3(p.x + factor * shake, p.y + factor * shake, 1.0));

    
	// use uv to draw bg
	vec2 uv = gl_FragCoord.xy / iResolution.xy;
    uv -=0.5;
    float point = length(uv);    
    vec3 bg = mix(vec3(0.78,0.32,0.78),vec3(0.2,0.8,0.35), point);    

    // marching and shading
	int steps = -1;
    vec4 res = raymarch(cam.pos, cam.dir, steps);  


	vec3 color = bg;
	if(res.a == 1){										// not sure why this is needed ...
		color = mix(vec3(0.2,0.2,0.8), vec3(0,0.35,0.3), (res.y + EPSILON));
		vec3 diffuse =  vec3(calcLight(res.xyz));	
		diffuse *= shadow(res.xyz, lightDir);
		diffuse += ambientOcclusion(res.xyz, getNormal(res.xyz)) * AMBIENT;
		color *=diffuse;
	}

	/*	
    float col2 = primary.hitObject ? calcLight(primary.point, vec3(4,1,-1)) : 0;
    float col3 = primary.hitObject ? calcLight(primary.point, vec3(4,0,0)) : 0;

    float total = (col1 + col2 + col3) / 4.5;
	
	float total = ambientOcclusion(primary.point, getNormal(primary.point));
    // background, using polar coordinates
	*/

    //vec4 col =  vec4(1-total, 0, 1-total, 0);   
	//diffuse += 1-bg;    
	
	gl_FragColor = vec4(color,1);


}

