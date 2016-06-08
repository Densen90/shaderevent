#include "func_collections.h"

vec3 lightDir = vec3(0.5,0.5,-1);

vec4 objColor = vec4(0);

uniform float spinCube;

uniform float cubeSize;
float cubeRadius = cubeSize/4;
float dotSize = cubeRadius;


float oneDot(vec3 p, vec3 offset, vec3 cutPlane){
	float cut = distRoundBox(p + offset, cutPlane,0.01);
	float sphere = distSphere(p + offset, dotSize);
	return max(cut,sphere);
}

vec3 repeat(vec3 p, vec3 c ){
    return mod(p, c) - 0.5 * c;
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
//	objColor = (res == dots) ?  vec4(0) : vec4(1);
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

float dist(vec3 p){
	float plane = distPlane(p + vec3(0,4,0), vec3(0,1,0));
	float rot = iGlobalTime * 50;
	float dots0 = 10000;	
	float res = 1000;

	vec3 p0 = rotate(p, vec3(0,rot * p.y/spinCube,0));	
	p0 = rotate(p0, vec3(45,0,45));
	
	
	float dice = dice(p0, dots0, res);
	res = min(plane,dice);
	objColor = (res == plane) ? vec4(0.1) : objColor;			
	return res;
}


void main(){
	vec2 p = getScreenPos(90);
	Camera cam;
	cam.pos = vec3(0,0,-10);
	cam.dir = normalize(vec3(p.x, p.y, 1));
	int steps = -1;
	vec4 res = raymarch(cam.pos, cam.dir, steps);
	vec4 color = vec4(0.1,0.2,0.1,1);
	if(res.a ==1){
		color = objColor;
		vec3 n = getNormal(res.xyz);
		color *= max(AMBIENT, dot(n, lightDir));
		//color *=shadow(res.xyz,n);
		color +=ambientOcclusion(res.xyz,n) * AMBIENT;
		color *=1;
	}
	gl_FragColor = color;

}