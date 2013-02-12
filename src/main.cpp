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
//#include <stdio.h>
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
#include <fstream>
#include <string>
#include <sstream>

//my includes
#include "helpers.hpp"
#include "TriLines.hpp"
#include "Thing.hpp"
#include "keys.h"

using namespace std;

//defines to make understanding the code easier
#define TRUE 1
#define FALSE 0

//used to keep track of which projection to use.
#define PROJ_DEFAULT 0
#define PROJ_ORTHO 1
#define PROJ_FRUSTUM 2

unsigned int window_width,// = 512,
             window_height;// = 512;

//the number of pixels the window contains
int size;

float* pixels;
float* zbuffer;
float zbuffer_default;
float red, green, blue;

std::vector<Thing> things; //the things to draw
mat4 CTM; //current transformation matrix
mat4 ortho_mat = mat4();
mat4 frustum_mat = mat4();
mat4 lookat_mat = mat4();
float pleft = 0.0, pright = 0.0, pbottom = 0.0, ptop = 0.0, pnear = 0.0,
		pfar = 0.0;
bool perspective_init = false;
bool lookat_init = false;
int current_proj = PROJ_DEFAULT;

//FUNCTION PROTOTYPES
int objToPix(float f, int pixels);
void drawLine(vec2 &v1, vec2 &v2);
void drawLine(int x1, int y1, int x2, int y2);
void drawLine3D(vec3 & v1, vec3 &v2);
void drawLine3D(int x1, int y1, float z1, int x2, int y2, float z2);
void drawTriangle(Thing* t);
void drawTriangle3D(Thing* t);
void drawSolidCube3D(Thing* t);
int parseString(std::stringstream* stream, std::vector<string>* v, char delim);


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
			y >= 0 && y < (int)window_height) {
		int buff = y*window_width+x;

		//TODO: figure out what the proper z-buffer conditions are...
		if(z > zbuffer[buff]) {
			zbuffer[buff] = z;
	        pixels[y*window_width*3+x*3] = r;  // red
	        pixels[y*window_width*3+x*3+1] = g;  // green
	        pixels[y*window_width*3+x*3+2] = b;  // blue
		}
	} else {
		printf("Pixel out of bounds: x=%d, y=%d\n",x,y);
	}
}

void display() {
	//TODO: figure out what to clear this to...
	for(int i = 0 ; i < size ; i++)
		zbuffer[i] = -100.0;

	for(unsigned int y = 0 ; y < window_height ; y++) {
		for(unsigned int x = 0 ; x < window_width ; x++)
			putPixel(x,y,1.0,1.0,1.0); //clear to white...
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//	mat4 mat = mat4();
//	mat[3].w = 0.0;
//
//	if(perspective_init && current_proj == PROJ_FRUSTUM)
//		//mat = frustum_mat * mat;
//		mat = frustum_mat;
//		//mat = mat * frustum_mat;
//	else if(perspective_init && current_proj == PROJ_ORTHO)
//		//mat = ortho_mat * mat;
//		mat = ortho_mat;
//		//mat = mat * ortho_mat;
//
//	if(lookat_init) {
//		//mat = lookat_mat * mat;
//		mat = mat * lookat_mat;
//	}

//	mat4 mat;
//	if(perspective_init) {
//		if(lookat_init) {
//			if(current_proj == PROJ_FRUSTUM)
//				mat = frustum_mat * lookat_mat;
//			else if(current_proj == PROJ_ORTHO)
//				mat = ortho_mat * lookat_mat;
//
//		} else {
//			if(current_proj == PROJ_FRUSTUM)
//				mat = frustum_mat;
//			else if(current_proj == PROJ_ORTHO)
//				mat = ortho_mat;
//		}
//	} else {
//		if(lookat_init)
//			mat = lookat_mat;
//		else {
//			mat = mat4();
//			mat[3].w = 0.0;
//		}
//	}


	//this is where the stuff gets drawn.
	for(std::vector<Thing>::iterator it = things.begin(), end = things.end();
			it != end ; ++it) {

		Thing th = it->clone();

		mat4 mat;
		if(perspective_init) {
			if(lookat_init) {
				if(current_proj == PROJ_FRUSTUM)
					mat = frustum_mat * lookat_mat;// * th.CTM;
				else if(current_proj == PROJ_ORTHO)
					mat = ortho_mat * lookat_mat;// * th.CTM;

			} else {
				if(current_proj == PROJ_FRUSTUM)
					mat = frustum_mat;// * th.CTM;
				else if(current_proj == PROJ_ORTHO)
					mat = ortho_mat;// * th.CTM;
			}
		} else {
			if(lookat_init)
				mat = lookat_mat;// * th.CTM;
			else {
				mat = mat4();// * th.CTM;
				//mat[3].w = 0.0;
			}
		}

		for(unsigned int i = 0 ; i < th.points.size() ; i++) {
			th.points[i] = mat * th.points[i];
			th.points[i] = th.points[i] / th.points[i].w;
		}

//		for(std::vector<vec4>::iterator i = th.points.begin(),
//				e = th.points.end() ; i != e ; ++i) {
//			*i = ()
//		}

		//mat4 p = th.CTM * mat;
		//applyViewMatrix(&th,&p);
		//applyViewMatrix(&th,&mat);

		switch(it->type) {
		case Thing::LINE: {
			//std::vector<vec3> points = vec4Tovec3(it->points);
			std::vector<vec3> points = vec4Tovec3(th.points);
			drawLine3D(points[0],points[1]);
			break;
		}
		case Thing::TRIANGLE:
			//drawTriangle3D(&*it);
			drawTriangle3D(&th);
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
			//std::vector<vec2> points = vec4Tovec2(it->points);
			std::vector<vec3> points = vec4Tovec3(th.points);

			drawLine3D(points[0],points[1]);
			drawLine3D(points[0],points[2]);
			drawLine3D(points[0],points[4]);
			drawLine3D(points[1],points[3]);

			drawLine3D(points[1],points[5]);
			drawLine3D(points[2],points[3]);
			drawLine3D(points[2],points[6]);
			drawLine3D(points[3],points[7]);

			drawLine3D(points[4],points[5]);
			drawLine3D(points[4],points[6]);
			drawLine3D(points[5],points[7]);
			drawLine3D(points[6],points[7]);
			break;
		}
		case Thing::SOLID_CUBE: {
			drawSolidCube3D(&th);
		}
		case Thing::CYLINDER: {
			//std::vector<vec2> points = vec4Tovec2(it->points);
			std::vector<vec3> points = vec4Tovec3(th.points);
			unsigned int half = points.size()/2;
			//top circle
			for(unsigned int i = 1 ; i < half ; i++)
				drawLine3D(points[i-1],points[i]);
			drawLine3D(points[0],points[half-1]);
			//bottom circle
			for(unsigned int i = half ; i < points.size()-1 ; i++)
				drawLine3D(points[i+1],points[i]);
			drawLine3D(points[half],points[points.size()-1]);
			//middle lines
			for(unsigned int i = 0 ; i < half ; i++)
				drawLine3D(points[i],points[i+half]);
			break;
		}
		case Thing::CONE: {
			//std::vector<vec2> points = vec4Tovec2(it->points);
			std::vector<vec3> points = vec4Tovec3(th.points);
			//draw lines from tip to base
			for(unsigned int i = 1 ; i < points.size() ; i++)
				drawLine3D(points[0],points[i]);
			//draw base circle lines
			for(unsigned int i = 2 ; i < points.size() ; i++)
				drawLine3D(points[i-1],points[i]);
			drawLine3D(points[1],points[points.size()-1]);
			break;
		}
		}//switch type
	}
	//glDrawPixels writes a block of pixels to the framebuffer.
	glDrawPixels(window_width,window_height,GL_RGB,GL_FLOAT,pixels);
	glutSwapBuffers();
}

void drawLine(vec2 &v1, vec2 &v2) {
	int x1 = objToPix(v1.x,window_width);
	int y1 = objToPix(v1.y,window_height);
	int x2 = objToPix(v2.x,window_width);
	int y2 = objToPix(v2.y,window_height);
	drawLine(x1,y1,x2,y2);
}

void drawLine(int x1, int y1, int x2, int y2) {
	std::vector<Point2D> points = getPointsFromLine2D(x1, y1, x2, y2);
	for(std::vector<Point2D>::iterator it = points.begin(),
			end = points.end() ; it != end ; ++it)
		putPixel(it->x,it->y,red,green,blue);
}

void drawLine3D(vec3 & v1, vec3 &v2) {
	int x1 = objToPix(v1.x,window_width);
	int y1 = objToPix(v1.y,window_height);
	int x2 = objToPix(v2.x,window_width);
	int y2 = objToPix(v2.y,window_height);
	drawLine3D(x1,y1,v1.z,x2,y2,v2.z);
}

void drawLine3D(int x1, int y1, float z1, int x2, int y2, float z2) {
	std::vector<Point3D> points = getPointsFromLine3D(x1, y1, z1, x2, y2, z2);
	for(std::vector<Point3D>::iterator it = points.begin(),
			end = points.end() ; it != end ; ++it)
		putPixel3D(it->x,it->y,it->z,red,green,blue);
}

void drawTriangle(Thing* t) {
	std::vector<vec2> points = vec4Tovec2(t->points);
	TriLines tl;

	int x1 = objToPix(points[0].x,window_width);
	int y1 = objToPix(points[0].y,window_height);
	int x2 = objToPix(points[1].x,window_width);
	int y2 = objToPix(points[1].y,window_height);
	int x3 = objToPix(points[2].x,window_width);
	int y3 = objToPix(points[2].y,window_height);

	tl.addLine(x1,y1,x2,y2);
	tl.addLine(x1,y1,x3,y3);
	tl.addLine(x2,y2,x3,y3);

	for(std::vector<HorizLine>::iterator it = tl.lines.begin(),
			end = tl.lines.end() ; it != end ; ++it)
		drawLine(it->left,it->y,it->right,it->y);
}

void drawTriangle3D(Thing* t) {
	std::vector<vec3> points = vec4Tovec3(t->points);
	TriLines3D tl;

	int x1 = objToPix(points[0].x,window_width);
	int y1 = objToPix(points[0].y,window_height);
	float z1 = points[0].z;
	int x2 = objToPix(points[1].x,window_width);
	int y2 = objToPix(points[1].y,window_height);
	float z2 = points[1].z;
	int x3 = objToPix(points[2].x,window_width);
	int y3 = objToPix(points[2].y,window_height);
	float z3 = points[2].z;

	tl.addLine(x1,y1,z1,x2,y2,z2);
	tl.addLine(x1,y1,z1,x3,y3,z3);
	tl.addLine(x2,y2,z2,x3,y3,z3);

	for(std::vector<HorizLine3D>::iterator it = tl.lines.begin(),
			end = tl.lines.end() ; it != end ; ++it)
		drawLine3D(it->left,it->y,it->zleft,it->right,it->y,it->zright);
}

void drawTriangle3D(vec4 a, vec4 b, vec4 c) {
	Thing t;
	t.type = Thing::TRIANGLE;
	t.points.push_back(a);
	t.points.push_back(b);
	t.points.push_back(c);
	drawTriangle3D(&t);
}

void drawWireCube3D(Thing* t) {
//TODO: fill this out so the corresponding code can be removed from display()
// but make sure to draw using the zbuffer
}

void drawSolidCube3D(Thing* t) {
	//point sets that make up the triangles:
	//top: (0, 1, 2), (0, 3, 2)
	drawTriangle3D(t->points[0],t->points[1],t->points[2]);
	drawTriangle3D(t->points[0],t->points[3],t->points[2]);
	//bottom: (4, 5, 6), (4, 7, 6)
	drawTriangle3D(t->points[4],t->points[5],t->points[6]);
	drawTriangle3D(t->points[4],t->points[7],t->points[6]);
	//left: (0, 4, 7), (0, 3, 7)
	drawTriangle3D(t->points[0],t->points[4],t->points[7]);
	drawTriangle3D(t->points[0],t->points[3],t->points[7]);
	//right: (1, 5, 6), (1, 2, 6)
	drawTriangle3D(t->points[1],t->points[5],t->points[6]);
	drawTriangle3D(t->points[1],t->points[2],t->points[6]);
	//front: (2, 3, 7), (2, 6, 7)
	drawTriangle3D(t->points[2],t->points[3],t->points[7]);
	drawTriangle3D(t->points[2],t->points[6],t->points[7]);
	//back: (0, 1, 4), (1, 4, 5)
	drawTriangle3D(t->points[0],t->points[1],t->points[4]);
	drawTriangle3D(t->points[1],t->points[4],t->points[5]);
}

/**
 * \brief Parses an input data file
 */
void readData() {
	string str;
	cout << "Enter name of input file (no blanks): ";
	getline(cin,str);
	ifstream file(str.c_str());

	if(!file.is_open())
		cerr << "Error opening file [" << str << "]" << endl;
	else {
		string in;
		std::vector<string> params;
		while(file.good()) {
			getline(file,str);
			std::stringstream ss(str);
			getline(ss,in,' ');
			if(in == "DIM") {
				parseString(&ss,&params,' ');
				window_width = atoi(params[0].c_str());
				window_height = atoi(params[1].c_str());
			} else if(in == "FRUSTUM") {
				if(!perspective_init) {
					parseString(&ss,&params,' ');
//					float l = atof(params[0].c_str());
//					float r = atof(params[1].c_str());
//					float b = atof(params[2].c_str());
//					float t = atof(params[3].c_str());
//					float n = atof(params[4].c_str());
//					float f = atof(params[5].c_str());
					pleft = atof(params[0].c_str());
					pright = atof(params[1].c_str());
					pbottom = atof(params[2].c_str());
					ptop = atof(params[3].c_str());
					pnear = atof(params[4].c_str());
					pfar = atof(params[5].c_str());
					//zbuffer_default = pfar; //TODO: adjust as necessary
					//frustum_mat = Frustum(l,r,b,t,n,f);
					//ortho_mat = Ortho(l,r,b,t,n,f);
					frustum_mat = Frustum(pleft,pright,pbottom,
							ptop,pnear,pfar);
					ortho_mat = Ortho(pleft,pright,pbottom,ptop,pnear,pfar);
					cout << "frustum matrix: \n" << frustum_mat
							<< "\northo matrix\n" << ortho_mat << endl;
					perspective_init = true;
					current_proj = PROJ_FRUSTUM;
				}
			} else if(in == "ORTHO") {
				if(!perspective_init) {
					parseString(&ss,&params,' ');
//					float l = atof(params[0].c_str());
//					float r = atof(params[1].c_str());
//					float b = atof(params[2].c_str());
//					float t = atof(params[3].c_str());
//					float n = atof(params[4].c_str());
//					float f = atof(params[5].c_str());
					pleft = atof(params[0].c_str());
					pright = atof(params[1].c_str());
					pbottom = atof(params[2].c_str());
					ptop = atof(params[3].c_str());
					pnear = atof(params[4].c_str());
					pfar = atof(params[5].c_str());
//					zbuffer_default = f; //TODO: adjust as necessary
//					frustum_mat = Frustum(l,r,b,t,n,f);
//					ortho_mat = Ortho(l,r,b,t,n,f);
					frustum_mat = Frustum(pleft,pright,pbottom,
							ptop,pnear,pfar);
					ortho_mat = Ortho(pleft,pright,pbottom,ptop,pnear,pfar);
					cout << "frustum matrix: \n" << frustum_mat
							<< "\northo matrix\n" << ortho_mat << endl;
					perspective_init = true;
					current_proj = PROJ_ORTHO;
				}
			} else if(in == "LOOKAT") {
				parseString(&ss,&params,' ');
				float ex = atof(params[0].c_str());
				float ey = atof(params[1].c_str());
				float ez = atof(params[2].c_str());
				float ax = atof(params[3].c_str());
				float ay = atof(params[4].c_str());
				float az = atof(params[5].c_str());
				float ux = atof(params[6].c_str());
				float uy = atof(params[7].c_str());
				float uz = atof(params[8].c_str());
				lookat_mat = LookAt(vec4(ex,ey,ez,1),vec4(ax,ay,az,1),
						vec4(ux,uy,uz,0.0));

				cout << "lookat matrix: " << endl << lookat_mat;
				lookat_init = true;
				//TODO: create lookat code.
				//need to read: eyeX,eyeY,eyeZ,atX,atY,atZ,upX,upY,upZ
			} else if(in == "LINE") {
				parseString(&ss,&params,' ');
				Thing t = createLine(&params);
				applyMatrix(&t,&CTM);
				things.push_back(t);
			} else if(in == "RGB") {
				//read R, G, B
				Thing t;
				t.type = Thing::RGB;
				parseString(&ss,&params,' ');
				t.r = atof(params[0].c_str());
				t.g = atof(params[1].c_str());
				t.b = atof(params[2].c_str());
				things.push_back(t);
			} else if(in == "TRI") {
				//read x1, y1, x2, y2, x3, y3
				parseString(&ss,&params,' ');
				Thing t = createTriangle(&params);
				applyMatrix(&t,&CTM);
				things.push_back(t);
			} else if(in == "WIREFRAME_CUBE") {
				Thing t = createUnitCube();
				//t.CTM = CTM;
				//mat4 m = frustum_mat * lookat_mat * CTM;
				//applyMatrix(&t,&m);
				applyMatrix(&t,&CTM);
				things.push_back(t);
			} else if(in == "SOLID_CUBE") {
				Thing t = createSolidCube();
				applyMatrix(&t,&CTM);
				things.push_back(t);
			} else if(in == "CYLINDER") {
				parseString(&ss,&params,' ');
				Thing t = createUnitCylinder(atoi(params[0].c_str()));
				applyMatrix(&t,&CTM);
				things.push_back(t);
			} else if(in == "CONE") {
				parseString(&ss,&params,' ');
				Thing t = createUnitCone(atoi(params[0].c_str()));
				applyMatrix(&t,&CTM);
				things.push_back(t);
			} else if(in == "LOAD_IDENTITY_MATRIX") {
				CTM = mat4();
			} else if(in == "ROTATEX") {
				parseString(&ss,&params,' ');
				CTM = RotateX(atof(params[0].c_str())) * CTM;
			} else if(in == "ROTATEY") {
				parseString(&ss,&params,' ');
				CTM = RotateY(atof(params[0].c_str())) * CTM;
			} else if(in == "ROTATEZ") {
				parseString(&ss,&params,' ');
				CTM = RotateZ(atof(params[0].c_str())) * CTM;
			} else if(in == "SCALE") {
				parseString(&ss,&params,' ');
				float sx = atof(params[0].c_str());
				float sy = atof(params[1].c_str());
				float sz = atof(params[2].c_str());
				CTM = Scale(sx,sy,sz) * CTM;
			} else if(in == "TRANSLATE") {
				parseString(&ss,&params,' ');
				float tx = atof(params[0].c_str());
				float ty = atof(params[1].c_str());
				float tz = atof(params[2].c_str());
				CTM = Translate(tx,ty,tz) * CTM;
			}
		}//while file is good
	}
	file.close();
}

int parseString(std::stringstream* stream, std::vector<string>* v,
		char delim) {
	v->clear();
	std::string in;
	while(stream->good()) {
		std::getline(*stream,in,delim);
		v->push_back(in);
	}
	return v->size();
}

/**
 * Converts object coordinates (-1 <= f <= 1) to pixel coordinates.
 * @param f The object coordinate to convert.
 * @param pixels The number of pixels in the dimension to convert.
 */
int objToPix(float f, int pixels) {
	float result = (float)pixels * ((f+1.0)/2.0);
	int num = (int)(result*10) % 10;
	if(num >= 5)
		return ((int)result) + 1; // round up
	else
		return (int)result; // round down
}

void keyboardSpecial(int key, int x, int y) {
	if(key == KEY_LEFT_ARROW || key == KEY_RIGHT_ARROW) {
		int deg = (key == KEY_LEFT_ARROW) ? -2 : 2;
		for(std::vector<Thing>::iterator it = things.begin(),
				end = things.end() ; it != end ; ++it)
			thingRotateY(&*it,deg);
	}
	if(key == KEY_UP_ARROW || key == KEY_DOWN_ARROW) {
		int deg = (key == KEY_UP_ARROW) ? -2 : 2;
		for(std::vector<Thing>::iterator it = things.begin(),
				end = things.end() ; it != end ; ++it)
			thingRotateX(&*it,deg);
	}
	glutPostRedisplay();
}

void keyboardHandler(unsigned char key, int x, int y) {
	//change projection styles
	if(key == 'p' && perspective_init)
		current_proj = PROJ_FRUSTUM;
	if(key == 'o' && perspective_init)
		current_proj = PROJ_ORTHO;

	// adjust view voluem
	if(key == 'l') pleft *= 1.10;
	if(key == 'L') pleft *= 0.90;
	if(key == 'r') pright *= 1.10;
	if(key == 'R') pright *= 0.90;
	if(key == 't') ptop *= 1.10;
	if(key == 'T') ptop *= 0.90;
	if(key == 'b') pbottom *= 1.10;
	if(key == 'B') pbottom *= 0.90;
	if(key == 'n') pnear *= 1.10;
	if(key == 'N') pnear *= 0.90;
	if(key == 'f') pfar *= 1.10;
	if(key == 'F') pfar *= 0.90;

	//if only projection styles change then this doesn't need to be
	//recalculated, but meh, saves space on if statements.
	ortho_mat = Ortho(pleft,pright,pbottom,ptop,pnear,pfar);
	frustum_mat = Frustum(pleft,pright,pbottom,ptop,pnear,pfar);
	glutPostRedisplay();
}

//TODO: create a regular keyboard handler function

int main(int argc, char** argv) {
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

	glutKeyboardFunc(keyboardHandler);
	glutSpecialFunc(keyboardSpecial);

	glClearColor(0.0, 0.0, 0.0, 1.0);

	glutMainLoop();
}
