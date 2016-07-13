#include "func_collections.h"

uniform float sat;
uniform float shake;

uniform sampler2D tex;

uniform float floating;

in vec2 uv;


#define RAD PI / 180.0

float iterations = 128.0;
float epsilon = 0.01;

vec3 lightDir = normalize(vec3(0.5,0.5,-1));


vec3 repeat(vec3 p, vec3 c ){
    return mod(p, c) - 0.5 * c;
}
/*
 * idea: let the letters appear from the right floating and docking to their place
 *       maybe let the camera shake on impact
 *       add some particles / damage to the letters **??**
 */

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
/*
float distLetter(vec3 p, vec3 whd){
    return distQuad(p, vec3(whd)*0.5);
}*/

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
    vec3 pl = rotate(p, vec3(0,0,20));	
	vec3 pr = rotate(p, vec3(0,0,-20));	
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

float dist(vec3 p) { 
    p.x-= floating;

	//vec3 ori = p;
    p+=vec3(16,-5,0);  
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
    p-=vec3(8,0,0);

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
    //return min(l, b2);*/
	//return x;
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

