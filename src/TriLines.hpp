/*
 * TriLines.hpp
 *
 *  Created on: Jan 23, 2013
 *      Author: Travis Lewis
 */

#ifndef TRILINES_HPP_
#define TRILINES_HPP_

#include <vector>

#include "helpers.hpp"

struct HorizLine
{
	int y, left, right;
};

class TriLines
{
public:
	std::vector<HorizLine> lines;

	TriLines() { lines.clear(); }

	void addLine(int x1, int y1, int x2, int y2)
	{
		std::vector<Point2D> points = getPointsFromLine2D(x1, y1, x2, y2);
		for(std::vector<Point2D>::iterator it = points.begin(),
				end = points.end() ; it != end ; ++it)
		{
			addPoint(it->x, it->y);
		}
	}

	void addPoint(int x, int y)
	{
		if(!lines.empty())
		{
			bool found = false;
			for(std::vector<HorizLine>::iterator it = lines.begin(),
					end = lines.end() ; it != end ; ++it)
			{
				if(it->y == y)
				{
					found = true;
					if(x < it->left)
						it->left = x;
					if(x > it->right)
						it->right = x;
				}
			}
			if(!found)
				newLine(y,x);
		}
		else
			newLine(y,x);
	}

	void newLine(int y, int x)
	{
		HorizLine l;
		l.y = y;
		l.left = x;
		l.right = x;
		lines.push_back(l);
	}
};


#endif /* TRILINES_HPP_ */
