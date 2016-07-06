uniform vec2 iResolution;
uniform float iGlobalTime;

uniform float uScale;
uniform float uRepeatX;
uniform float uMoveX;
uniform float uRepeatY;
uniform float uMoveY;
uniform float uRepeatZ;
uniform float uMoveZ;

float WEIGHT = 3.0 / iResolution.x;
const vec3 RED = vec3(1.0,0.0,0.0);
const vec3 GREEN = vec3(0.0,1.0,0.0);
const vec3 BLUE = vec3(0.0,0.8,1.0);

// rasterize functions
float line(vec2 p, vec2 p0, vec2 p1, float w) 
{
    vec2 d = p1 - p0;
    float t = clamp(dot(d,p-p0) / dot(d,d), 0.0,1.0);
    vec2 proj = p0 + d * t;
    float dist = length(p - proj);
    dist = 1.0/dist*WEIGHT*w;
    return min(dist*dist,1.0);
}

// matrices
mat4 getRotMatrix(vec3 a) 
{
    vec3 s = 0.8*sin(a);
    vec3 c = cos(a);    
    mat4 ret;
    ret[0] = vec4(c.y*c.z,c.y*s.z,-s.y,0.0);
    ret[1] = vec4(s.x*s.y*c.z-c.x*s.z,s.x*s.y*s.z+c.x*c.z,s.x*c.y,0.0);
    ret[2] = vec4(c.x*s.y*c.z+s.x*s.z, c.x*s.y*s.z-s.x*c.z,   c.x*c.y,0.0);    
    ret[3] = vec4(0.0,0.0,0.0,1.0);
    return ret;
}

mat4 getPosMatrix(vec3 p) 
{   
    mat4 ret;
    ret[0] = vec4(1.0,0.0,0.0,p.x);
    ret[1] = vec4(0.0,1.0,0.0,p.y);
    ret[2] = vec4(0.0,0.0,1.0,p.z);   
    ret[3] = vec4(0.0,0.0,0.0,1.0);
    return ret;
}

// utils
vec3 mix3(vec3 a, vec3 b, vec3 c, float t) 
{
    if(t>0.5) return mix(b,c,t*2.0-1.0);
    else return mix(a,b,t*2.0);
}

vec3 fragment(vec3 p) 
{
    float t = sin(p.x*0.8+iGlobalTime*0.5)*0.5+0.5;
    float fog = min(pow(p.z,3.0)*400.0,1.0);
    return mix3(RED,GREEN,BLUE,t) * fog;
} 

float calcBox(vec2 uv, mat4 viewMat, vec3 move)
{
    float line_width = 0.9;

    // input assembly
        vec3 vert[8];
        vert[0] = (vec3(-1.0,-1.0, 1.0) + move) * uScale;
        vert[1] = (vec3(-1.0, 1.0, 1.0) + move) * uScale;    
        vert[2] = (vec3( 1.0, 1.0, 1.0) + move) * uScale;    
        vert[3] = (vec3( 1.0,-1.0, 1.0) + move) * uScale;
        vert[4] = (vec3(-1.0,-1.0,-1.0) + move) * uScale;
        vert[5] = (vec3(-1.0, 1.0,-1.0) + move) * uScale;    
        vert[6] = (vec3( 1.0, 1.0,-1.0) + move) * uScale;    
        vert[7] = (vec3( 1.0,-1.0,-1.0) + move) * uScale;

        for(int i = 0; i < 8; i++) {

            // transform
            vert[i] = (vec4(vert[i],1.0) * viewMat).xyz;

            // perspective
            vert[i].z = 1.0 / vert[i].z;
            vert[i].xy *= vert[i].z;
        }    

        // primitive assembly and rasterize
        float i;
        i  = line(uv,vert[0].xy,vert[1].xy,line_width);
        i += line(uv,vert[1].xy,vert[2].xy,line_width);
        i += line(uv,vert[2].xy,vert[3].xy,line_width);
        i += line(uv,vert[3].xy,vert[0].xy,line_width);
        i += line(uv,vert[4].xy,vert[5].xy,line_width);
        i += line(uv,vert[5].xy,vert[6].xy,line_width);
        i += line(uv,vert[6].xy,vert[7].xy,line_width);
        i += line(uv,vert[7].xy,vert[4].xy,line_width);
        i += line(uv,vert[0].xy,vert[4].xy,line_width);
        i += line(uv,vert[1].xy,vert[5].xy,line_width);
        i += line(uv,vert[2].xy,vert[6].xy,line_width);
        i += line(uv,vert[3].xy,vert[7].xy,line_width);

    return i;
}   

void main() 
{
	vec2 uv = gl_FragCoord.xy / iResolution.xy;
    uv = uv * 2.0 - 1.0;
    uv.x *= iResolution.x / iResolution.y;
    //uv = uv * (1.0 + pow(length(uv)*0.4,0.5)) * 0.6;
    float time = iGlobalTime * 0.31415;
    vec3 c = vec3(mix(vec3(0.19,0.13,0.1),vec3(1.0), 0.5*pow(length(uv)*0.5,2.0)));
    mat4 cam = getPosMatrix(vec3(0.0,0.0,10.0));
    

    mat4 rot = getRotMatrix(vec3(time,time*0.86,time*0.873));
    // rot = getRotMatrix(vec3(2.0, 1.0, 2.0));
    // rot = getRotMatrix(vec3(0));
    
    vec3 instance = vec3( -0.4, 0.2, 0);

    for(int y=0; y<uRepeatY; y++)
    {
        for(int z=0; z<uRepeatZ; z++)
        {
            for(int x=0; x<uRepeatX; x++)
            {
                int dirX = (x%2)==0 ? 1 : -1;
                int dirY = (y%2)==0 ? 1 : -1;
                int dirZ = (z%2)==0 ? 1 : -1;

                // vertex processing
                mat4 pos = getPosMatrix(instance * 5.0);
                mat4 mat = pos * rot * cam;

                float rect = calcBox(uv, mat, vec3( (x+0.5)*(uMoveX-0.8)*dirX, 
                                                    (y+0.5)*(uMoveY-0.8)*dirY, 
                                                    (z+0.5)*(uMoveZ-0.8)*dirZ) );    

                c += fragment(vec3(1.0)) * min(rect,1.0);
            }
        }
    }

    // fragment
	gl_FragColor = vec4(c,1.0);
}