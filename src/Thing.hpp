/*
 * Thing.hpp
 *
 *  Created on: Jan 23, 2013
 *      Author: Travis Lewis
 */

#ifndef THING_HPP_
#define THING_HPP_

//std includes
#include <vector>

//other includes
#include "vec.h"
#include "mat.h"

class Thing {
public:
	static const int LINE = 1;
	static const int TRIANGLE = 2;
	static const int RGB = 3;
	static const int WIRE_CUBE = 4;
	static const int SOLID_CUBE = 5;
	static const int CYLINDER = 6;
	static const int CONE = 7;

	int type;
	std::vector<vec4> points;
	float r,g,b;
	mat4 CTM;

	Thing clone() {
		Thing t;
		t.type = type;
		t.r = r;
		t.g = g;
		t.b = b;
		for(std::vector<vec4>::iterator it = points.begin(),
				end = points.end() ; it != end ; ++it)
			t.points.push_back(*it);
		return t;
	}
};

void thingRotateX(Thing* t, int deg) {
	mat4 m = RotateX(deg);
	for(std::vector<vec4>::iterator it = t->points.begin(),
			end = t->points.end() ; it != end ; ++it)
		*it = m*(*it);
}

void thingRotateY(Thing* t, int deg) {
	mat4 m = RotateY(deg);
	for(std::vector<vec4>::iterator it = t->points.begin(),
			end = t->points.end() ; it != end ; ++it)
		*it = m*(*it);
}

void thingRotateZ(Thing* t, int deg) {
	mat4 m = RotateZ(deg);
	for(std::vector<vec4>::iterator it = t->points.begin(),
			end = t->points.end() ; it != end ; ++it)
		*it = m*(*it);
}

#endif /* THING_HPP_ */
