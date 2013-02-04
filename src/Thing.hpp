/*
 * Thing.hpp
 *
 *  Created on: Jan 23, 2013
 *      Author: Travis Lewis
 */

#ifndef OBJECT_HPP_
#define OBJECT_HPP_

//std includes
#include <vector>

//other includes
#include "vec.h"
#include "mat.h"

struct Thing
{
public:
	static const int LINE = 1;
	static const int TRIANGLE = 2;
	static const int RGB = 3;
	static const int CUBE = 4;
	static const int CYLINDER = 5;
	static const int CONE = 6;

	int type;
	std::vector<vec4> points;
	float r,g,b;
};

void thingRotateX(Thing* t, int deg)
{
	mat4 m = RotateX(deg);
	for(std::vector<vec4>::iterator it = t->points.begin(),
			end = t->points.end() ; it != end ; ++it)
	{
		*it = m * (*it);
	}
}

void thingRotateY(Thing* t, int deg)
{
	mat4 m = RotateY(deg);
	for(std::vector<vec4>::iterator it = t->points.begin(),
			end = t->points.end() ; it != end ; ++it)
	{
		*it = m * (*it);
	}
}

void thingRotateZ(Thing* t, int deg)
{
	mat4 m = RotateZ(deg);
	for(std::vector<vec4>::iterator it = t->points.begin(),
			end = t->points.end() ; it != end ; ++it)
	{
		*it = m * (*it);
	}
}


#endif /* OBJECT_HPP_ */
