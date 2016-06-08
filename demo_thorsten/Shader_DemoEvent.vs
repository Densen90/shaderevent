#include "func_collections.h"

uniform float iGlobalTime;

//Camera Position
uniform float cameraX = 10.0;
uniform float cameraY = 25.0;
uniform float cameraZ = 10.0;

//Camera Rotation
uniform float cameraAngleX = 90.0;
uniform float cameraAngleY = 90.0;
uniform float cameraAngleZ = 90.0;

const float toRadian = PI/180.0;

vec3 pointLight = vec3(0.0, 5.0, -5.0);


vec3 Repeat(vec3 P, vec3 b)
{
	return mod(P, b) - 0.5 * b;
}

float dist(vec3 p)
{
	float boxes = Box(Repeat(p, vec3(5, 5, 10)), vec3(0.5, 0.5, 0.5));
	float boxes1 = Box(Repeat(p, vec3(2.5, 2.5, 5)), vec3(0.5, 0.5, 0.5));
	float temp = Difference(boxes, boxes1)  * iGlobalTime * 0.1;

	return abs(sin(temp));
}

void main()
{
	vec2 p = getScreenPos(60);

	Camera cam;
	cam.pos = vec3(0);
	cam.dir = normalize(vec3(p.x, p.y, 1.0));

	int steps = -1;

	vec4 res = raymarch(cam.pos, cam.dir, steps);
	vec3 currentCol = vec3(1);

	if(res.a == 1.0)
	{
		currentCol = color;
		vec3 n = getNormal(res.xyz);

		currentCol *= lighting(res.xyz, cam.dir, n);
	}

	gl_FragColor = vec4(currentCol, 1.0);
}