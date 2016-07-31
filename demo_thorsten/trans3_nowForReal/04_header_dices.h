#define STEPS 32
#define PRECISION 0.01
#define DEPTH 10.0


vec2 resolution;
float globalTimeTestShader;

vec3 eye = vec3(0,0,-2.5);
vec2 uv; bool hit = false;


float cubeSize = 0.125;
float cubeRadius = cubeSize/4;
float dotSize = cubeRadius;

vec4 objColor = vec4(0);

vec3 repeat(vec3 p, vec3 c ){
    return mod(p, c) - 0.5 * c;
}

float distSphere(vec3 p, float rad);

float distRoundBox(vec3 p, vec3 b, float r);

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

float scene(vec3 p){
    float dots = 1000;
    float res = 1000;
    float dices = dice(repeat(p, vec3(0.5)), dots, res);
    return dices;
}
vec3 march(vec3 ro,vec3 rd){float t=0.0,d;hit=false;for(int i=0;i<STEPS;i++){d=scene(ro+rd*t);if(d<PRECISION){hit=true;break;}if(t>DEPTH){break;}t+=d;}return(ro+rd*t);}
vec3 lookAt(vec3 o,vec3 t,vec2 p){vec2 uv=(2.0*p-resolution.xy)/resolution.xx;vec3 d=normalize(t-o),u=vec3(0,1,0),r=cross(u,d);return(normalize(r*uv.x+cross(d,r)*uv.y+d));}


vec3 getShader2Color(vec2 fragCoord, vec2 resol, float globTime, vec3 camPos){   
    resolution = resol;
    globalTimeTestShader = globTime;

    uv = (2.0 * fragCoord.xy - resolution.xy) / resolution.xx;

    eye += camPos;
    
    vec3 p = march(eye,lookAt(eye,vec3(0), fragCoord));
    
    vec3 col = objColor.xyz;
    
    if (hit == false) { col = vec3(0.0, 0.0, 0.0); }

    return col;
}