#define RAD PI / 180.0
#define PI 3.14159
#define AMBIENT 0.1
#define EPSILON 0.001
#define MAXSTEPS 256
#define MAXDEPTH 60.0
#define AOSAMPLES 5.0
#define SOFTSDHADOWFAC 2.0

uniform float divide;
uniform float tresh;
uniform float rep;
float cubeSize = 1;
float cubeRadius = cubeSize/4;
float dotSize = cubeRadius;

vec4 objColor4 = vec4(0);
vec3 rot4;
vec3 lightDir4 = vec3(0.5,0.5,-1);

struct Camera{
    vec3 pos;
    vec3 dir;
} ;

vec3 rotate4( vec3 p, vec3 r ){
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

vec3 repeat4(vec3 p, vec3 c ){
    return mod(p, c) - 0.5 * c;
}

float distSphere4(vec3 p, float rad){
	return length(p) - rad;
}

float distRoundBox4(vec3 p, vec3 b, float r){
 	return length(max(abs(p)-b,0.0))-r;
}

float oneDot4(vec3 p, vec3 offset, vec3 cutPlane){
    float cut = distRoundBox4(p + offset, cutPlane,0.01);
    float sphere = distSphere4(p + offset, dotSize);
    return max(cut,sphere);
}

float addDots4(in vec3 p) {  
    float one = oneDot4(p, vec3(0,cubeSize + cubeRadius,0), vec3(1,0.0001,1));

    float two = oneDot4(p, vec3(-cubeSize - cubeRadius, cubeSize/2,cubeSize/2), vec3(0.0001,1,1));
    two = min(two,oneDot4(p, vec3(-cubeSize - cubeRadius,-cubeSize/2,-cubeSize/2), vec3(0.0001,1,1)));

    float three = oneDot4(p, vec3(0,0,cubeSize + cubeRadius), vec3(1,1,0.0001));
    three = min(three,oneDot4(p, vec3(-cubeSize/2,-cubeSize/2,cubeSize + cubeRadius), vec3(1,1,0.0001)));
    three = min(three,oneDot4(p, vec3(cubeSize/2,cubeSize/2,cubeSize + cubeRadius), vec3(1,1,0.0001)));

    float four = oneDot4(p, vec3(-cubeSize/2,cubeSize/2,-cubeSize - cubeRadius), vec3(1,1,0.0001));
    four = min(four,oneDot4(p, vec3(-cubeSize/2,-cubeSize/2,-cubeSize - cubeRadius), vec3(1,1,0.0001)));
    four = min(four,oneDot4(p, vec3(cubeSize/2,cubeSize/2,-cubeSize - cubeRadius), vec3(1,1,0.0001)));
    four = min(four,oneDot4(p, vec3(cubeSize/2,-cubeSize/2,-cubeSize - cubeRadius), vec3(1,1,0.0001)));

    float five = oneDot4(p, vec3(cubeSize + cubeRadius,0.,0.), vec3(0.0001,1,1));
    five = min(five,oneDot4(p, vec3(cubeSize + cubeRadius,-cubeSize/2,-cubeSize/2), vec3(0.0001,1,1)));
    five = min(five,oneDot4(p, vec3(cubeSize + cubeRadius,cubeSize/2,cubeSize/2), vec3(0.0001,1,1)));
    five = min(five,oneDot4(p, vec3(cubeSize + cubeRadius,-cubeSize/2,cubeSize/2), vec3(0.0001,1,1)));
    five = min(five,oneDot4(p, vec3(cubeSize + cubeRadius,cubeSize/2,-cubeSize/2), vec3(0.0001,1,1)));

    float six = oneDot4(p, vec3(cubeSize/2 - cubeSize/10,-cubeSize - cubeRadius,cubeSize/2 + cubeSize/10), vec3(1,0.0001,1));
    six = min(six,oneDot4(p, vec3(-cubeSize/2 - cubeSize/10,-cubeSize - cubeRadius,cubeSize/2 + cubeSize/10), vec3(1,0.0001,1)));

    six = min(six,oneDot4(p, vec3(-cubeSize/2 - cubeSize/10,-cubeSize - cubeRadius,-0.), vec3(1,0.0001,1)));
    six = min(six,oneDot4(p, vec3(cubeSize/2 - cubeSize/10,-cubeSize - cubeRadius,-0.), vec3(1,0.0001,1)));

    six = min(six,oneDot4(p, vec3(-cubeSize/2 - cubeSize/10,-cubeSize - cubeRadius,-cubeSize/2 - cubeSize/10), vec3(1,0.0001,1)));
    six = min(six,oneDot4(p, vec3(cubeSize/2 - cubeSize/10,-cubeSize - cubeRadius,-cubeSize/2 - cubeSize/10), vec3(1,0.0001,1)));

    float seven = oneDot4(p, vec3(cubeSize/2 - cubeSize/2,-cubeSize - cubeRadius,-cubeSize/2 - cubeSize/4), vec3(1,0.0001,1));
    seven = min(seven,oneDot4(p, vec3(cubeSize/2 + cubeSize/4,-cubeSize - cubeRadius,(-cubeSize/2 - cubeSize/3)/2.6), vec3(1,0.0001,1)));
    seven = min(seven,oneDot4(p, vec3(-cubeSize/2 - cubeSize/4,-cubeSize - cubeRadius,(-cubeSize/2 - cubeSize/3)/2.6), vec3(1,0.0001,1)));

    seven = min(seven,oneDot4(p, vec3(0,-cubeSize - cubeRadius,0), vec3(1,0.0001,1)));

    seven = min(seven,oneDot4(p, vec3(cubeSize/2 + cubeSize/4,-cubeSize - cubeRadius,(cubeSize/2 + cubeSize/3)/2.6), vec3(1,0.0001,1)));
    seven = min(seven,oneDot4(p, vec3(-cubeSize/2 - cubeSize/5,-cubeSize - cubeRadius,(cubeSize/2 + cubeSize/3)/2.6), vec3(1,0.0001,1)));
    seven = min(seven,oneDot4(p, vec3(cubeSize/2 - cubeSize/2,-cubeSize - cubeRadius,cubeSize/2 + cubeSize/4), vec3(1,0.0001,1)));

    float dots = min(one,min(two,min(three, min(four, min(five, six))))); 
    return dots;
}

float dice4(vec3 p, float dots, out float res) {
    float cube = distRoundBox4(p, vec3(cubeSize),cubeRadius);
    float dots1 = addDots4(p);
    dots = min(dots,dots1);
    res = min(cube,dots);
    
    vec3 color = mix(vec3(0.2,0.2,0.8), vec3(0,0.35,0.3),p.y);
    vec3 buttonColor = mix(vec3(0.7,0.7,0.4), vec3(0,0.6,0.6),p.z);
    
    objColor4 = (res == dots) ?  vec4(buttonColor,1) : vec4(color,1);
    return min(dots, cube);
}

float dist4(vec3 p){
    p = rotate4(p, rot4);	
	float dots0 = 1e10;	
	float res = 1e10;
	// tresh
	if(distance(p, vec3(0,0,0)) > tresh){
		return 0;
	}
	vec3 p0 = rotate4(p, vec3(p.y/divide));
	// rep
	float dices = dice4(repeat4(p0, vec3(rep)), dots0, res);
	return dices;
}

vec3 getNormal4(vec3 p){
    float h = EPSILON;
    return normalize(vec3(
        dist4(p + vec3(h, 0, 0)) - dist4(p - vec3(h, 0, 0)),
        dist4(p + vec3(0, h, 0)) - dist4(p - vec3(0, h, 0)),
        dist4(p + vec3(0, 0, h)) - dist4(p - vec3(0, 0, h))));
}

vec4 raymarch4(vec3 rayOrigin, vec3 rayDir, out int steps){
    float totalDist = 0.0;
    for(int j=0; j<MAXSTEPS; j++)    {
        steps = j;
        vec3 p = rayOrigin + totalDist*rayDir;
        float d = dist4(p);
        if(abs(d)<EPSILON)
            return vec4(p, 1.0);        
        totalDist += d;
        if(totalDist>=MAXDEPTH) break;
    }
    return vec4(0);
}

float ambientOcclusion4(vec3 p, vec3 n){
    float res = 0.0;
    float fac = 1.0;
    for(float i=0.0; i<AOSAMPLES; i++){
        float distOut = i*0.3;  //go on normal ray AOSAMPLES times with factor 0.3
        res += fac * (distOut - dist4(p + n*distOut));   //look for every step, how far the nearest object is
        fac *= 0.5; //for every step taken on the normal ray, the fac decreases, so the shadow gets brighter
    }
    return 1.0 - clamp(res, 0.0, 1.0);
}

float shadow4(vec3 ro, vec3 rd){
    float res = 1.0;
    for( float t=0.01; t<32.0; ){
        float h = dist4(ro + rd*t);
        if( h<EPSILON )
            return AMBIENT;
        res = min( res, SOFTSDHADOWFAC*h/t );
        t += h;
    }
    return res;
}

vec4 getDiceUniverseColor(Camera cam, vec2 uv, vec3 cubeRotation){  
	// use uv to draw bg
	rot4 = cubeRotation;
    // marching and shading
	int steps = -1;
    vec4 res = raymarch4(cam.pos, cam.dir, steps); 
	vec4 color;
	if(res.a == 1){										
        color = objColor4;
        vec3 n = getNormal4(res.xyz);
        color *= max(AMBIENT, dot(n, lightDir4));
        //color *=shadow4(res.xyz,n);
        //color +=ambientOcclusion4(res.xyz,n) * AMBIENT;
	}   
	
	return color;
}