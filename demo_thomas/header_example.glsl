#include "func_collections.h"

#define SPECULAR 20

const vec3 lightDir = normalize(vec3(0.8, 0.8, -1.0));

vec3 color = vec3(1);

/**
 * here go the calculation of the distance functions --> this method has to be called 'float dist(vec3 p)'
 * @param  p point
 * @return   distance to nearest object
 */
float dist(vec3 p)
{
	float cube = distRoundBox(p - vec3(sin(iGlobalTime*0.6),0.5,5.0), vec3(0.3), 0.15);
	float sphere = distSphere(p - vec3(sin(iGlobalTime*0.8),-0.5,5.3), 0.5);

	float dist = min(cube, sphere);

	color = (dist==cube) ? vec3(0.4, 0.5, 0.8) : vec3(0.8,0.5,0.4);
	return dist;
}

vec3 lighting(vec3 pos, vec3 rd, vec3 n)
{
	vec3 light = vec3(max(AMBIENT, dot(n, lightDir)));	//lambert light with light Color

	//specular
	vec3 reflection = normalize(reflect(lightDir, n));
	vec3 viewDirection = normalize(pos);
	float spec = max(AMBIENT, dot(reflection, viewDirection));

	light += pow(spec, SPECULAR);

	light *= shadow(pos, lightDir);
	light += ambientOcclusion(pos, n) * AMBIENT;
	return light;
}

void main()
{
	vec2 p = getScreenPos(60.0);

	Camera cam;
	cam.pos = vec3(0);
	cam.dir = normalize(vec3( p.x, p.y, 1 ));

	int steps = -1;

	vec4 res = raymarch(cam.pos, cam.dir, steps);
	vec3 currentCol = vec3(1);

	if(res.a==1.0)
	{
		currentCol = color;
		vec3 n = getNormal(res.xyz);

		currentCol *= lighting(res.xyz, cam.dir, n);
	}

	gl_FragColor = vec4(currentCol, 1.0);
}