/*==================================================================================
* COSC 363  Computer Graphics (2022)
* Department of Computer Science and Software Engineering, University of Canterbury.
*
* A basic ray tracer
* See Lab06.pdf   for details.
*===================================================================================
*/

#define _USE_MATH_DEFINES

#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "Cone.h"
#include "Cylinder.h"
#include "SceneObject.h"
#include "Ray.h"
#include "Plane.h"
#include "TextureBMP.h"
#include <GL/freeglut.h>


using namespace std;

TextureBMP texture;

const float EDIST = 40.0;
const int NUMDIV = 500;
const int MAX_STEPS = 5;
const float XMIN = -10.0;
const float XMAX = 10.0;
const float YMIN = -10.0;
const float YMAX = 10.0;

vector<SceneObject*> sceneObjects;


//---The most important function in a ray tracer! ---------------------------------- 
//   Computes the colour value obtained by tracing a ray and finding its 
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
	glm::vec3 backgroundCol(0);						//Background colour = (0,0,0)
	glm::vec3 lightPos(40, 40, -3);					//Light's position
	glm::vec3 lightPos2(-40, 40, -40);
	glm::vec3 color(0);
	glm::vec3 color2(0);
	SceneObject* obj;

    ray.closestPt(sceneObjects);					//Compare the ray with all objects in the scene
    if(ray.index == -1) return backgroundCol;		//no intersection
	obj = sceneObjects[ray.index];					//object on which the closest point of intersection is found

	if (ray.index == 3)
	{
		//chequred pattern
		int stripeWidth = 8;
		int iz = (ray.hit.z) / stripeWidth;
		int k = iz % 2;   // Two colours
		if (k == 0)
		{
			int stripeLength = 4;
			int ix = (ray.hit.x) / stripeLength;
			int m = ix % 2;
			if (m == 0)
			{
				color = glm::vec3(0, 1, 1);
			}
			else
			{
				color = glm::vec3(0, 0.5, 1);
			}
		}
		else
		{
			int stripeLength = 4;
			int ix = (ray.hit.x) / stripeLength;
			int m = ix % 2;
			if (m == 0)
			{
				color = glm::vec3(0, 0.5, 1);
			}
			else
			{
				color = glm::vec3(0, 1, 1);
			}
		}
		obj->setColor(color);
	}
	
	if (ray.index == 4)
	{
		//chequred pattern
		int stripeWidth = 8;
		int iz = (ray.hit.z) / stripeWidth;
		int k = iz % 2;   // Two colours
		if (k == 0)
		{
			int stripeLength = 4;
			int ix = (ray.hit.x) / stripeLength;
			int m = ix % 2;
			if (m == 0)
			{
				color = glm::vec3(0, 0.5, 1);
			}
			else
			{
				color = glm::vec3(0, 1, 1);
			}
		}
		else
		{
			int stripeLength = 4;
			int ix = (ray.hit.x) / stripeLength;
			int m = ix % 2;
			if (m == 0)
			{
				color = glm::vec3(0, 1, 1);
			}
			else
			{
				color = glm::vec3(0, 0.5, 1);
			}
		}
		obj->setColor(color);
	}

	if (ray.index == 10)
	{
		glm::vec3 spherecoords(0.0, -11.0, -80.0);
		glm::vec3 n = glm::normalize(ray.hit - spherecoords);
		float texcoords = (0.5 + (atan2(n.x, n.z) / (2 * M_PI)));
		float texcoordt = (0.5 - (asin(n.y) / M_PI));

		if (texcoords > 0 && texcoords < 1 && texcoordt > 0 && texcoordt < 1)
		{
			color = texture.getColorAt(texcoords, texcoordt);
			obj->setColor(color);
		}
	} 


	color = obj->multilighting(lightPos, lightPos2, -ray.dir, ray.hit);		//Object's colour

	glm::vec3 lightVec = lightPos - ray.hit;
	glm::vec3 lightVec2 = lightPos2 - ray.hit;
	Ray shadowRay(ray.hit, lightVec);
	Ray shadowRay2(ray.hit, lightVec2);
	shadowRay.closestPt(sceneObjects);
	shadowRay2.closestPt(sceneObjects);

	float lightDist = glm::length(lightVec);
	float lightDist2 = glm::length(lightVec2);

	if (shadowRay.index >= 0 and shadowRay.dist < lightDist)
	{
		if (shadowRay.index == 1)
		{
			color = 0.5f * obj->getColor();
		}
		else if (shadowRay.index == 2)
		{
			color = 0.6f * obj->getColor();
		}
		else
		{
			color = 0.2f * obj->getColor();
		} 
	}

	if (shadowRay2.index >= 0 and shadowRay2.dist < lightDist2)
	{
		if (shadowRay2.index == 1)
		{
			color = 0.5f * obj->getColor();
		}
		else if (shadowRay2.index == 2)
		{
			color = 0.6f * obj->getColor();
		}
		else
		{
			color = 0.2f * obj->getColor();
		}
	}

	if (obj->isReflective() && step < MAX_STEPS)
	{
		float rho = obj->getReflectionCoeff();
		glm::vec3 normalVec = obj->normal(ray.hit);
		glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVec);
		Ray reflectedRay(ray.hit, reflectedDir);
		glm::vec3 reflectedColor = trace(reflectedRay, step + 1);
		color = color + (rho * reflectedColor);
		color2 = color2 + (rho * reflectedColor);
	}

	if (obj->isRefractive() && step < MAX_STEPS)
	{
		float rhi = obj->getRefractionCoeff();
		float ri = obj->getRefractiveIndex();
		glm::vec3 normalVec = obj->normal(ray.hit);
		glm::vec3 refractDir = glm::refract(ray.dir, normalVec, 1.0f/ri);
		Ray refrRay(ray.hit, refractDir);
		refrRay.closestPt(sceneObjects);
		glm::vec3 normalVec2 = obj->normal(refrRay.hit);
		glm::vec3 refractDir2 = glm::refract(refractDir, -normalVec2, ri);

		Ray refrRay2(refrRay.hit, refractDir2);
		refrRay2.closestPt(sceneObjects);
		glm::vec3 refractedColor = trace(refrRay2, step + 1);
		color = color * 0.2f + refractedColor;
		color2 = color2 * 0.2f + refractedColor;
	}

	if (obj->isTransparent() && step < MAX_STEPS)
	{
		float tra = obj->getTransparencyCoeff();
		Ray transRay(ray.hit, ray.dir);
		glm::vec3 transColor = trace(transRay, 1);
		color = color * 0.3f + transColor;
	}

	return color;
}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
	float xp, yp;  //grid point
	float cellX = (XMAX - XMIN) / NUMDIV;  //cell width
	float cellY = (YMAX - YMIN) / NUMDIV;  //cell height
	glm::vec3 eye(0., 0., 0.);

	glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glBegin(GL_QUADS);  //Each cell is a tiny quad.

	for (int i = 0; i < NUMDIV; i++)	//Scan every cell of the image plane
	{
		xp = XMIN + i * cellX;
		for (int j = 0; j < NUMDIV; j++)
		{
			yp = YMIN + j * cellY;

			glm::vec3 dir(xp + 0.5 * cellX, yp + 0.5 * cellY, -EDIST);	//direction of the primary ray

			Ray ray = Ray(eye, dir);

			glm::vec3 col = trace(ray, 1); //Trace the primary ray and get the colour value
			glColor3f(col.r, col.g, col.b);
			glVertex2f(xp, yp);				//Draw each cell with its color value
			glVertex2f(xp + cellX, yp);
			glVertex2f(xp + cellX, yp + cellY);
			glVertex2f(xp, yp + cellY);
		}
	}

    glEnd();
    glFlush();
}



//---This function initializes the scene ------------------------------------------- 
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL 2D orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);

    glClearColor(0, 0, 0, 1);

	Sphere *sphere1 = new Sphere(glm::vec3(0.0, -7.0, -100.0), 7.0);
	sphere1->setReflectivity(true, 0.8);
	sphere1->setColor(glm::vec3(1, 0, 0));   //Set colour to blue
//	sphere1->setSpecularity(false);
//	sphere1->setShininess(50);
	sceneObjects.push_back(sphere1);		 //Add sphere to scene objects

	Sphere *sphere2 = new Sphere(glm::vec3(-13.0, -9.0, -85.0), 5.0);
	sphere2->setReflectivity(true, 0.8);
//	sphere2->setTransparency(true, 0.8);
	sphere2->setRefractivity(true, 0.0, 1.02);
//	sphere2->setColor(glm::vec3(0, 1, 0));
//	sphere2->setSpecularity(false);
//	sphere2->setShininess(5);
	sceneObjects.push_back(sphere2);

	Sphere* sphere3 = new Sphere(glm::vec3(13.0, -9.0, -85.0), 5.0);
	sphere3->setReflectivity(true, 0.2);
	sphere3->setTransparency(true, 0.8);
	sphere3->setColor(glm::vec3(0, 1, 0));
//	sphere3->setSpecularity(false);
//	sphere3->setShininess(5);
	sceneObjects.push_back(sphere3);

	Plane* leftplane = new Plane(glm::vec3(-40.0, -15.0, -40.0),
		glm::vec3(0.0, -15.0, -40.0),
		glm::vec3(0.0, -15.0, -200),
		glm::vec3(-40.0, -15.0, -200));
	leftplane->setColor(glm::vec3(0.8, 0.8, 0));
	leftplane->setSpecularity(false);
	sceneObjects.push_back(leftplane);

	Plane* rightplane = new Plane(glm::vec3(0.0, -15.0, -40.0),
		glm::vec3(40.0, -15.0, -40.0),
		glm::vec3(40.0, -15.0, -200),
		glm::vec3(0.0, -15.0, -200));
	rightplane->setColor(glm::vec3(0.8, 0.8, 0));
	rightplane->setSpecularity(false);
	sceneObjects.push_back(rightplane);

	Plane* pyrafront = new Plane(glm::vec3(-10.0, -15.0, -70.0),
		glm::vec3(-5.0, -15.0, -70.0),
		glm::vec3(-7.5, -5.0, -75.0));
	pyrafront->setReflectivity(true, 0.8);
	pyrafront->setColor(glm::vec3(1, 1, 0.3));
//	pyrafront->setSpecularity(false);
	sceneObjects.push_back(pyrafront);

	Plane* pyraleft = new Plane(glm::vec3(-10.0, -15.0, -80.0),
		glm::vec3(-10.0, -15.0, -70.0),
		glm::vec3(-7.5, -5.0, -75.0));
	pyraleft->setReflectivity(true, 0.8);
	pyraleft->setColor(glm::vec3(1, 1, 0.3));
//	pyraleft->setSpecularity(false);
	sceneObjects.push_back(pyraleft);

	Plane* pyraright = new Plane(glm::vec3(-5.0, -15.0, -70.0),
		glm::vec3(-5.0, -15.0, -80.0),
		glm::vec3(-7.5, -5.0, -75.0));
	pyraright->setReflectivity(true, 0.8);
	pyraright->setColor(glm::vec3(1, 1, 0.3));
//	pyraright->setSpecularity(false);
	sceneObjects.push_back(pyraright);

	Plane* pyraback = new Plane(glm::vec3(-5.0, -15.0, -80.0),
		glm::vec3(-10.0, -15.0, -80.0),
		glm::vec3(-7.5, -5.0, -75.0));
	pyraback->setReflectivity(true, 0.8);
	pyraback->setColor(glm::vec3(1, 1, 0.3));
//	pyraback->setSpecularity(false);
	sceneObjects.push_back(pyraback);

	Cone* cone1 = new Cone(glm::vec3(8.0, -15.0, -74.0), 3.5, 10.0);
	cone1->setReflectivity(true, 0.8);
	cone1->setColor(glm::vec3(1, 1, 0.3));   //Set colour to yellow
	sceneObjects.push_back(cone1);

	Sphere* sphere4 = new Sphere(glm::vec3(0.0, -11.0, -80.0), 4.0);
//	sphere4->setReflectivity(true, 0.2);
//	sphere4->setTransparency(true, 0.8);
	sphere4->setColor(glm::vec3(1, 1, 1));
	sceneObjects.push_back(sphere4);

	Cylinder* cylinder1 = new Cylinder(glm::vec3(-14.0, -15.0, -70.0), 2.0, 6.0);
	cylinder1->setReflectivity(true, 0.5);
	cylinder1->setColor(glm::vec3(0.5, 0.0, 1.0));
	sceneObjects.push_back(cylinder1);

	Cylinder* cylinder2 = new Cylinder(glm::vec3(14.0, -15.0, -70.0), 2.0, 6.0);
	cylinder2->setReflectivity(true, 0.5);
	cylinder2->setColor(glm::vec3(0.5, 0.0, 1.0));
	sceneObjects.push_back(cylinder2);

	texture = TextureBMP("Butterfly.bmp");

}


int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracing");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
