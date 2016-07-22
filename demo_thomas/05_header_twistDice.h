#define RAD PI / 180.0
#define PI 3.14159
#define AMBIENT 0.1
#define EPSILON 0.001
#define MAXSTEPS 256
#define MAXDEPTH 60.0
#define AOSAMPLES 5.0
#define SOFTSDHADOWFAC 2.0

vec3 lightDir = normalize(vec3(0.5,0.5,-1));

uniform float cubeSize;
uniform float camPosZ;

float cubeRadius = cubeSize/4;
float dotSize = cubeRadius;

struct Camera
{
    vec3 pos;
    vec3 dir;
} ;

vec3 rotate5( vec3 p, vec3 r )
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

float distSphere(vec3 p, float rad){
	return length(p) - rad;
}

float distRoundBox(vec3 p, vec3 b, float r){
 	return length(max(abs(p)-b,0.0))-r;
}

float oneDot(vec3 p, vec3 offset, vec3 cutPlane){
    float cut = distRoundBox(p + offset, cutPlane,0.01);
    float sphere = distSphere(p + offset, dotSize);
    return max(cut,sphere);
}


float addDots(in vec3 p) {  
    float one = oneDot(p, vec3(0,cubeSize + cubeRadius,0), vec3(1,0.0001,1));

    float two = oneDot(p, vec3(-cubeSize - cubeRadius, cubeSize/2,cubeSize/2), vec3(0.0001,1,1));
    two = min(two,oneDot(p, vec3(-cubeSize - cubeRadius,-cubeSize/2,-cubeSize/2), vec3(0.0001,1,1)));

    float three = oneDot(p, vec3(0,0,cubeSize + cubeRadius), vec3(1,1,0.0001));
    three = min(three,oneDot(p, vec3(-cubeSize/2,-cubeSize/2,cubeSize + cubeRadius), vec3(1,1,0.0001)));
    three = min(three,oneDot(p, vec3(cubeSize/2,cubeSize/2,cubeSize + cubeRadius), vec3(1,1,0.0001)));

    float four = oneDot(p, vec3(-cubeSize/2,cubeSize/2,-cubeSize - cubeRadius), vec3(1,1,0.0001));
    four = min(four,oneDot(p, vec3(-cubeSize/2,-cubeSize/2,-cubeSize - cubeRadius), vec3(1,1,0.0001)));
    four = min(four,oneDot(p, vec3(cubeSize/2,cubeSize/2,-cubeSize - cubeRadius), vec3(1,1,0.0001)));
    four = min(four,oneDot(p, vec3(cubeSize/2,-cubeSize/2,-cubeSize - cubeRadius), vec3(1,1,0.0001)));

    float five = oneDot(p, vec3(cubeSize + cubeRadius,0.,0.), vec3(0.0001,1,1));
    five = min(five,oneDot(p, vec3(cubeSize + cubeRadius,-cubeSize/2,-cubeSize/2), vec3(0.0001,1,1)));
    five = min(five,oneDot(p, vec3(cubeSize + cubeRadius,cubeSize/2,cubeSize/2), vec3(0.0001,1,1)));
    five = min(five,oneDot(p, vec3(cubeSize + cubeRadius,-cubeSize/2,cubeSize/2), vec3(0.0001,1,1)));
    five = min(five,oneDot(p, vec3(cubeSize + cubeRadius,cubeSize/2,-cubeSize/2), vec3(0.0001,1,1)));

    float six = oneDot(p, vec3(cubeSize/2 - cubeSize/10,-cubeSize - cubeRadius,cubeSize/2 + cubeSize/10), vec3(1,0.0001,1));
    six = min(six,oneDot(p, vec3(-cubeSize/2 - cubeSize/10,-cubeSize - cubeRadius,cubeSize/2 + cubeSize/10), vec3(1,0.0001,1)));

    six = min(six,oneDot(p, vec3(-cubeSize/2 - cubeSize/10,-cubeSize - cubeRadius,-0.), vec3(1,0.0001,1)));
    six = min(six,oneDot(p, vec3(cubeSize/2 - cubeSize/10,-cubeSize - cubeRadius,-0.), vec3(1,0.0001,1)));

    six = min(six,oneDot(p, vec3(-cubeSize/2 - cubeSize/10,-cubeSize - cubeRadius,-cubeSize/2 - cubeSize/10), vec3(1,0.0001,1)));
    six = min(six,oneDot(p, vec3(cubeSize/2 - cubeSize/10,-cubeSize - cubeRadius,-cubeSize/2 - cubeSize/10), vec3(1,0.0001,1)));

    float seven = oneDot(p, vec3(cubeSize/2 - cubeSize/2,-cubeSize - cubeRadius,-cubeSize/2 - cubeSize/4), vec3(1,0.0001,1));
    seven = min(seven,oneDot(p, vec3(cubeSize/2 + cubeSize/4,-cubeSize - cubeRadius,(-cubeSize/2 - cubeSize/3)/2.6), vec3(1,0.0001,1)));
    seven = min(seven,oneDot(p, vec3(-cubeSize/2 - cubeSize/4,-cubeSize - cubeRadius,(-cubeSize/2 - cubeSize/3)/2.6), vec3(1,0.0001,1)));

    seven = min(seven,oneDot(p, vec3(0,-cubeSize - cubeRadius,0), vec3(1,0.0001,1)));

    seven = min(seven,oneDot(p, vec3(cubeSize/2 + cubeSize/4,-cubeSize - cubeRadius,(cubeSize/2 + cubeSize/3)/2.6), vec3(1,0.0001,1)));
    seven = min(seven,oneDot(p, vec3(-cubeSize/2 - cubeSize/5,-cubeSize - cubeRadius,(cubeSize/2 + cubeSize/3)/2.6), vec3(1,0.0001,1)));
    seven = min(seven,oneDot(p, vec3(cubeSize/2 - cubeSize/2,-cubeSize - cubeRadius,cubeSize/2 + cubeSize/4), vec3(1,0.0001,1)));


    float dots = min(one,min(two,min(three, min(four, min(five, six))))); 
//  objColor = (res == dots) ?  vec4(0) : vec4(1);
    return dots;
}

float dice(vec3 p, float dots, out float res) {
	float cube = distRoundBox(p, vec3(cubeSize),cubeRadius);
	float dots1 = addDots(p);
	dots = min(dots,dots1);
	res = min(cube,dots);
	
	vec3 color = mix(vec3(0.2,0.2,0.8), vec3(0,0.35,0.3),p.y);
	vec3 buttonColor = mix(vec3(0.7,0.7,0.4), vec3(0,0.6,0.6),p.z);
	
	objColor = (res == dots) ?  vec4(buttonColor,1) : vec4(color,1);
	return min(dots, cube);
}

float chain(vec3 p){
	p.y += iGlobalTime * 2;	
	vec3 pt = p;
	pt.x += (uv.x * 2);
    pt = rotate(pt, vec3(90,0,0));
	pt.y += sin(iGlobalTime)/3;
    float t2 = distTorus(repeat(pt + vec3(0,0,0), vec3(0,0,3)),vec2(1,0.2));
	vec3 pt2 = p;
	pt2.x += uv.x * 2;
    pt2 = rotate(pt2, vec3(90,45,0));
	pt2.y += sin(iGlobalTime)/3;
    float t3 = distTorus(repeat(pt2 - vec3(0,0,1.5), vec3(0,0,3)),vec2(1,0.2));	
    return min(t2,t3);
}

float dist5(vec3 p) { 
    float rot = iGlobalTime * 50;
    float dots0 = 1000; 
	float res = 1000;
    vec3 p0 = rotate5(p+vec3(0,2,0), vec3(0,rot,0));
    p0 = rotate5(p0, vec3(0,rot * p.y/spinCube,0));
    p0 = rotate5(p0, vec3(45, 0, 45));
    float dices = dice(p0, dots0, res);
    //float torus = distTorus(pt, vec2(3,1));

    vec3 pc = p + vec3(0,2,0);
    pc = rotate5(pc, vec3(90,0,0));
    float ch = chain(pc+vec3(-1,0,0));
    res = min(ch, min(5000,dices));
    vec3 color = mix(vec3(0.2,0.2,0.8), vec3(0,0.35,0.3),p.y);
    vec3 buttonColor = mix(vec3(0.7,0.7,0.4), vec3(0,0.6,0.6),p0.z);    
    objColor = ( res == ch) ?  vec4(0.3) : objColor;    

	return p.z < 3  ? ch : 5000;
}


vec3 getNormal5(vec3 p)
{
    float h = EPSILON;
    return normalize(vec3(
        dist5(p + vec3(h, 0, 0)) - dist5(p - vec3(h, 0, 0)),
        dist5(p + vec3(0, h, 0)) - dist5(p - vec3(0, h, 0)),
        dist5(p + vec3(0, 0, h)) - dist5(p - vec3(0, 0, h))));
}

float calcLight(vec3 point){
    vec3 normal = getNormal1(point);  
    return max(AMBIENT, dot(normal, lightDir));
}

vec4 raymarch5(vec3 rayOrigin, vec3 rayDir, out int steps)
{
    float totalDist = 0.0;
    for(int j=0; j<MAXSTEPS; j++)
    {
        steps = j;
        vec3 p = rayOrigin + totalDist*rayDir;
        float d = dist5(p);
        if(abs(d)<EPSILON)  //if it is near the surface, return an intersection
        {
            return vec4(p, 1.0);
        }
        totalDist += d;
        if(totalDist>=MAXDEPTH) break;
    }
    return vec4(0);
}

float ambientOcclusion5(vec3 p, vec3 n)
{
    float res = 0.0;
    float fac = 1.0;
    for(float i=0.0; i<AOSAMPLES; i++)
    {
        float distOut = i*0.3;  //go on normal ray AOSAMPLES times with factor 0.3
        res += fac * (distOut - dist5(p + n*distOut));   //look for every step, how far the nearest object is
        fac *= 0.5; //for every step taken on the normal ray, the fac decreases, so the shadow gets brighter
    }
    return 1.0 - clamp(res, 0.0, 1.0);
}

float shadow5(vec3 ro, vec3 rd)
{
    float res = 1.0;
    for( float t=0.01; t<32.0; )
    {
        float h = dist5(ro + rd*t);
        if( h<EPSILON )
            return AMBIENT;
        res = min( res, SOFTSDHADOWFAC*h/t );
        t += h;
    }
    return res;
}

vec4 getTwistDiceColor(Camera cam){  
	// use uv to draw bg

    float point = length(uv-0.5);    
    vec3 bg = mix(vec3(0.78,0.32,0.78),vec3(0.2,0.8,0.35), point);    

    // marching and shading
	int steps = -1;
    vec4 res = raymarch5(cam.pos, cam.dir, steps);  


	vec3 color = bg;
	if(res.a == 1){										// not sure why this is needed ...
		color = mix(vec3(0.2,0.2,0.8), vec3(0,0.35,0.3), (res.y + EPSILON));
		vec3 diffuse =  vec3(calcLight(res.xyz));	
		diffuse *= shadow5(res.xyz, lightDir);
		diffuse += ambientOcclusion5(res.xyz, getNormal1(res.xyz)) * AMBIENT;
		color *= diffuse;
	}   
	
	return vec4(color,1);
}

