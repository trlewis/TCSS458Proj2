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
#include "keys.h"

using namespace std;

//defines to make understanding the code easier
#define TRUE 1
#define FALSE 0

unsigned int window_width,// = 512,
             window_height;// = 512;

//the number of pixels the window contains
int size;

float* pixels;
float* zbuffer;
float red, green, blue;

std::vector<Thing> things; //the things to draw
mat4 CTM; //current transformation matrix
bool perspective_init = false;

//FUNCTION PROTOTYPES
int objToPix(float f, int pixels);
void drawLine(vec2 &v1, vec2 &v2);
void drawLine(int x1, int y1, int x2, int y2);
void drawLine3D(vec3 & v1, vec3 &v2);
void drawLine3D(int x1, int y1, float z1, int x2, int y2, float z2);
void drawTriangle(Thing* t);
void drawTriangle3D(Thing* t);


void putPixel(int x, int y, float r, float g, float b) {
    if (0 <= x && x < (int)window_width &&
        0 <= y && y < (int)window_height) {
        pixels[y*window_width*3+x*3] = r;  // red
        pixels[y*window_width*3+x*3+1] = g;  // green
        pixels[y*window_width*3+x*3+2] = b;  // blue
    } else {
        printf("Pixel out of bounds: %d %d\n", x, y);
    }
}

void putPixel3D(int x, int y, float z, float r, float g, float b)
{
	if(x >= 0 && x < (int)window_width &&
	   y >= 0 && y < (int)window_height)
	{
		int buff = y*window_width + x;

		//TODO: figure out what the proper z-buffer conditions are...
		if(z > zbuffer[buff])
		{
			zbuffer[buff] = z;
	        pixels[y*window_width*3+x*3] = r;  // red
	        pixels[y*window_width*3+x*3+1] = g;  // green
	        pixels[y*window_width*3+x*3+2] = b;  // blue
		}
	}
	else
	{
		printf("Pixel out of bounds: x=%d, y=%d\n", x, y);
	}
}

void display()
{
	//TODO: figure out what to clear this to...
	for(int i = 0 ; i < size ; i++)
		zbuffer[i] = -100.0;

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
			std::vector<vec3> points = vec4Tovec3(it->points);
			//drawLine(points[0], points[1]);
			drawLine3D(points[0], points[1]);
			break;
		}
		case Thing::TRIANGLE:
			//drawTriangle(&*it);
			drawTriangle3D(&*it);
			break;
		case Thing::RGB:
			red = it->r;
			green = it->g;
			blue = it->b;
			break;
		case Thing::WIRE_CUBE: {
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

void drawLine3D(vec3 & v1, vec3 &v2)
{
	int x1 = objToPix(v1.x, window_width);
	int y1 = objToPix(v1.y, window_height);
	int x2 = objToPix(v2.x, window_width);
	int y2 = objToPix(v2.y, window_height);
	drawLine3D(x1, y1, v1.z, x2, y2, v2.z);
}

void drawLine3D(int x1, int y1, float z1, int x2, int y2, float z2)
{
	std::vector<Point3D> points = getPointsFromLine3D(x1, y1, z1, x2, y2, z2);
	for(std::vector<Point3D>::iterator it = points.begin(),
			end = points.end() ; it != end ; ++it)
	{
		putPixel3D(it->x, it->y, it->z, red, green, blue);
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

void drawTriangle3D(Thing* t)
{
	std::vector<vec3> points = vec4Tovec3(t->points);

	TriLines3D tl;

	int x1 = objToPix(points[0].x, window_width);
	int y1 = objToPix(points[0].y, window_height);
	float z1 = points[0].z;
	int x2 = objToPix(points[1].x, window_width);
	int y2 = objToPix(points[1].y, window_height);
	float z2 = points[1].z;
	int x3 = objToPix(points[2].x, window_width);
	int y3 = objToPix(points[2].y, window_height);
	float z3 = points[2].z;

	tl.addLine(x1, y1, z1, x2, y2, z2);
	tl.addLine(x1, y1, z1, x3, y3, z3);
	tl.addLine(x2, y2, z2, x3, y3, z3);

	for(std::vector<HorizLine3D>::iterator it = tl.lines.begin(),
			end = tl.lines.end() ; it != end ; ++it)
	{
		drawLine3D(it->left, it->y, it->zleft, it->right, it->y, it->zright);
	}
}

void drawTriangle3D(vec4 a, vec4 b, vec4 c)
{
	Thing t;
	t.type = Thing::TRIANGLE;

	t.points.push_back(a);
	t.points.push_back(b);
	t.points.push_back(c);

	drawTriangle3D(&t);
}

void drawWireCube3D(Thing* t)
{
//TODO: fill this out so the corresponding code can be removed from display()
// but make sure to draw using the zbuffer
}

void drawSolidCube3D(Thing* t)
{
	//point sets that make up the triangles:
	//top: (0, 1, 2), (0, 3, 2)
	drawTriangle3D(t->points[0], t->points[1], t->points[2]);
	drawTriangle3D(t->points[0], t->points[3], t->points[2]);
	//bottom: (4, 5, 6), (4, 7, 6)
	drawTriangle3D(t->points[4], t->points[5], t->points[6]);
	drawTriangle3D(t->points[4], t->points[7], t->points[6]);
	//left: (0, 4, 7), (0, 3, 7)
	drawTriangle3D(t->points[0], t->points[4], t->points[7]);
	drawTriangle3D(t->points[0], t->points[3], t->points[7]);
	//right: (1, 5, 6), (1, 2, 6)
	drawTriangle3D(t->points[1], t->points[5], t->points[6]);
	drawTriangle3D(t->points[1], t->points[2], t->points[6]);
	//front: (2, 3, 7), (2, 6, 7)
	drawTriangle3D(t->points[2], t->points[3], t->points[7]);
	drawTriangle3D(t->points[2], t->points[6], t->points[7]);
	//back: (0, 1, 4), (1, 4, 5)
	drawTriangle3D(t->points[0], t->points[1], t->points[4]);
	drawTriangle3D(t->points[1], t->points[4], t->points[5]);
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

			if(strcmp(s, "DIM") == 0)
				fscanf(input, "%d %d", &window_width, &window_height);
			else if(strcmp(s, "FRUSTUM") == 0)
			{
				if(!perspective_init)
				{
					//TODO: create frustum thing.
					//need to read: left/right/bottom/top/near/far
					perspective_init = true;
				}
			}
			else if(strcmp(s, "ORTHO") == 0)
			{
				if(!perspective_init)
				{
					//TODO: create ortho thing.
					//need to read: left/right/bottom/top/near/far
					perspective_init = true;
				}
			}
			else if(strcmp(s, "LOOKAT") == 0)
			{
				//TODO: create lookat code.
				//need to read: eyeX,eyeY,eyeZ,atX,atY,atZ,upX,upY,upZ
			}
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
			else if(strcmp(s, "SOLID_CUBE") == 0)
			{
				//TODO: create a solid cube
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
	if(key == KEY_LEFT_ARROW || key == KEY_RIGHT_ARROW)
	{
		int deg = (key == KEY_LEFT_ARROW) ? -2 : 2;
		for(std::vector<Thing>::iterator it = things.begin(),
				end = things.end() ; it != end ; ++it)
		{
			thingRotateY(&*it, deg);
		}
	}
	if(key == KEY_UP_ARROW || key == KEY_DOWN_ARROW)
	{
		int deg = (key == KEY_UP_ARROW) ? -2 : 2;
		for(std::vector<Thing>::iterator it = things.begin(),
				end = things.end() ; it != end ; ++it)
		{
			thingRotateX(&*it, deg);
		}
	}

	glutPostRedisplay();
}

//TODO: create a regular keyboard handler function

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	//read data from file
	readData();
	size = window_width * window_height;
	pixels = new float[size*3];
	zbuffer = new float[size];

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(window_width, window_height);
	glutCreateWindow("OpenGL glDrawPixels demo - simplified by JM");

	glutDisplayFunc(display);

	glutSpecialFunc(keyboardSpecial);

	glClearColor(0.0, 0.0, 0.0, 1.0);

	glutMainLoop();
}
