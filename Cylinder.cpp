/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The cylinder class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cylinder.h"
#include <math.h>

/**
* Cylinder's intersection method.  The input is a ray. 
*/
float Cylinder::intersect(glm::vec3 p0, glm::vec3 dir)
{
	glm::vec3 vdif = p0 - center;   //Vector s (see Slide 28)

	float a = pow(dir.x, 2) + pow(dir.z, 2);
	float b = 2 * ((dir.x * vdif.x) + (dir.z * vdif.z));
	float c = pow(vdif.x, 2) + pow(vdif.z, 2) - pow(radius, 2);

	float delta = (b * b - 4.0 * a * c);

	if (fabs(delta) < 0.001)
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
	float y1 = p0.y + dir.y * min;
	float y2 = p0.y + dir.y * max;
	
	if ((y1 >= center.y) && (y1 <= center.y + height)) 
	{
		return t1;
	}

	if ((y2 >= center.y) && (y2 <= center.y + height)) 
	{
		return t2;
	}

	return -1.0;
	
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the cone.
*/
glm::vec3 Cylinder::normal(glm::vec3 p)
{
	glm::vec3 n(p.x - center.x, 0, p.z - center.z);
	n = glm::normalize(n);
	return n;
}