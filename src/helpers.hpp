/*
 * Helpers.hpp
 *
 *  Created on: Jan 23, 2013
 *      Author: Travis Lewis
 */

#ifndef HELPERS_HPP_
#define HELPERS_HPP_

#include <vector>
#include <iostream>
#include <string>

using namespace std;

#include "vec.h"
#include "math.h"

#include "Thing.hpp"

#define PI 3.1415962

struct Point2D
{
	int x, y;
};

struct Point3D
{
	int x, y;
	float z;
};

void applyMatrix(Thing* t, mat4* m)
{
	for(std::vector<vec4>::iterator it = t->points.begin(),
			end = t->points.end() ; it != end ; ++it)
	{
		*it = (*m) * (*it);
	}
}

Thing createLine(std::vector<string>* params)
{
	Thing line;
	line.type = Thing::LINE;

	std::vector<string>::iterator it = params->begin();
	for(int i = 0 ; i < 2 ; i++)
	{
		vec4 p;
		p.x = atof((it++)->c_str());
		p.y = atof((it++)->c_str());
		p.z = atof((it++)->c_str());
		p.w = 1.0;
		line.points.push_back(p);
	}

	return line;
}

Thing createTriangle(std::vector<string>* params)
{
	Thing tri;
	tri.type = Thing::TRIANGLE;

	std::vector<string>::iterator it = params->begin();
	for(int i = 0 ; i < 3 ; i++)
	{
		vec4 p;
		p.x = atof((it++)->c_str());
		p.y = atof((it++)->c_str());
		p.z = atof((it++)->c_str());
		p.w = 1.0;
		tri.points.push_back(p);
	}
	return tri;
}

Thing createUnitCube()
{
	Thing cube;
	cube.type = Thing::WIRE_CUBE;
	cube.points.push_back(vec4(-0.5, -0.5, -0.5, 1));
	cube.points.push_back(vec4(-0.5, -0.5, 0.5, 1));
	cube.points.push_back(vec4(-0.5, 0.5, -0.5, 1));
	cube.points.push_back(vec4(-0.5, 0.5, 0.5, 1));

	cube.points.push_back(vec4(0.5, -0.5, -0.5, 1));
	cube.points.push_back(vec4(0.5, -0.5, 0.5, 1));
	cube.points.push_back(vec4(0.5, 0.5, -0.5, 1));
	cube.points.push_back(vec4(0.5, 0.5, 0.5, 1));

	//which vectors pairs to draw as lines:
	// (0,1), (0,2), (0,4), (1,3), (1,5), (2,3), (2,6), (3,7), (4,5),
	// (4,6), (5,7), (6,7)
	//^those are the indices of the points array to use.

	return cube;
}

Thing createSolidCube()
{
	Thing cube;
	cube.type = Thing::SOLID_CUBE;

	cube.points.push_back(vec4(-0.5, 0.5, -0.5, 1));
	cube.points.push_back(vec4(0.5, 0.5, -0.5, 1));
	cube.points.push_back(vec4(0.5, 0.5, 0.5, 1));
	cube.points.push_back(vec4(-0.5, 0.5, 0.5, 1));

	cube.points.push_back(vec4(-0.5, -0.5, -0.5, 1));
	cube.points.push_back(vec4(0.5, -0.5, -0.5, 1));
	cube.points.push_back(vec4(0.5, -0.5, 0.5, 1));
	cube.points.push_back(vec4(-0.5, -0.5, 0.5, 1));

	return cube;

	//point sets that make up the triangles:
	//top: (0, 1, 2), (0, 3, 2)
	//bottom: (4, 5, 6), (4, 7, 6)
	//left: (0, 4, 7), (0, 3, 7)
	//right: (1, 5, 6), (1, 2, 6)
	//front: (2, 3, 7), (2, 6, 7)
	//back: (0, 1, 4), (1, 4, 5)
}

Thing createUnitCylinder(int n)
{
	float theta = (2*PI) / n;

	Thing t;
	t.type = Thing::CYLINDER;

//	float pythag = sqrt((0.5*0.5) + (0.5*0.5));

	/*the commented out lines that use pythag make a cylinder that's quite a
	bit wider than the unit cube. The uncommented lines work fairly well,
	the resulting cylinder is a little short in the x axis, but still
	better than the pythag one. */

	for(int i = 0 ; i < n ; i++)
	{
		float x = 0.5 * cos((float)i * theta + (theta/2.0));
		float z = 0.5 * sin((float)i * theta + (theta/2.0));
//		float x = pythag * cos((float)i * theta);
//		float z = pythag * sin((float)i * theta);
		vec4 v(x,0.5,z,1.0);
		t.points.push_back(v);
	}

	for(int i = 0 ; i < n ; i++)
	{
		float x = 0.5 * cos((float)i * theta + (theta/2.0));
		float z = 0.5 * sin((float)i * theta + (theta/2.0));
//		float x = pythag * cos((float)i * theta);
//		float z = pythag * sin((float)i * theta);
		vec4 v(x,-0.5,z,1.0);
		t.points.push_back(v);
	}
	return t;
}

Thing createUnitCone(int n) //n = number of points that make up the base
{
	Thing t;
	t.type = Thing::CONE;

	t.points.push_back(vec4(0.0, 0.5, 0.0, 1.0)); //top of cone

	float theta = (2*PI)/(float)n;

	for(int i = 0 ; i < n ; i++)
	{
		float x = 0.5 * cos((float)i * theta + (theta/2.0));
		float z = 0.5 * sin((float)i * theta + (theta/2.0));
		vec4 v(x, -0.5, z, 1.0);
		t.points.push_back(v);
	}
	return t;
}

std::vector<Point2D> getPointsFromLine2D(int x1, int y1, int x2, int y2)
{
	std::vector<Point2D> points;

	//make sure x1 < x2
	if(x1 > x2)
	{
		int temp = x1;
		x1 = x2;
		x2 = temp;
		temp = y1;
		y1 = y2;
		y2 = temp;
	}

	//get slope/distance information
	int dx = x2 - x1;
	int dy = y2 - y1;
	float m = ((float)dy) / ((float)dx);

	if(dx == 0 && dx != 0)
	{ //vertical line
		int inc = (y1 < y2) ? 1 : -1;
		for(int i = y1 ; i != y2 ; i += inc)
		{
			Point2D p;
			p.x = x1;
			p.y = i;
			points.push_back(p);
		}
	}
	else if(dy == 0 && dx != 0)
	{ // horizontal line
		//x1 < x2 so we don't need to check that
		for(int i = x1 ; i < x2 ; i++)
		{
			Point2D p;
			p.x = i;
			p.y = y1;
			points.push_back(p);
		}
	}
	else if(m <=1 && m >= -1)
	{ // non-steep line, iterate over x
		float ypos = (float)y1;
		for(int x = x1 ; x < x2 ; x++)
		{
			Point2D p;
			p.x = x;
			p.y = (int)ypos;
			points.push_back(p);
			ypos += m;
		}
	}
	else if(m > 1 || m < -1)
	{ // steep line, iterate over y
		m = 1.0/m; //invert slope for vertical use
		float xpos;
		if(y1 < y2)
		{
			xpos = (float)x1;
			for(int y = y1 ; y < y2 ; y++)
			{
				Point2D p;
				p.x = (int)xpos;
				p.y = y;
				points.push_back(p);
				xpos += m;
			}
		}
		else
		{
			xpos = (float)x2;
			for(int y = y2 ; y < y1 ; y++)
			{
				Point2D p;
				p.x = (int)xpos;
				p.y = y;
				points.push_back(p);
				xpos += m;
			}
		}
	}

	return points;
}

std::vector<Point3D> getPointsFromLine3D(int x1, int y1, float z1,
		int x2, int y2, float z2)
{
	if(x1 > x2)
	{
		int tempi = x1;
		x1 = x2;
		x2 = tempi;
		tempi = y1;
		y1 = y2;
		y2 = tempi;
		float tempf = z1;
		z1 = z2;
		z2 = tempf;
	}

	std::vector<Point2D> twopoints = getPointsFromLine2D(x1, y1, x2, y2);

	std::vector<Point3D> threepoints;

	float dz = z2 - z1; //difference in z between the two endpoints
	float incz = dz/((float)twopoints.size()); //how much to increment

	float ziter = z1; //the value to give the points
	for(std::vector<Point2D>::iterator it = twopoints.begin(),
			end = twopoints.end() ; it != end ; ++it)
	{
		Point3D p;
		p.x = it->x;
		p.y = it->y;
		p.z = ziter;
		threepoints.push_back(p);
		ziter += incz;
	}

	return threepoints;
}

vec3 vec4Tovec3(vec4 &v)
{
	vec3 vec;
	vec.x = v.x;
	vec.y = v.y;
	vec.z = v.z;

	return vec;
}

std::vector<vec3> vec4Tovec3(std::vector<vec4> list)
{
	std::vector<vec3> converted;

	for(std::vector<vec4>::iterator it = list.begin(), end = list.end() ;
			it != end ; ++it)
	{
		vec3 v;
		v.x = it->x;
		v.y = it->y;
		v.z = it->z;
		converted.push_back(v);
	}
	return converted;
}

vec2 vec4Tovec2(vec4 &v)
{
	vec2 vec;
	vec.x = v.x;
	vec.y = v.y;
	return vec;
}

std::vector<vec2> vec4Tovec2(std::vector<vec4> list)
{
	std::vector<vec2> converted;

	for(std::vector<vec4>::iterator it = list.begin(), end = list.end() ;
			it != end ; ++it)
	{
		vec2 v;
		v.x = it->x;
		v.y = it->y;
		converted.push_back(v);
	}
	return converted;
}

vec2 vec3Tovec2(vec3 &v)
{
	vec2 vec;
	vec.x = v.x;
	vec.y = v.y;

	return vec;
}

std::vector<vec2> vec3Tovec2(std::vector<vec3> list)
{
	std::vector<vec2> converted;

	for(std::vector<vec3>::iterator it = list.begin(), end = list.end() ;
			it != end ; ++it)
	{
		vec2 v;
		v.x = it->x;
		v.y = it->y;
		converted.push_back(v);
	}
	return converted;
}

#endif /* HELPERS_HPP_ */
