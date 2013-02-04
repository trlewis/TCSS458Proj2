/*
 * main.cpp
 *
 *  Created on: Jan 23, 2013
 *      Author: Travis Lewis
 */

//helper stuff, needs to be at the top for some reason.
#include "mat.h"
#include "vec.h"

//C includes
#include <stdio.h>
#include <string.h>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else // non-Mac OS X operating systems
#  include <GL/glut.h>
#endif  // __APPLE__

#include "math.h"

//C++ includes
#include <vector>
#include <iostream>

//my includes
#include "helpers.hpp"
#include "TriLines.hpp"
#include "Thing.hpp"

using namespace std;

//defines to make understanding the code easier
#define TRUE 1
#define FALSE 0

unsigned int window_width,// = 512,
             window_height;// = 512;

//the number of pixels the window contains
int size;

float* pixels;
float red, green, blue;

std::vector<Thing> things;

mat4 CTM;

//FUNCTION PROTOTYPES
int objToPix(float f, int pixels);
void drawLine(vec2 &v1, vec2 &v2);
void drawLine(int x1, int y1, int x2, int y2);
void drawTriangle(Thing* t);


void putPixel(int x, int y, float r, float g, float b) {
    if (0 <= x && x < (int)window_width &&
        0 <= y && y < (int)window_height) {
        pixels[y*window_width*3+x*3] = r;  // red
        pixels[y*window_width*3+x*3+1] = g;  // green
        pixels[y*window_width*3+x*3+2] = b;  // bluee
    } else {
        printf("Pixel out of bounds: %d %d", x, y);
    }
}

void display()
{
	for(unsigned int y = 0 ; y < window_height ; y++)
	{
		for(unsigned int x = 0 ; x < window_width ; x++)
			putPixel(x, y, 1.0, 1.0, 1.0); //clear to white...
			//putPixel(x,y, 1.0*y/window_height, 1.0*x/window_width,0);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//this is where the stuff gets drawn.
	for(std::vector<Thing>::iterator it = things.begin(), end = things.end();
		it != end ; ++it)
	{
		switch(it->type)
		{
		case Thing::LINE:
		{
			std::vector<vec2> points = vec4Tovec2(it->points);
			drawLine(points[0], points[1]);
			break;
		}
		case Thing::TRIANGLE:
			drawTriangle(&*it);
			break;
		case Thing::RGB:
			red = it->r;
			green = it->g;
			blue = it->b;
			break;
		case Thing::CUBE: {
			// (0,1), (0,2), (0,4), (1,3),
			// (1,5), (2,3), (2,6), (3,7),
			// (4,5), (4,6), (5,7), (6,7)
			std::vector<vec2> points = vec4Tovec2(it->points);

			drawLine(points[0], points[1]);
			drawLine(points[0], points[2]);
			drawLine(points[0], points[4]);
			drawLine(points[1], points[3]);

			drawLine(points[1], points[5]);
			drawLine(points[2], points[3]);
			drawLine(points[2], points[6]);
			drawLine(points[3], points[7]);

			drawLine(points[4], points[5]);
			drawLine(points[4], points[6]);
			drawLine(points[5], points[7]);
			drawLine(points[6], points[7]);

			break;
		}
		case Thing::CYLINDER: {
			std::vector<vec2> points = vec4Tovec2(it->points);
			unsigned int half = points.size()/2;

			//top circle
			for(unsigned int i = 1 ; i < half ; i++)
				drawLine(points[i-1], points[i]);
			drawLine(points[0], points[half-1]);

			//bottom circle
			for(unsigned int i = half ; i < points.size()-1 ; i++)
				drawLine(points[i+1], points[i]);
			drawLine(points[half],points[points.size()-1]);

			//middle lines
			for(unsigned int i = 0 ; i < half ; i++)
				drawLine(points[i], points[i+half]);

			break;
		}
		case Thing::CONE: {
			std::vector<vec2> points = vec4Tovec2(it->points);

			//draw lines from tip to base
			for(unsigned int i = 1 ; i < points.size() ; i++)
				drawLine(points[0], points[i]);

			//draw base circle lines
			for(unsigned int i = 2 ; i < points.size() ; i++)
				drawLine(points[i-1], points[i]);
			drawLine(points[1], points[points.size()-1]);

			break;
		}
		}

	}

	//glDrawPixels writes a block of pixels to the framebuffer.
	glDrawPixels(window_width,window_height,GL_RGB,GL_FLOAT,pixels);

	glutSwapBuffers();
}

void drawLine(vec2 &v1, vec2 &v2)
{
	int x1 = objToPix(v1.x, window_width);
	int y1 = objToPix(v1.y, window_height);
	int x2 = objToPix(v2.x, window_width);
	int y2 = objToPix(v2.y, window_height);
	drawLine(x1, y1, x2, y2);
}

void drawLine(int x1, int y1, int x2, int y2)
{
	std::vector<Point2D> points = getPointsFromLine2D(x1, y1, x2, y2);
	for(std::vector<Point2D>::iterator it = points.begin(),
			end = points.end() ; it != end ; ++it)
	{
		putPixel(it->x, it->y, red, green, blue);
	}
}

void drawTriangle(Thing* t)
{
	std::vector<vec2> points = vec4Tovec2(t->points);

	TriLines tl;

	int x1 = objToPix(points[0].x, window_width);
	int y1 = objToPix(points[0].y, window_height);
	int x2 = objToPix(points[1].x, window_width);
	int y2 = objToPix(points[1].y, window_height);
	int x3 = objToPix(points[2].x, window_width);
	int y3 = objToPix(points[2].y, window_height);

	tl.addLine(x1, y1, x2, y2);
	tl.addLine(x1, y1, x3, y3);
	tl.addLine(x2, y2, x3, y3);

	for(std::vector<HorizLine>::iterator it = tl.lines.begin(),
			end = tl.lines.end() ; it != end ; ++it)
	{
		drawLine(it->left, it->y, it->right, it->y);
	}
}





/**
 * \brief Parses an input data file
 */
void readData()
{
	FILE* input;
	printf("Enter name of input file (no blanks): ");
	char filename[50];
	gets(filename);

	char s[30];

	input = fopen(filename, "r+");
	if(input == NULL)
		perror("Error opening file");
	else
	{
		while(feof(input) == FALSE)
		{
			fscanf(input,"%s",s);

			//TODO: create an if statement that will read until the end of
			// the line is encountered whenever a // is read
			if(strcmp(s, "DIM") == 0)
				fscanf(input, "%d %d", &window_width, &window_height);
			else if(strcmp(s, "LINE") == 0)
			{
				Thing t;
				t.type = Thing::LINE;
				for(int i = 0 ; i < 2 ; i++)
				{
					vec4 p;
					fscanf(input, "%f %f %f", &p.x, &p.y, &p.z);
					p.w = 1.0;
					t.points.push_back(p);
				}

				for(std::vector<vec4>::iterator it = t.points.begin(),
						end = t.points.end() ; it != end ; ++it)
				{
					*it = CTM * (*it);
				}

				things.push_back(t);
			}
			else if(strcmp(s, "RGB") == 0)
			{
				//read R, G, B
				Thing t;
				t.type = Thing::RGB;
				fscanf(input, "%f %f %f", &t.r, &t.g, &t.b);
				things.push_back(t);
			}
			else if(strcmp(s, "TRI") == 0)
			{
				//read x1, y1, x2, y2, x3, y3
				Thing t;
				t.type = Thing::TRIANGLE;
				for(int i = 0 ; i < 3 ; i++)
				{
					vec4 p;
					fscanf(input, "%f %f %f", &p.x, &p.y, &p.z);
					p.w = 1.0;
					t.points.push_back(p);
				}

				for(std::vector<vec4>::iterator it = t.points.begin(),
						end = t.points.end() ; it != end ; ++it)
				{
					*it = CTM * (*it);
				}

				things.push_back(t);
			}
			else if(strcmp(s, "WIREFRAME_CUBE") == 0)
			{
				Thing t = createUnitCube();

				for(std::vector<vec4>::iterator it = t.points.begin(),
						end = t.points.end() ; it != end ; ++it)
				{
					*it = CTM * (*it);
				}
				things.push_back(t);
			}
			else if(strcmp(s, "CYLINDER") == 0)
			{
				int c;
				fscanf(input, "%d", &c);
				Thing t = createUnitCylinder(c);

				for(std::vector<vec4>::iterator it = t.points.begin(),
						end = t.points.end() ; it != end ; ++it)
				{
					*it = CTM * (*it);
				}
				things.push_back(t);
			}
			else if(strcmp(s, "CONE") == 0)
			{
				int c;
				fscanf(input, "%d", &c);
				Thing t = createUnitCone(c);
				for(std::vector<vec4>::iterator it = t.points.begin(),
						end = t.points.end() ; it != end ; ++it)
				{
					*it = CTM * (*it);
				}
				things.push_back(t);
			}
			else if(strcmp(s, "LOAD_IDENTITY_MATRIX") == 0)
			{
				CTM = mat4();
			}
			else if(strcmp(s, "ROTATEX") == 0)
			{
				float angle;
				fscanf(input, "%f", &angle);
				CTM = RotateX(angle) * CTM;
			}
			else if(strcmp(s, "ROTATEY") == 0)
			{
				float angle;
				fscanf(input, "%f", &angle);
				CTM = RotateY(angle) * CTM;
			}
			else if(strcmp(s, "ROTATEZ") == 0)
			{
				float angle;
				fscanf(input, "%f", &angle);
				CTM = RotateZ(angle) * CTM;
			}
			else if(strcmp(s, "SCALE") == 0)
			{
				float sx, sy, sz;
				fscanf(input, "%f %f %f", &sx, &sy, &sz);
				CTM = Scale(sx, sy, sz) * CTM;
			}
			else if(strcmp(s, "TRANSLATE") == 0)
			{
				float tx, ty, tz;
				fscanf(input, "%f %f %f", &tx, &ty, &tz);
				CTM = Translate(tx, ty, tz) * CTM;
			}
		}
	}
	fclose(input);
}

/**
 * Converts object coordinates (-1 <= f <= 1) to pixel coordinates.
 * @param f The object coordinate to convert.
 * @param pixels The number of pixels in the dimension to convert.
 */
int objToPix(float f, int pixels)
{
	float result = (float)pixels * ((f+1.0)/2.0);
	int num = (int)(result*10) % 10;
	if(num >= 5)
		return ((int) result) + 1; // round up
	else
		return (int) result; // round down
}

void keyboardSpecial(int key, int x, int y)
{
	//100 = left
	//101 = up
	//102 = right
	//103 = down

	if(key == 100 || key == 102)
	{
		int deg = (key == 100) ? -2 : 2;
		for(std::vector<Thing>::iterator it = things.begin(),
				end = things.end() ; it != end ; ++it)
		{
			thingRotateY(&*it, deg);
		}
	}
	if(key == 101 || key == 103)
	{
		int deg = (key == 101) ? -2 : 2;
		for(std::vector<Thing>::iterator it = things.begin(),
				end = things.end() ; it != end ; ++it)
		{
			thingRotateX(&*it, deg);
		}
	}

	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	//read data from file
	readData();
	size = window_width * window_height;
	pixels = new float[size*3];

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(window_width, window_height);
	glutCreateWindow("OpenGL glDrawPixels demo - simplified by JM");

	glutDisplayFunc(display);

	glutSpecialFunc(keyboardSpecial);

	glClearColor(0.0, 0.0, 0.0, 1.0);

	glutMainLoop();
}
