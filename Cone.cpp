/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The cone class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cone.h"
#include <math.h>

/**
* Cone's intersection method.  The input is a ray. 
*/
float Cone::intersect(glm::vec3 p0, glm::vec3 dir)
{
    glm::vec3 vdif = p0 - center;   //Vector s (see Slide 28)
	
	float ydif = height - p0.y + center.y;
	float m = pow((radius/height), 2);
	
	float a = pow(dir.x, 2) + pow(dir.z, 2) - (m * pow(dir.y, 2));
	float b = 2 * ((vdif.x * dir.x) + (vdif.z * dir.z) + (m * ydif * dir.y));
	float c = pow(vdif.x, 2) + pow(vdif.z, 2) - m * (pow(ydif, 2));
	
	float delta = (b * b - 4.0 * a * c);
   
	if(fabs(delta) < 0.001) 
	{
		return -1.0;    //includes zero and negative values
	}
	
    float t1 = (-b - sqrt(delta)) / (2 * a);
    float t2 = (-b + sqrt(delta)) / (2 * a);
	float max = t1;
	float min = t2;

	if (t1 < t2)
	{
		min = t1;
		max = t2;
	}
	float y1 = p0.y + min * dir.y - center.y;
	float y2 = p0.y + max * dir.y - center.y;
	if (!(y1 < 0 || y1 > height) && min != -1.0)
	{
		return min;
	}
	else if (!(y2 < 0 || y2 > height) && max != -1.0)
	{
		return max;
	}
	return -1.0;
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the cone.
*/
glm::vec3 Cone::normal(glm::vec3 p)
{
	float alpha = atan((p.x - center.x) / (p.z - center.z));
	float theta = atan(height / radius);
	glm::vec3 n(sin(alpha) * cos(theta), sin(theta), cos(alpha) * cos(theta));
    n = glm::normalize(n);
    return n;
}
