//============================================================================
// Name        : SRSdemo1.cpp
// Author      : Sung Hee Park @ ETRI
// Version     :
// Copyright   : (C) Copyright 2014
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <GL/glu.h>
#include <GL/glut.h>
#include "rapidxml/rapidxml.hpp"
#include "include/json/json.h"

#include <math.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <time.h>
#include <sstream>

#include "graph.h"

using namespace Json;

using namespace std;

using namespace rapidxml;

#define window_width  1024
#define window_height 768

#define ARTERY		1
#define GALLBLADDER   2
#define LIVER		3
#define PANCREAS	4
#define PORTALVEIN	5
#define STOMACH		6
#define LIGHTSTICK	7
#define INSTRUMENT1 8
#define INSTRUMENT2 9
#define NODEBALL	10
#define LIGHTARROW		11
#define CLIP		12


string model_filenames[]= {
		"C:\\Users\\sunghee\\Google 드라이브\\SRS\\HYJ\\artery_merged.json",
		"C:\\Users\\sunghee\\Google 드라이브\\SRS\\HYJ\\gall_merged.json",
		"C:\\Users\\sunghee\\Google 드라이브\\SRS\\HYJ\\liver_merged.json",
		"C:\\Users\\sunghee\\Google 드라이브\\SRS\\HYJ\\panc_merged.json",
		"C:\\Users\\sunghee\\Google 드라이브\\SRS\\HYJ\\portal_merged.json",
		"C:\\Users\\sunghee\\Google 드라이브\\SRS\\HYJ\\stom_merged.json"
};

#define BUFSIZE 1024


char *objectNames[] =
{"Nothing", "Artery", "Gallbladder", "Liver", "Pancreas",
"Portal Vein", "Stomach","Lightstick", "Instrument1", "Instrument2",
"Nodeball","Lightarrow", "CLIP"};

static GLfloat theta[] = {0.0, 0.0, 0.0}; // Rotation (X,Y,Z)
static GLint axis = 2; // Changed by the Mouse Callback Routine

// YLP 01/05/2013 : add scaling and dynamics rotations speed
static int lastx = 0, lasty = 0;
static GLfloat rotate_speed = 0.5f;
static GLfloat  scaling = 1.0f;
static GLfloat  shift = 1.0f;

GLuint selectBuf[BUFSIZE];

int W = 1024, H = 768;
GLfloat x, y;
int locating = 0;
int theObject = 0;
int menu_inuse = 0;
int mouse_state = 0;

static xml_document<> doc;
static xml_node<> * root_node=NULL;
//vector<char> elements;
static char * elements=NULL; // char variables for elements
static char * elements2=NULL;

string str_elements; // string variables for elements
string str_elements2;


static int submenu, submenu2;

//int window_width = 640;
//int window_height = 480;

#define X .525731112119133606
#define Z .850650808352039932

double sum_x=0.0, sum_y=0.0, sum_z=0.0;
double average_x=0.0, average_y=0.0, average_z=0.0;
double point_number = 0.0;

static GLfloat vdata[12][3] = {
   {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},
   {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},
   {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0}
};
static GLuint tindices[20][3] = {
   {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},
   /*{8,10,1},*/ {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},
   {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6},
   {6,1,10},/* {9,0,11},*/ {9,11,2}, {9,2,5}, {7,2,11} };

int i;


static GLfloat nodes[13][3] = {
		{0.0, 0.0, 100.}, {0.0, 0.0, 30.}, {0.0, 0.0, -20.}, {0.0, 0.0, -100.},// aorta: shortest (v0, v3),CT: shortest path(v1, v4),
		{0.0, -30., 30.}, {30., -30., 60.}, {60., -30., 40.}, //CT: shortest path(v1, v4),LGA:shortpath(v4, v6)
		{0., -40, 30},
		{-40., -40., 30.},  //CHA: shortest path(v4, v8)
		{-60., -40., 80.}, // LHA: shortest path(v8, v9)
		{ 20., -50., 30.}, {80.,-50., 30.}, // SpA: shortest path(v4, v10)
		{10., -50., -20.} // SMA: shortest path(v2,v11)
};

static GLuint edges[12][2] = {
		{0,1}, {1,2}, {2,3}, // aorta: shortest (v0, v3),
		{1,4},{4,7},//CT: shortest path(v1, v7),
		{4,5}, {5,6},//LGA:shortpath(v4, v6)
		{7,8},//CHA: shortest path(v7, v8)
		{8,9},// LHA: shortest path(v8, v9)
		{7,10},{10,11}, // SpA: shortest path(v7, v11)
		{2, 12}// SMA: shortest path(v2,v12)
};

string label_vessels[] ={"aorta" ,
					"celiac trunk",
					"LGA",
					"SpA",
					"CHA",
					"LHA",
					"SMA"
};

int sLabel=0, eLabel=7;

double rotate_by_key=0;

double rotate_x=0.5;
/*
void drawStomach()
{
	  glBegin(GL_TRIANGLES);
	  for (i = 0; i < 1439; i++) {
//	        glNormal3fv(&vnormal[(tindices2[3*i+0])*3+0]);
//	        glVertex3fv(&vdata2[(tindices2[3*i+0])*3+0]);
//	        glNormal3fv(&vnormal[(tindices2[3*i+1])*3+0]);
//	        glVertex3fv(&vdata2[(tindices2[3*i+1])*3+0]);
//	        glNormal3fv(&vnormal[(tindices2[3*i+2])*3+0]);
//	        glVertex3fv(&vdata2[(tindices2[3*i+2])*3+0]);
	        glNormal3f(vnormal[(tindices2[3*i+0])*3+0], vnormal[(tindices2[3*i+0])*3+1], vnormal[(tindices2[3*i+0])*3+2]);
	        glVertex3f(vdata2[(tindices2[3*i+0])*3+0],vdata2[(tindices2[3*i+0])*3+1], vdata2[(tindices2[3*i+0])*3+2]);
	        glNormal3f(vnormal[(tindices2[3*i+1])*3+0], vnormal[(tindices2[3*i+1])*3+1], vnormal[(tindices2[3*i+1])*3+2]);
	        glVertex3f(vdata2[(tindices2[3*i+1])*3+0],vdata2[(tindices2[3*i+1])*3+1], vdata2[(tindices2[3*i+1])*3+2]);
	        glNormal3f(vnormal[(tindices2[3*i+2])*3+0], vnormal[(tindices2[3*i+2])*3+1], vnormal[(tindices2[3*i+2])*3+2]);
	        glVertex3f(vdata2[(tindices2[3*i+2])*3+0],vdata2[(tindices2[3*i+2])*3+1], vdata2[(tindices2[3*i+2])*3+2]);
	        //	        glNormal3fv(&vdata[tindices[i][0]][0]);
	        //	        glVertex3fv(&vdata[tindices[i][0]][0]);
	        //	        glNormal3fv(&vdata[tindices[i][1]][0]);
	        //	        glVertex3fv(&vdata[tindices[i][1]][0]);
	        //	        glNormal3fv(&vdata[tindices[i][2]][0]);
	        //	        glVertex3fv(&vdata[tindices[i][2]][0]);

	  }
	  glEnd();

}
*/

void normalize(float v[3]) {
   GLfloat d = sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
   if (d == 0.0) {
      cout << "zero length vector" << endl;
      return;
   }
   v[0] /= d; v[1] /= d; v[2] /= d;
}

void normcrossprod(float v1[3], float v2[3], float out[3])
{
   GLint i, j;
   GLfloat length;

   out[0] = v1[1]*v2[2] - v1[2]*v2[1];
   out[1] = v1[2]*v2[0] - v1[0]*v2[2];
   out[2] = v1[0]*v2[1] - v1[1]*v2[0];
   normalize(out);
}

void drawtriangle(float *v1, float *v2, float *v3)
{
   glBegin(GL_TRIANGLES);
      glNormal3fv(v1); glVertex3fv(v1);
      glNormal3fv(v2); glVertex3fv(v2);
      glNormal3fv(v3); glVertex3fv(v3);
   glEnd();
}

void subdivide(float *v1, float *v2, float *v3, long depth)
{
   GLfloat v12[3], v23[3], v31[3];
   GLint i;

   if (depth == 0) {
      drawtriangle(v1, v2, v3);
      return;
   }
   for (i = 0; i < 3; i++) {
      v12[i] = v1[i]+v2[i];
      v23[i] = v2[i]+v3[i];
      v31[i] = v3[i]+v1[i];
   }
   normalize(v12);
   normalize(v23);
   normalize(v31);
   subdivide(v1, v12, v31, depth-1);
   subdivide(v2, v23, v12, depth-1);
   subdivide(v3, v31, v23, depth-1);
   subdivide(v12, v23, v31, depth-1);
}
void drawIcosahedron()
{
	  glBegin(GL_TRIANGLES);
//	  for (i = 0; i < 20; i++) {
//	        glNormal3fv(&vdata[tindices[i][0]][0]);
//	        glVertex3fv(&vdata[tindices[i][0]][0]);
//	        glNormal3fv(&vdata[tindices[i][1]][0]);
//	        glVertex3fv(&vdata[tindices[i][1]][0]);
//	        glNormal3fv(&vdata[tindices[i][2]][0]);
//	        glVertex3fv(&vdata[tindices[i][2]][0]);
//	  }
	    GLfloat d1[3], d2[3], norm[3];
	    for (int j = 0; j < 3; j++) {
	       d1[j] = vdata[tindices[i][0]][j] - vdata[tindices[i][1]][j];
	       d2[j] = vdata[tindices[i][1]][j] - vdata[tindices[i][2]][j];
	    }
	    normcrossprod(d1, d2, norm);
	    glNormal3fv(norm);
	    for (i = 0; i < 20; i++) {
	       subdivide(&vdata[tindices[i][0]][0],
	                 &vdata[tindices[i][1]][0],
	                 &vdata[tindices[i][2]][0], 3);
	    }
	  glEnd();

}


void drawPolygon(int n =20 , int arg = 0, float mult = 1, float v = 1.0)
{
//if (arg < 0)
//    arg = 0;
//
// Cylinder Bottom
glBegin(GL_POLYGON);
    glColor4f(1.0, 0.0, 0.0, 1.0);
    for(int i = arg; i <= (360 + arg); i += (360 / n)) {
        float a = i * M_PI / 180; // degrees to radians
        glVertex3f(mult * cos(a), mult * sin(a), 0.0);
    }
glEnd();

// Cylinder Top
glBegin(GL_POLYGON);
    glColor4f(0.0, 0.0, 1.0, 1.0);
    for(int i = arg; i <= (360 + arg); i += (360 / n)) {
        float a = i * M_PI / 180; // degrees to radians
        glVertex3f(mult * cos(a), mult * sin(a), v);
    }
glEnd();

// Cylinder "Cover"
glBegin(GL_QUAD_STRIP);
    glColor4f(1.0, 1.0, 0.0, 1.0);
    for(int i = arg; i < 480; i += (360 / n)) {
        float a = i * M_PI / 180; // degrees to radians
        glVertex3f(mult * cos(a), mult * sin(a), 0.0);
        glVertex3f(mult * cos(a), mult * sin(a), v);
    }
glEnd();
}


void mouseCallBack(int btn, int state, int x, int y)
{
    // Changes the rotation axis depending on the mouse button pressed.
	cout << "here is mouseCallback" << endl;
    if ( state == GLUT_DOWN )
    {
        if( btn == GLUT_LEFT_BUTTON ) {cout << "here is left button pressed" << endl; axis = 0;}
        if( btn == GLUT_MIDDLE_BUTTON ) axis = 1;
        if( btn == GLUT_RIGHT_BUTTON) axis = 2;
    }

    // Wheel reports as button 3(scroll up) and button 4(scroll down)
    if ((btn == 3) || (btn == 4)) // It's a wheel event
    {
        // Each wheel event reports like a button click, GLUT_DOWN then GLUT_UP
        if (state == GLUT_UP) return; // Disregard redundant GLUT_UP events
        	printf("Scroll %s At %d %d\n", (btn == 3) ? "Up" : "Down", x, y);
        if (btn == 3)
            rotate_x += .05;
        if (btn == 4)
            rotate_x -= .05;
    }else{  // normal button event
        printf("Button %s At %d %d\n", (state == GLUT_DOWN) ? "Down" : "Up", x, y);
    }

}


void motionCallBack(int x, int y)
{
    int movx, movy;

    movx = lastx - x;
    movy = lasty - y;

    if ( abs(movx) < 10 ){ rotate_speed += (float)(movx) / 50.0f; }
    if ( abs(movy) < 10 ){ scaling  += (float)(movy) / 100.0f;  }

    if ( scaling < 00.1f) scaling = 1.f;
    if ( scaling > 10.0f) scaling = 10.0f;

    lastx = x;
    lasty = y;



}

void draw_sphere()
{


}
void drawCone()
{
	float base = 3.0;
	float height = 30.0;
	float slices = 10.0;
	float stacks = 20.0;

	glutSolidCone(base,height,slices,stacks);
}
void drawCylinder()
{
	float top = 3.0;
	float base = 3.0;
	float height = 30.0;
	float slices = 10.0;
	float stacks = 20.0;

//	GLUquadricObj *quadric // Create pointer for storage space for object

	//example
	GLUquadricObj *Cylinder; // Create pointer for our cylinder

	Cylinder = gluNewQuadric(); // Create our new quadric object
	gluQuadricDrawStyle( Cylinder, GLU_FILL); //FILL also can be line(wire)
//	gluQuadricNormals( Cylinder, GLU_SMOOTH); // For if lighting is to be used.
	gluQuadricOrientation( Cylinder,GLU_OUTSIDE);
//	gluQuadricTexture( Cylinder, GL_TRUE);// if you want to map a texture to it.

//	gluCylinder(gluNewQuadric(), base, top, height, slices, stacks);
	gluCylinder(Cylinder, base, top, height, slices, stacks);
}

void drawArtery(int i);

void myInit(void)
{
  GLfloat light_ambient[] =
  {0.2, 0.2, 0.2, 1.0};
  GLfloat light_diffuse[] =
  {1.0, 1.0, 1.0, 1.0};
  GLfloat light_specular[] =
  {1.0, 1.0, 1.0, 1.0};
  GLfloat light_position[] =
  {0.0, 0.0, 200.0, 0.0};

  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  glShadeModel(GL_SMOOTH);

  glEnable(GL_LIGHT0);
  glDepthFunc(GL_LESS);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);

  glSelectBuffer(BUFSIZE, selectBuf);

  for (int i = 1; i < 2; i++)
  {
	  glNewList(i, GL_COMPILE);
	  drawArtery(i);
	  glEndList();
  }
  average_x = sum_x/(point_number/3.0);
  average_y = sum_y/(point_number/3.0);
  average_z = sum_z/(point_number/3.0);
  cout << "draw: average_x: "<< average_x << ",average_y: " << average_y << ",average_z: " << average_z;

//
//  glNewList(GALLBLADDER, GL_COMPILE);
//  drawArtery(2);
//  glEndList();
//  glNewList(LIVER, GL_COMPILE);
//  //glutSolidTorus(0.275, 0.85, 10, 15);
////  drawCylinder();
//  glEndList();
//
//  glNewList(PANCREAS, GL_COMPILE);
////  glutSolidTetrahedron();
////  glutSolidSphere(4.0, 16, 16);
////  drawPolygon();
//  glEndList();
//  glNewList(PORTALVEIN, GL_COMPILE);
////  glutSolidTetrahedron();
////  glutSolidSphere(4.0, 16, 16);
////  drawPolygon();
//  glEndList();

  glNewList(STOMACH, GL_COMPILE);
  //glutSolidIcosahedron();
  glColor3f(1.0,0.0,1.0);
  glutSolidSphere(3.0, 16, 16);
  glEndList();

  glNewList(LIGHTSTICK, GL_COMPILE);
  //glutSolidIcosahedron();
  glutSolidSphere(2.0, 16, 16);
  glEndList();

//  glNewList(CYLINDER, GL_COMPILE);
////  drawCylinder();
//  glutSolidSphere(2.0, 16, 16);
//  glEndList();

  glNewList(INSTRUMENT1, GL_COMPILE);
  //glutSolidIcosahedron();
  	  glutSolidCube(2.0);
  glEndList();

  glNewList(INSTRUMENT2, GL_COMPILE);
  //glutSolidIcosahedron();
  	  glutSolidCube(2.0);
  glEndList();

  glNewList(NODEBALL, GL_COMPILE);
  //glutSolidIcosahedron();
	  glColor3f(0.5,0.5,0.5);
	  glutSolidSphere(3.0, 100, 100);
  glEndList();

  glNewList(LIGHTARROW, GL_COMPILE);
  	  drawCone();
  glEndList();

  glNewList(CLIP, GL_COMPILE);
  	  glutSolidCube(2.0);
  glEndList();

//
//
//  //glutSolidIcosahedron();
//  //drawIcosahedron();

}
void init()
{
//	GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
//	GLfloat mat_shininess[] = {50.0};
//
//	GLfloat light_ambient[] = {0.2, 0.2, 0.2, 1.0};
//	GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
//	GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};
//	GLfloat light_position[] = {0.0, 0.0, 60.0, 1.0};
//	GLfloat spot_direction[] = {-1.0, -1.0, 0.0};
//
//	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
//	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
//
//	glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
//	glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
//	glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);
//
//	glLightfv (GL_LIGHT0, GL_POSITION, light_position);
//
//	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 2.0);
//	glLightf (GL_LIGHT0, GL_LINEAR_ATTENUATION, 1.0);
//	glLightf (GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.5);
//
//	glLightf (GL_LIGHT0, GL_SPOT_CUTOFF, 45.0);
//	glLightfv (GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
//	glLightf (GL_LIGHT0, GL_SPOT_EXPONENT, 2.0);
//
//
//	glShadeModel(GL_SMOOTH);
//	glEnable(GL_LIGHT0);
//	glEnable(GL_LIGHTING);

}
/*
void main_loop()
{
	static double angle;

	GLdouble eqn[4] = {0.0, 1.0, 0.0, 0.0};
	GLdouble eqn2[4]={1.0, 0.0, 0.0, 0.0};


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(0,1,0,1);

	glColor3f(0.5,0.0,0.0);



	glLoadIdentity();

	// light

	gluLookAt(0.0-2*scaling,10.0,60.0+2 * scaling, 0.0, 0.0, 0.0, 0,1,0);

	glColor3f(0.5,0.5,0.0);
	glBegin(GL_LINES);
		glVertex3f(0,0,0);
		glVertex3f(500,0,0);
		glVertex3f(0,0,0);
		glVertex3f(0,500,0);
		glVertex3f(0,0,0);
		glVertex3f(0,0,500);
	glEnd();

// vessels
	glPushMatrix();
	glTranslatef(0,15,0);
//	glClipPlane(GL_CLIP_PLANE0, eqn);
//	glEnable(GL_CLIP_PLANE0);
	glRotatef(90, 1, 0, 0);
	drawCylinder();


	// vessel label
	char text[] = "Vessel - cylinder";

	float x = 0, y = 20, z = 0;
	// The color, red for me
	glColor3f(1, 0, 0);
	// Position of the text to be printer
	glRasterPos3f(x, y, z);

	for(int i = 0; text[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);

	glPopMatrix();

	// organs
	// stomach
	glPushMatrix();
	glTranslatef(5, -7, 15);
	glRotatef(45, 0, 0, 1.0);
	glScalef(2.0, 1.0, 1.0);
	glutWireSphere(4.0, 16, 16);

	// stomach label
	char text4[] = "stomach: ellipsoid";
	x = 5, y = -5, z = 0;
	// The color, red for me
	glColor3f(1, 0, 0);
	// Position of the text to be printer
	glRasterPos3f(x, y, z);
	for(int i = 0; text4[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text4[i]);

	glPopMatrix();

	// liver
	glPushMatrix();
	glTranslatef(-10,0, 0);
	glRotatef(0, 0, 0, 1.0);
	glScalef(2.0, 1.0, 1.0);
	glutWireSphere(2.0, 16, 16);

	// liver label
	char text5[] = "liver: ellipsoid";
	x = -10, y = 5, z = 0;
	// The color, red for me
	glColor3f(1, 0, 0);
	// Position of the text to be printer
	glRasterPos3f(x, y, z);
	for(int i = 0; text5[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text5[i]);

	glPopMatrix();

	// instruments

	// instrument1
	glPushMatrix();
	glTranslatef(-10, -10, 10);
	glRotatef(45, 1, 1, 1.0);
	glScalef(3.0, 1.0, 1.0);
	glutWireCube(2.0);

	// instrument1 label
	char text2[] = "instrument1-stick";
	x = 10, y = 10, z = 0;
	// The color, red for me
	glColor3f(1, 0, 0);
	// Position of the text to be printer
	glRasterPos3f(x, y, z);
	for(int i = 0; text2[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text2[i]);

	glPopMatrix();

	// instrument2
	glPushMatrix();
	glTranslatef(10,10, 5);
	glRotatef(30, 1,1, 1.0);
	glScalef(2.0, 1.0, 1.0);
	glutWireCube(2.0);

	// instrument2 label
	char text3[] = "instrument2 - stick";
	x = -20, y = -15, z = 0;
	// The color, red for me
	glColor3f(1, 0, 0);
	// Position of the text to be printer
	glRasterPos3f(x, y, z);
	for(int i = 0; text3[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text3[i]);

	glPopMatrix();


//	glBegin(GL_QUADS);
//
//	glEnd();
	glutSwapBuffers();
	angle += 0.1;
}
*/
void draw_front();
void display();

void GL_Setup(int a, int b)
{
	W = a;
	H = b;

	glMatrixMode(GL_PROJECTION);
	glEnable(GL_DEPTH_TEST);
	gluPerspective(45, (float)a/b, 1.1, 1000);
	glMatrixMode(GL_MODELVIEW);

}

/*
void myReshape(int w, int h)
{
  W = w;
  H = h;
  glViewport(0, 0, W, H);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  //myortho();
  glMatrixMode(GL_MODELVIEW);
}
*/

void control_mode(int value)
{
	  switch (value) {
	  case 3:
	    glEnable(GL_LIGHTING);
	    glDisable(GL_BLEND);
	    glEnable(GL_DEPTH_TEST);
	    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	    break;
	  case 4:
	    glDisable(GL_LIGHTING);
	    glColor3f(1.0, 1.0, 1.0);
	    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	    glEnable(GL_LINE_SMOOTH);
	    glEnable(GL_BLEND);
	    glDisable(GL_DEPTH_TEST);
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	    break;
	  }
	  glutPostRedisplay();
}

void polygon_mode(int value)
{
  switch (value) {
  case 1:
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    break;
  case 2:
    glDisable(GL_LIGHTING);
    glColor3f(1.0, 1.0, 1.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    break;
  }
  glutPostRedisplay();
}



void highlightBegin(void)
{
  static GLfloat red[4] =
  {1.0, 1.0, 0.0, 1.0};

  glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
  glColor3f(1.0, 1.0, 0.0);
}

void highlightEnd(void)
{
	glPopAttrib();
}

//////////////////////
// load the cognitive vascular model
/////////////////////
graph<vertex, edge> vascular;//= new graph<vertex, edge>();
stack<string> path;
ostringstream start, end;

void loadCVModel() {

	//vessels->add();


	char ch = '0';
	// add nodes
	cout << " adding vertices ..." << endl;

	for (int i = 0; i < 13; i++)
	{
		vertex aNode;

		ostringstream convert;   // stream used for the conversion

		convert << i;      // insert the textual representation of 'Number' in the characters in the stream

		aNode.label = convert.str(); // set 'Result' to the contents of the stream

		aNode.x = nodes[i][0];
		aNode.y = nodes[i][1];
		aNode.z = nodes[i][2];

		vascular.addVertex(aNode);
	}
	// add links
	cout << " adding links ..." << endl;
	for (int i = 0; i < 12; i++)
	{
//		cout << i << endl;
		// add links
		edge aLink;
		ostringstream convert1, convert2;   // stream used for the conversion

//		cout << edges[i][0];
		convert1 << edges[i][0];      // insert the textual representation of 'Number' in the characters in the stream
		aLink.start_node = vascular.getVertexByLabel(convert1.str());
		convert1.flush();
		aLink.start_node->links.push_back(aLink); // insert this edge to which the vertex is connected
//		cout << edges[i][1];
		convert2 << edges[i][1]; //
		aLink.end_node = vascular.getVertexByLabel(convert2.str());
		aLink.end_node->links.push_back(aLink); //insert this edge to which the vertex is connected
		aLink.start_node->neighbors.push_back(*(aLink.end_node));
		aLink.end_node->neighbors.push_back(*(aLink.start_node));



		vascular.addEdge(aLink);
	}



//
//		{0,1}, {1,2}, {2,3}, // aorta: shortest (v0, v3),
//		{1,4},{4,7},//CT: shortest path(v1, v4),
//		{4,5}, {5,6},//LGA:shortpath(v4, v6)
//		{7,8},//CHA: shortest path(v4, v7)
//		{8,9},// LHA: shortest path(v7, v8)
//		{7,10},{10,11}, // SpA: shortest path(v4, v10)
//		{2, 12}// SMA: shortest path(v2v11)	//


	cout << "Input the start vessel node: ";
//	cin >> sLabel;
	cout << "Input the end vessel node:";
//	cin >> eLabel;

	start << sLabel; end << eLabel;


//	vascular.shortestPath(sLabel);
	path = vascular.path(start.str(), end.str());
	cout << "path size: " << path.size() << "\n";

	cout << "Distance from " << sLabel << " to " << eLabel << " is " << path.size() -1
			<< " and the path is " ;

	while(!path.empty())
	{
		cout << "[" << (path.top())<<"],";
	    path.pop();
	}

	cout << "the end" << endl;
//	while(ch != 'q')
//	{
//		cin >> ch;
//	}


	return;
}

void draw_cognitive(void)
{
	static double angle;

	GLdouble eqn[4] = {0.0, 1.0, 0.0, 0.0};
	GLdouble eqn2[4]={1.0, 0.0, 0.0, 0.0};



//	glClearColor(0,1,0,1);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//	glColor3f(0.5,0.0,0.0);

	glPushMatrix();


	glLoadIdentity();
	/* light */
//	cout << average_x << average_y << average_z << endl;
//	gluLookAt(0.0+shift, -500.0, 0.0+scaling, average_x, average_y, average_z, 0,1,0);
    glRotatef(  rotate_x,-1.0f, 1.5f, -5.0f );
	gluLookAt(-20.0-2*scaling,-500.0,0.0+2 * scaling, 0.0, 0.0, 0.0, 0,0,1);

//	glColor3f(0.5,0.5,0.0);
//	glTranslatef(0,0,-50);


	glPushMatrix();
		glLineWidth(1.);
		glColor3f(i*0.05, 1, 1);
		glBegin(GL_LINES);
			for (int i = 0; i < 12; i++)
			{
				glVertex3f(nodes[edges[i][0]][0],nodes[edges[i][0]][1], nodes[edges[i][0]][2]);
				glVertex3f(nodes[edges[i][1]][0],nodes[edges[i][1]][1], nodes[edges[i][1]][2]);
			}
		glEnd();
	glPopMatrix();

	glLineWidth(2.0);
	float x, y, z;
	// Nodes
	for (int i = 0; i < 13; i++)
	{
		////////////////
		// node ball
		///////////////
		glPushMatrix();
		glLoadName(NODEBALL);
		if (theObject == NODEBALL)
			highlightBegin();

		x = nodes[i][0], y = nodes[i][1], z = nodes[i][2];

		glTranslatef(x, y, z);
	//	//  if (!menu_inuse) {
	//		if (axis == 0) {
			  if (theObject == NODEBALL) {
	//			theObject = 1;
	//	//		glRotatef(90, 0, 1, 0);
	//			glRotatef((pow(rotate_speed,2)+pow(scaling,2)), rotate_speed, scaling, 0);
	//	//		glutPostRedisplay();
			  }
	//		  axis = 2;
	//		}
	//	//  }
	//	glRotatef(90, 1, 0, 0);
		glScalef(2.0, 2.0, 2.0);
	//	if(menu_inuse)
	//	{
	//		glPushMatrix();
	//		glRotatef(sqrt(pow(rotate_speed,2)+pow(scaling,2)), rotate_speed, scaling, 0);
	//		glPopMatrix();
	//	}
		glColor3f(i*0.05, 0.3, 0.3);
		glCallList(NODEBALL);
		glPopMatrix();

		glPushMatrix();

		/* vessel label */
		string str_text;
		char text[] = "v";
		ostringstream index, text2;


		index << i;
		text2 << text;
		str_text = text2.str() + index.str();

		x = (nodes[i][0])+2., y = nodes[i][1], z = nodes[i][2];
		// The color, red for me
		glColor3f(1, 0, 0);
		// Position of the text to be printer
		glRasterPos3f(x, y, z);
		for(int j = 0; str_text[j] != '\0'; j++)
		{
//			cout << "str_text:" << str_text[j] << end;
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str_text[j]);
		}


		if (theObject == NODEBALL)
			highlightEnd();
		glPopMatrix();

	}


	path = vascular.path(start.str(), end.str());

	// path
	glPushMatrix();
		glLineWidth(10.);
		glColor3f(1, 0, 0);
		glBegin(GL_LINE_STRIP);
		while(!path.empty())
		{
			char a;
			a = (char)(*path.top().c_str());
			int i = a - '0';
	//		cout << "i : " << i << endl;
			glVertex3f(nodes[i][0],nodes[i][1], nodes[i][2]);
			path.pop();
		}
		glEnd();
	glPopMatrix();
	/* vessels */
//	glPushMatrix();
//	glClipPlane(GL_CLIP_PLANE0, eqn);
//	glEnable(GL_CLIP_PLANE0);
//	glRotatef(90, 1, 0, 0);
//	drawCylinder();
	glLineWidth(1.0);

//	////////////////
//	// Arrow
//	///////////////
//	glLoadName(LIGHTARROW);
//	glPushMatrix();
//	if (theObject == LIGHTARROW)
//		highlightBegin();
//	glTranslatef(0,0, 5);
////	glRotatef(1, 1,1, 1.0);
//	glScalef(10.0, 30.0, 1.0);
//
//	/* instrument2 label */
//	char text9[] = "Light";
//	x = -20, y = -15, z = 0;
//	// The color, red for me
//	glColor3f(1, 0, 0);
//	// Position of the text to be printer
//	glRasterPos3f(x, y, z);
//	for(int i = 0; text9[i] != '\0'; i++)
//		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text9[i]);
//
//	glCallList(LIGHTARROW);
//	if (theObject == LIGHTARROW)
//		highlightEnd();
//	glPopMatrix();
//
//	////////////////
//	// Cyliner
//	///////////////
//	glLoadName(LIGHTSTICK);
//	glPushMatrix();
//	if (theObject == LIGHTSTICK)
//		highlightBegin();
//	glTranslatef(0,0, 5);
////	glRotatef(1, 1,1, 1.0);
//	glScalef(10.0, 30.0, 1.0);
//
//	glCallList(LIGHTSTICK);
//	if (theObject == LIGHTSTICK)
//		highlightEnd();
//	glPopMatrix();

		////////////////
		// CLIP1
		///////////////
		glLoadName(CLIP);
		glPushMatrix();
		if (theObject == CLIP)
			highlightBegin();
		glTranslatef(10,-27, 30);
	//	glRotatef(1, 1,1, 1.0);
		glScalef(10.0, 30.0, 1.0);

		glCallList(CLIP);
		if (theObject == CLIP)
			highlightEnd();
		glPopMatrix();

		////////////////
		// CLIP2
		///////////////
		glLoadName(CLIP);
		glPushMatrix();
		if (theObject == CLIP)
			highlightBegin();
		glTranslatef(5,-22, 30);
	//	glRotatef(1, 1,1, 1.0);
		glScalef(10.0, 30.0, 1.0);

		glCallList(CLIP);
		if (theObject == CLIP)
			highlightEnd();
		glPopMatrix();

//		////////////////
//		// CLIP3
//		///////////////
//		glLoadName(CLIP);
//		glPushMatrix();
//		if (theObject == CLIP)
//			highlightBegin();
//		glTranslatef(0,0, 5);
//	//	glRotatef(1, 1,1, 1.0);
//		glScalef(10.0, 30.0, 1.0);
//
//		glCallList(CLIP);
//		if (theObject == CLIP)
//			highlightEnd();
//		glPopMatrix();
//
//		////////////////
//		// CLIP4
//		///////////////
//		glLoadName(CLIP);
//		glPushMatrix();
//		if (theObject == CLIP)
//			highlightBegin();
//		glTranslatef(0,0, 5);
//	//	glRotatef(1, 1,1, 1.0);
//		glScalef(10.0, 30.0, 1.0);
//
//		glCallList(LIGHTSTICK);
//		if (theObject == CLIP)
//			highlightEnd();
//		glPopMatrix();

	glPopMatrix();


}
void draw_front(void)
{

	glClearColor(0.5,0.5,0.5,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glPushMatrix();
	glLoadIdentity();
    glRotatef(  rotate_x,-1.0f, 1.5f, -5.0f );
    glRotatef(  rotate_x,-1.0f, 1.5f, -5.0f );
    gluLookAt(0.0+shift, -300.0, 20+scaling+average_z, average_x, average_y, average_z, 0,0,1);


	/////////////////////
	// STOMACH
	////////////////////
	glLoadName(STOMACH);
	glPushMatrix();
		if (theObject == STOMACH)
			highlightBegin();
		//  if (!menu_inuse) {
	//		if (mouse_state == GLUT_LEFT) {
			  if (theObject == STOMACH) {
	//			theObject = 2;
	//			glRotatef(90, 0, 1, 0);
				glRotatef((pow(rotate_speed,2)+pow(scaling,2)), rotate_speed, scaling, 0);
		//		glutPostRedisplay();
			  }
	//		}
		//  }

		glTranslatef(50,-20, -50);
		glRotatef(0, 0, 0, 1.0);
		glScalef(10.0, 10.0, 10.0);
		glColor3f(0.6,0.8,0.8);             // Set color to yellow
//		glutSolidSphere(3,100,100);	     // Draw Sphere
	//	glCallList(STOMACH);
		/* stomach label */
		char text6[] = "stomach";
		float x, y, z;
		 x = -10, y = 5, z = 0;
		// The color, red for me
		glColor3f(1, 0, 0);
		// Position of the text to be printer
		glRasterPos3f(x, y, z);
		for(int i = 0; text6[i] != '\0'; i++)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text6[i]);


		if (theObject == STOMACH)
			highlightEnd();
	glPopMatrix();

	////////////////
	// Artery
	///////////////
	glLoadName(ARTERY);
	glPushMatrix();
		if (theObject == ARTERY)
			highlightBegin();

	//	glTranslatef(0,15,0);
	//	//  if (!menu_inuse) {
	//		if (axis == 0) {
			  if (theObject == ARTERY) {
	//			theObject = 1;
	//	//		glRotatef(90, 0, 1, 0);
	//			glRotatef((pow(rotate_speed,2)+pow(scaling,2)), rotate_speed, scaling, 0);
	//	//		glutPostRedisplay();
			  }
	//		  axis = 2;
	//		}
	//	//  }
	//	glRotatef(90, 1, 0, 0);


	//	if(menu_inuse)
	//	{
	//		glPushMatrix();
	//		glRotatef(sqrt(pow(rotate_speed,2)+pow(scaling,2)), rotate_speed, scaling, 0);
	//		glPopMatrix();
	//	}
		glCallList(ARTERY);
	glPopMatrix();
	glPushMatrix();
		/* vessel label */
		char text[] = "Artery";
		x = 0, y = 20, z = 0;
		// The color, red for me
		glColor3f(1, 0, 0);
		// Position of the text to be printer
		glRasterPos3f(x, y, z);
		for(int i = 0; text[i] != '\0'; i++)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);


		/* vessel label from srsml */
		string str_text;
		str_text = str_elements + str_elements2;
		x = 0, y = 22, z = 0;
		// The color, red for me
		glColor3f(0, 1, 0);
		// Position of the text to be printer
		glRasterPos3f(x, y, z);

		for(int i = 0; str_text[i] != '\0'; i++)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str_text[i]);


		if (theObject == ARTERY)
			highlightEnd();
	glPopMatrix();


	//////////////
	// Gallbladder
	/////////////
	glLoadName(GALLBLADDER);
	glPushMatrix();
		if (theObject == GALLBLADDER)
			highlightBegin();
		//glTranslatef(0,40,20);
		//  if (!menu_inuse) {
	//		if (mouse_state == GLUT_LEFT) {
			  if (theObject == GALLBLADDER) {
	//			theObject = 2;
	//			glRotatef(90, 0, 1, 0);
	//			glRotatef((pow(rotate_speed,2)+pow(scaling,2)), rotate_speed, scaling, 0);
		//		glutPostRedisplay();
			  }
	//		}
		//  }
		//glRotatef(45, 0, 0, 1.0);
		//glScalef(6.0, 3.0, 3.0);
		/* stomach label */
		char text2[] = "GALLBLADDER";
		 x = 5, y = -5, z = 0;
		// The color, red for me
		glColor3f(1, 0, 0);
		// Position of the text to be printer
		glRasterPos3f(x, y, z);
		for(int i = 0; text2[i] != '\0'; i++)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text2[i]);

		glCallList(GALLBLADDER);
		if (theObject == GALLBLADDER)
			highlightEnd();
	glPopMatrix();

	//////////////
	// Liver
	/////////////
	glLoadName(LIVER);
		glPushMatrix();
		if (theObject == LIVER)
			highlightBegin();
		//glTranslatef(0,40,20);
		//  if (!menu_inuse) {
	//		if (mouse_state == GLUT_LEFT) {
			  if (theObject == LIVER) {
	//			theObject = 2;
	//			glRotatef(90, 0, 1, 0);
				glRotatef((pow(rotate_speed,2)+pow(scaling,2)), rotate_speed, scaling, 0);
		//		glutPostRedisplay();
			  }
	//		}
		//  }
		/* liver label */
		char text3[] = "liver";
		x = 5, y = -5, z = 0;
		// The color, red for me
		glColor3f(1, 0, 0);
		// Position of the text to be printer
		glRasterPos3f(x, y, z);
		for(int i = 0; text3[i] != '\0'; i++)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text3[i]);

		glCallList(LIVER);
		if (theObject == LIVER)
			highlightEnd();
	glPopMatrix();

	//////////////
	// PANCREAS
	/////////////
	glLoadName(PANCREAS);
	glPushMatrix();
		if (theObject == PANCREAS)
			highlightBegin();
		//  if (!menu_inuse) {
	//		if (mouse_state == GLUT_LEFT) {
			  if (theObject == PANCREAS) {
	//			theObject = 2;
	//			glRotatef(90, 0, 1, 0);
	//			glRotatef((pow(rotate_speed,2)+pow(scaling,2)), rotate_speed, scaling, 0);
		//		glutPostRedisplay();
			  }
	//		}
		//  }
		/* pancreas label */
		char text4[] = "pancreas";
		x = 5, y = -5, z = 0;
		// The color, red for me
		glColor3f(1, 0, 0);
		// Position of the text to be printer
		glRasterPos3f(x, y, z);
		for(int i = 0; text4[i] != '\0'; i++)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text4[i]);

		glCallList(PANCREAS);
		if (theObject == PANCREAS)
			highlightEnd();
	glPopMatrix();

	//////////////
	// Portal vein
	/////////////
	glLoadName(PORTALVEIN);
	glPushMatrix();
	if (theObject == PORTALVEIN)
		highlightBegin();
	//  if (!menu_inuse) {
//		if (mouse_state == GLUT_LEFT) {
		  if (theObject == PORTALVEIN) {
//			theObject = 2;
//			glRotatef(90, 0, 1, 0);
			glRotatef((pow(rotate_speed,2)+pow(scaling,2)), rotate_speed, scaling, 0);
	//		glutPostRedisplay();
		  }
//		}
	//  }
	/* stomach label */
	char text5[] = "potal vein";
	x = 5, y = -5, z = 0;
	// The color, red for me
	glColor3f(1, 0, 0);
	// Position of the text to be printer
	glRasterPos3f(x, y, z);
	for(int i = 0; text5[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text5[i]);

	glCallList(PORTALVEIN);
	if (theObject == PORTALVEIN)
		highlightEnd();
	glPopMatrix();

//	/////////////////////
//	// LIGHTSTICK
//	////////////////////
//	glLoadName(LIGHTSTICK);
//	glPushMatrix();
//	if (theObject == LIGHTSTICK)
//		highlightBegin();
//	//  if (!menu_inuse) {
////		if (mouse_state == GLUT_LEFT) {
//		  if (theObject == LIGHTSTICK) {
////			theObject = 2;
////			glRotatef(90, 0, 1, 0);
//			glRotatef((pow(rotate_speed,2)+pow(scaling,2)), rotate_speed, scaling, 0);
//	//		glutPostRedisplay();
//		  }
////		}
//	//  }
//
//	glTranslatef(50,-20, -100);
//	glRotatef(0, 0, 0, 1.0);
//	glScalef(10.0, 10.0, 10.0);
//	glCallList(LIGHTSTICK);
//
//	/* stomach label */
//	char text10[] = "stick";
//	x = 70, y = -20, z = -100;
//	// The color, red for me
//	glColor3f(1, 0, 0);
//	// Position of the text to be printer
//	glRasterPos3f(x, y, z);
//	for(int i = 0; text10[i] != '\0'; i++)
//		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text10[i]);
//
//	if (theObject == LIGHTSTICK)
//		highlightEnd();
//	glPopMatrix();

	////////////////
	// Instrument 1
	////////////////
	glLoadName(INSTRUMENT1);
	glPushMatrix();
	if (theObject == INSTRUMENT1)
		highlightBegin();

	glTranslatef(70, -30, -30);
	glRotatef(45, 1, 1, 1.0);

	glScalef(30.0, 5.0, 5.0);
	/* instrument1 label */
	char text7[] = "instrument1-stick";
	x = 10, y = 10, z = 0;
	// The color, red for me
	glColor3f(1, 0, 0);
	// Position of the text to be printer
	glRasterPos3f(x, y, z);
	for(int i = 0; text7[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text7[i]);

	glCallList(INSTRUMENT1);
	if (theObject == INSTRUMENT1)
		highlightEnd();
	glPopMatrix();



	////////////////
	// Instrument 2
	///////////////
	glLoadName(INSTRUMENT2);
	glPushMatrix();
	if (theObject == INSTRUMENT2)
		highlightBegin();
	glTranslatef(-70,-40, -20);
	glRotatef(30, 1,1, 1.0);
	glScalef(30.0, 5.0, 5.0);

	/* instrument2 label */
	char text8[] = "instrument2 - stick";
	x = -20, y = -15, z = 0;
	// The color, red for me
	glColor3f(1, 0, 0);
	// Position of the text to be printer
	glRasterPos3f(x, y, z);
	for(int i = 0; text8[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text8[i]);

	glCallList(INSTRUMENT2);
	if (theObject == INSTRUMENT2)
		highlightEnd();
	glPopMatrix();

	////////////////
	// CLIP1
	///////////////
	glLoadName(CLIP);
		glPushMatrix();
		if (theObject == CLIP)
			highlightBegin();
		glTranslatef(10,-55,-80);
	//	glRotatef(1, 1,1, 1.0);
		glScalef(3.0, 30.0, 1.0);

		glCallList(CLIP);
		if (theObject == CLIP)
			highlightEnd();
	glPopMatrix();

	glLoadName(CLIP);
		glPushMatrix();
		if (theObject == CLIP)
			highlightBegin();
		glTranslatef(15,-50,-80);
	//	glRotatef(1, 1,1, 1.0);
		glScalef(5.0, 30.0, 1.0);

		glCallList(CLIP);
		if (theObject == CLIP)
			highlightEnd();
	glPopMatrix();
//	////////////////
//	// Arrow
//	///////////////
//	glLoadName(LIGHTARROW);
//	glPushMatrix();
//	if (theObject == LIGHTARROW)
//		highlightBegin();
//	glTranslatef(0,0, 5);
////	glRotatef(1, 1,1, 1.0);
//	glScalef(10.0, 30.0, 1.0);
//
//	/* light label */
//	char text9[] = "Light";
//	x = -20, y = -15, z = 0;
//	// The color, red for me
//	glColor3f(1, 0, 0);
//	// Position of the text to be printer
//	glRasterPos3f(x, y, z);
//	for(int i = 0; text9[i] != '\0'; i++)
//		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text9[i]);
//
//	glCallList(LIGHTARROW);
//	if (theObject == LIGHTARROW)
//		highlightEnd();
//	glPopMatrix();


	glPopMatrix();



}

GLdouble SCALE = 50;

void myortho(void)
{

  if (W <= H)
    glOrtho(-SCALE, SCALE, SCALE * (GLfloat) H / (GLfloat) W,
    		-SCALE * (GLfloat) H / (GLfloat) W, -SCALE, SCALE);
  else
    glOrtho(-1*SCALE * (GLfloat) W / (GLfloat) H,
    		SCALE * (GLfloat) W / (GLfloat) H, -1*SCALE, SCALE, -1*SCALE, SCALE);
}

void mypers(void)
{
	gluPerspective(45, (float)W/H, 1.1, 1000);
}
/*  processHits() prints out the contents of the
 *  selection array.
 */
void processHits(GLint hits, GLuint buffer[])
{
  GLuint depth = ~0;
  unsigned int i, getThisName;
  GLuint names, *ptr;
  GLuint newObject;

  ptr = (GLuint *) buffer;
  newObject = 0;
  for (i = 0; i < hits; i++) {  /* for each hit  */
    getThisName = 0;
    names = *ptr;
    ptr++;              /* skip # name */
    if (*ptr <= depth) {
      depth = *ptr;
      getThisName = 1;
    }
    ptr++;              /* skip z1 */
    if (*ptr <= depth) {
      depth = *ptr;
      getThisName = 1;
    }
    ptr++;              /* skip z2 */

    if (getThisName)
      newObject = *ptr;
    ptr += names;       /* skip the names list */
  }
  if (theObject != newObject) {
    theObject = newObject;
    glutPostRedisplay();
  }
}

void output(GLfloat x, GLfloat y, char *format,...)
{
  va_list args;
  char buffer[200], *p;

  va_start(args, format);
  vsprintf(buffer, format, args);
  va_end(args);
  glPushMatrix();
  glTranslatef(x, y, 0);
  for (p = buffer; *p; p++)
    glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
  glPopMatrix();
}


void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//  glPopMatrix();

  /* left view */
  glViewport(0,0, W/2, H);

  glColor3f(0.0,0.0,0.5);
  draw_front();


  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glDisable(GL_LINE_SMOOTH);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, 3000, 0, 3000);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
//  gluLookAt(0.0-2*scaling,10.0,60.0+2 * scaling, 0.0, 0.0, 0.0, 0,1,0);

  output(80, 2800, "Automatically names object under mouse.");
  output(80, 100, "Located: %s.", objectNames[theObject]);
  output(80, 200, "First Model: %s\n",(char*) elements);

//  output(80, 200, "First Model: %s", (const char*) (((root_node)->first_node("models"))->first_attribute("name"))->value());
//  output(80, 200, "First Model: %s", (const char*) (((root_node->last_node())->last_node())->first_attribute("name"))->value());
//  output(80, 200, "First Model: %s.", (const char*) root_node->first_node("Brewery")->first_node("Beer")->first_attribute("name")->value());
  glPopMatrix();

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  glMatrixMode(GL_MODELVIEW);
  glPopAttrib();

  /* right view */

  glViewport(W/2,0, W/2, H);
  draw_cognitive();
//  draw_front();
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glDisable(GL_LINE_SMOOTH);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, 3000, 0, 3000);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
//  gluLookAt(0.0-2*scaling,10.0,60.0+2 * scaling, 0.0, 0.0, 0.0, 0,1,0);

  output(80, 2800, "Automatically names object under mouse.");
  output(80, 100, "Located: %s.", objectNames[theObject]);
  output(80, 200, "First Model: %s\n",(char*) elements);

//  output(80, 200, "First Model: %s", (const char*) (((root_node)->first_node("models"))->first_attribute("name"))->value());
//  output(80, 200, "First Model: %s", (const char*) (((root_node->last_node())->last_node())->first_attribute("name"))->value());
//  output(80, 200, "First Model: %s.", (const char*) root_node->first_node("Brewery")->first_node("Beer")->first_attribute("name")->value());
  glPopMatrix();

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  glMatrixMode(GL_MODELVIEW);
  glPopAttrib();

  glutSwapBuffers();
}


/* ARGS USED */
void locate(int value)
{
  GLint viewport[4];
  GLint hits;

  if (locating) {
    if (mouse_state == GLUT_ENTERED) {
      (void) glRenderMode(GL_SELECT);
      glInitNames();
      glPushName(-1);

      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
      glLoadIdentity();
      viewport[0] = 0;
      viewport[1] = 0;
      viewport[2] = W;
      viewport[3] = H;
      gluPickMatrix(x, H - y, 5.0, 5.0, viewport);
      myortho(); // this is different from locate_pers

      glMatrixMode(GL_MODELVIEW);
      draw_cognitive();
      glMatrixMode(GL_PROJECTION);
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
      hits = glRenderMode(GL_RENDER);
    } else {
      hits = 0;
    }
    processHits(hits, selectBuf);
  }
  locating = 0;
}

/* ARGSUSED */
void locate_pers(int value)
{
  GLint viewport[4];
  GLint hits;

  if (locating) {
    if (mouse_state == GLUT_ENTERED) {
      (void) glRenderMode(GL_SELECT);
      glInitNames();
      glPushName(-1);

      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
      glLoadIdentity();
      viewport[0] = W/2;
      viewport[1] = 0;
      viewport[2] = W/2;
      viewport[3] = H;
      gluPickMatrix(x, H - y, 5.0, 5.0, viewport);
      mypers(); // this is different from the function locate above

      glMatrixMode(GL_MODELVIEW);
      draw_cognitive();
      glMatrixMode(GL_PROJECTION);
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
//      hits = glRenderMode(GL_RENDER);
    } else {
      hits = 0;
    }
    processHits(hits, selectBuf);
  }
  if (locating) {
    if (mouse_state == GLUT_ENTERED) {
//      (void) glRenderMode(GL_SELECT);
      glInitNames();
      glPushName(-1);

      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
      glLoadIdentity();
      viewport[0] = 0;
      viewport[1] = 0;
      viewport[2] = W/2;
      viewport[3] = H;
      gluPickMatrix(x, H - y, 5.0, 5.0, viewport);
      mypers(); // this is different from the function locate above

      glMatrixMode(GL_MODELVIEW);
      draw_front();
      glMatrixMode(GL_PROJECTION);
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
      hits = glRenderMode(GL_RENDER);
    } else {
      hits = 0;
    }
    processHits(hits, selectBuf);
  }

  locating = 0;
}

/* passive function for perspective view */
void passive_pers(int newx, int newy)
{
  x = newx;
  y = newy;
  if (!locating) {
    locating = 1;
    glutTimerFunc(1, locate_pers, 0);
  }
}
/* passive function for orthographic view */
void passive(int newx, int newy)
{
  x = newx;
  y = newy;
  if (!locating) {
    locating = 1;
    glutTimerFunc(1, locate, 0);
  }
}

void entry(int state)
{
  mouse_state = state;
  if (!menu_inuse) {
    if (state == GLUT_LEFT) {
      if (theObject != 0) {
        theObject = 0;
        glutPostRedisplay();
      }
    }
  }
}

void main_menu(int value)
{
  if (value == 999)
    exit(0);

  glutPostRedisplay();

}

void mstatus(int status, int newx, int newy)
{
  if (status == GLUT_MENU_NOT_IN_USE) {
    menu_inuse = 0;
    passive(newx, newy);
  } else {
    menu_inuse = 1;
  }
}



void idle()
{
	glutPostRedisplay();
}


void keyboard(int key, int x, int y)
{
	switch(key)
	{
	case GLUT_KEY_UP:
	//do something here
		scaling -= 1.0f;
		output(80, 150, "Up Arrow:");

	break;
	case GLUT_KEY_DOWN:
		scaling += 1.0f;
		output(80, 150, "Down Arrow:");
	break;
	case GLUT_KEY_LEFT:
		shift -= 1.0f;
		output(80, 150, "Down Arrow:");

	break;
	case GLUT_KEY_RIGHT:
		shift += 1.0f;
		output(80, 150, "Down Arrow:");
	break;
	}
	glutPostRedisplay();
}

void loadSRSML()
{

	cout << "parsing srsml..." << endl;


	// Read the xml file into a vector
	ifstream theFile ("gastrectomy.xml");
	vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
	buffer.push_back('\0');
	// Parse the buffer using the xml file parsing library into doc
	doc.parse<0>(&buffer[0]);
	// Find our root node
	cout << "parsing a root element" << endl;
	root_node = doc.first_node("srsml");

	int size = strlen((((root_node->first_node("models"))->first_node("vessels"))->first_attribute("name"))->value());
	int size2 = strlen((((root_node->first_node("models"))->first_node("vessels"))->first_attribute("type"))->value());

	// variables for malloc
	elements=(char*) malloc(size+1);
	elements2=(char*) malloc(size2+1);

	strcpy ((char*)elements,(char*)((((root_node->first_node("models"))->first_node("vessels"))->first_attribute("name"))->value()));
	strcpy ((char*)elements2,(char*)((((root_node->first_node("models"))->first_node("vessels"))->first_attribute("type"))->value()));

	// variables for string
	str_elements.assign((((root_node->first_node("models"))->first_node("vessels"))->first_attribute("name"))->value());
	str_elements2.assign((((root_node->first_node("models"))->first_node("vessels"))->first_attribute("type"))->value());

	// Iterate over the vessels
	if (root_node){
		cout << "if there is a root element" << endl;
		for (xml_node<> * models_node = root_node->first_node("models"); models_node!=NULL; models_node = models_node->next_sibling())
		{
			cout << "here is in the model for loop" << endl;
			for (xml_node<> * vessels_node = models_node->first_node("vessels"); vessels_node!=NULL; vessels_node = vessels_node->next_sibling())
			{
				cout << "here is in the vessels for loop" << endl;
				printf("I have visited in. ");//,
					// Interate over the beers
				cout << "here is before the inner for loop" << endl;
				for(xml_node<> * vessel_node = vessels_node->first_node("vessel"); vessel_node; vessel_node = vessel_node->next_sibling())
				{
					printf("On, I tried their %s which is a %s. ",
							vessel_node->first_attribute("name")->value(),
							vessel_node->first_attribute("type")->value());
				}
				cout << endl;
			}
		}
	}
	loadCVModel();
}

void loadXML()
{

	cout << "Parsing my beer journal..." << endl;


	// Read the xml file into a vector
	cout << "before loading a xml file" << endl;
	ifstream theFile ("beerJournal.xml");
	cout << "after loading a xml file" << endl;
	vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
	buffer.push_back('\0');
	// Parse the buffer using the xml file parsing library into doc
	doc.parse<0>(&buffer[0]);
	// Find our root node
	cout << "before parsing a root element" << endl;
	root_node = doc.first_node("MyBeerJournal");
	// Iterate over the brewerys
//	elements = "sldfjlsdjfldsjfldsjflsdjflds";
	elements=(char*) malloc(strlen(((((root_node)->first_node("Brewery"))->first_attribute("name"))->value()))+1);

	strcpy ((char*)elements,(char*)(((root_node)->first_node("Brewery"))->first_attribute("name"))->value());
	elements[strlen(((((root_node)->first_node("Brewery"))->first_attribute("name"))->value()))]='\0';
//	elements = "char";
	printf("here is element:%s\n",elements);
	cout << "after parsing a root element" << endl;
	if (root_node){
		cout << "if there is a root element" << endl;
		for (xml_node<> * brewery_node = root_node->first_node("Brewery"); brewery_node!=NULL; brewery_node = brewery_node->next_sibling())
		{
			cout << "here is in the for loop" << endl;
			printf("I have visited %s in %s. ",
				brewery_node->first_attribute("name")->value(),
				brewery_node->first_attribute("location")->value());
				// Interate over the beers
			cout << "here is before the inner for loop" << endl;
			for(xml_node<> * beer_node = brewery_node->first_node("Beer"); beer_node; beer_node = beer_node->next_sibling())
			{
				printf("On %s, I tried their %s which is a %s. ",
					beer_node->first_attribute("dateSampled")->value(),
					beer_node->first_attribute("name")->value(),
					beer_node->first_attribute("description")->value());
				printf("I gave it the following review: %s", beer_node->value());
			}
			cout << endl;
		}
	}
}


void loadPlugIn(string a)
{
	cout << "this is load Plugin: " << a << endl;
}
void loadPlugIn(double a)
{
	cout << "this is load Plugin: " << a << endl;
}

void loadModels2()
{
//	ifstream theFile2 ("plug-in.json");
	cout << "before loading a json file" << endl;

	ifstream theFile2 ("D:\\Dropbox-etri\\Dropbox\\artery_merged.json");


	cout << "after loading a json file" << endl;
	vector<char> buffer2((istreambuf_iterator<char>(theFile2)), istreambuf_iterator<char>());
	buffer2.push_back('\0');


	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;
//	bool parsingSuccessful = reader.parse(&buffer[0], root );
    std::cout  << "befor parsing configuration\n" << endl;
	bool parsingSuccessful = reader.parse(&buffer2.at(0), root );
    std::cout  << "after parsing configuration\n" << endl;
	if ( !parsingSuccessful )
	{
	    // report to the user the failure and their locations in the document.
	    std::cout  << "Failed to parse configuration\n"
	               << reader.getFormatedErrorMessages();
	    return;
	}

	// Get the value of the member of root named 'encoding', return 'UTF-8' if there is no
	// such member.
//	std::string encoding = root.get("encoding", "UTF-8" ).asString();
	// Get the value of the member of root named 'encoding', return a 'null' value if
	// there is no such member.
	const Json::Value plugins = root["vertices"];
	for ( int index = 0; index < plugins.size(); ++index )  // Iterates over the sequence elements.
	   loadPlugIn( plugins[index].asString() );

//	setIndentLength( root["indent"].get("length", 3).asInt() );
//	setIndentUseSpace( root["indent"].get("use_space", true).asBool() );

	// ...
	// At application shutdown to make the new configuration document:
	// Since Json::Value has implicit constructor for all value types, it is not
	// necessary to explicitly construct the Json::Value object:
//	root["encoding"] = getCurrentEncoding();
//	root["indent"]["length"] = getCurrentIndentLength();
//	root["indent"]["use_space"] = getCurrentIndentUseSpace();

//	Json::StyledWriter writer;
	// Make a new JSON document for the configuration. Preserve original comments.
//	std::string outputConfig = writer.write( root );

	// You can also use streams.  This will put the contents of any JSON
	// stream at a particular sub-value, if you'd like.
//	std::cin >> root["subtree"];

	// And you can write to a stream, using the StyledWriter automatically.
//	std::cout << root;

	return;
}

void PrintJSONValue( Json::Value &val )
{
    if( val.isString() ) {
        printf( "string(%s)", val.asString().c_str() );
    } else if( val.isBool() ) {
        printf( "bool(%d)", val.asBool() );
    } else if( val.isInt() ) {
        printf( "int(%d)", val.asInt() );
    } else if( val.isUInt() ) {
        printf( "uint(%u)", val.asUInt() );
    } else if( val.isDouble() ) {
        printf( "double(%f)", val.asDouble() );
    }
    else
    {
        printf( "unknown type=[%d]", val.type() );
    }
}

bool PrintJSONTree( Json::Value &root, unsigned short depth /* = 0 */)
{
    depth += 1;
    printf( " {type=[%d], size=%d}", root.type(), root.size() );

    if( root.size() > 0 ) {
        printf("\n");
        for( Json::ValueIterator itr = root.begin() ; itr != root.end() ; itr++ ) {
            // Print depth.
            for( int tab = 0 ; tab < depth; tab++) {
               printf("-");
            }
            printf(" subvalue(");
//            PrintJSONValue((Json::Value&)itr.deref());
            printf(") -");
            PrintJSONTree( *itr, depth);
        }
        return true;
    } else {
        printf(" ");
//        PrintJSONValue(root);
        printf( "\n" );
    }
    return true;
}

void drawArtery(int i)
{
	vector <Json::Value> values;
	vector<Json::Value> indices;

	////	ifstream theFile2 ("plug-in.json");
	cout << "before loading a json file" << endl;
//
	ifstream theFile2 (model_filenames[i-1].data());
	cout << model_filenames[i-1].data() << endl;
//
//
	cout << "after loading a json file" << endl;
//	vector<char> buffer2((istreambuf_iterator<char>(theFile2)), istreambuf_iterator<char>());
//	buffer2.push_back('\0');
//
//
	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;
//	bool parsingSuccessful = reader.parse(&buffer[0], root );
    std::cout  << "befor parsing configuration\n" << endl;
	bool parsingSuccessful = reader.parse(theFile2, root, false );
    std::cout  << "after parsing configuration\n" << endl;
	if ( !parsingSuccessful )
	{
	    // report to the user the failure and their locations in the document.
	    std::cout  << "Failed to parse configuration\n"
	               << reader.getFormatedErrorMessages();
	    return;
	}

	// Get the value of the member of root named 'encoding', return 'UTF-8' if there is no
	// such member.
	std::string encoding = root.get("encoding", "UTF-8" ).asString();
	// Get the value of the member of root named 'encoding', return a 'null' value if
	// there is no such member.
	const Json::Value plugins = root["version"];
	cout << "plugin.size():" << plugins.size() << endl;
	if (!plugins.size())
		loadPlugIn(plugins.asString());
	else
		for ( int index = 0; index < plugins.size(); index++ )  // Iterates over the sequence elements.
			loadPlugIn( plugins[index].asString() );

	const Json::Value vertices = root["vertices"];
	cout << "vertices.size():" << vertices.size() << endl;
	vector <double> points, normal;
	if (!vertices.size())
		cout << vertices.asDouble()<<endl;
	else
	{
		for ( int index1 = 0; index1 < vertices.size(); index1++ )  // Iterates over the sequence elements.
		{
			values.push_back( vertices[index1].get("values",0.0));
		}

//		sum_x = sum_y = sum_z = 0.0;

		for ( int index2 = 0; index2 < (values[0]).size(); index2+=3 )  // Iterates over the sequence elements.
		{
			sum_x += (values[0])[index2].asDouble();
			sum_y += (values[0])[index2+1].asDouble();
			sum_z += (values[0])[index2+2].asDouble();
		}
		point_number += (values[0]).size();

	}
	const Json::Value connects = root["connectivity"];
	cout << "connects.size():" << connects.size() << endl;
	if (!connects.size())
		cout << connects.asDouble()<<endl;
	else
	{
		for ( int index1 = 0; index1 < connects.size(); index1++ )  // Iterates over the sequence elements.
		{
			indices.push_back(connects[index1].get("indices",0.0));
		}

	    glBegin(GL_TRIANGLES);

		for (vector<Json::Value>::const_iterator index1=indices.begin(); index1!=indices.end();++index1)
		{
			cout << "indices size: " << (*index1).size() << endl;
			if (!((*index1).size()))
				cout << (*index1).asDouble()<<endl;
			else
			{
				for ( int index2 = 0; index2 < (*index1).size(); index2+=3 )  // Iterates over the sequence elements.
				{


					int i =  (*index1)[index2].asInt();
					int i2 =  (*index1)[index2+1].asInt();
					int i3 =  (*index1)[index2+2].asInt();

					// assign vertices and normals
			        glNormal3f((values[1])[i*3+0].asDouble(), (values[1])[i*3+1].asDouble(), (values[1])[i*3+2].asDouble());
			        glVertex3f((values[0])[i*3+0].asDouble(), (values[0])[i*3+1].asDouble(), (values[0])[i*3+2].asDouble());
			        glNormal3f((values[1])[i2*3+0].asDouble(), (values[1])[i2*3+1].asDouble(), (values[1])[i2*3+2].asDouble());
			        glVertex3f((values[0])[i2*3+0].asDouble(), (values[0])[i2*3+1].asDouble(), (values[0])[i2*3+2].asDouble());
			        glNormal3f((values[1])[i3*3+0].asDouble(), (values[1])[i3*3+1].asDouble(), (values[1])[i3*3+2].asDouble());
			        glVertex3f((values[0])[i3*3+0].asDouble(), (values[0])[i3*3+1].asDouble(), (values[0])[i3*3+2].asDouble());

				}
	 		}
		}
		glEnd();
	}
	values.clear();
	indices.clear();
	theFile2.clear();
	root.clear();


	return;
}

void loadJson()
{

	ifstream theFile3 ("plug-in.json");
//	ifstream theFile2 ("D:\\Dropbox-etri\\Dropbox\\artery_merged.json");


	cout << "after loading a xml file" << endl;
	vector<char> buffer3((istreambuf_iterator<char>(theFile3)), istreambuf_iterator<char>());
	buffer3.push_back('\0');


	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;
//	bool parsingSuccessful = reader.parse(&buffer[0], root );
    std::cout  << "befor parsing configuration\n" << endl;
	bool parsingSuccessful = reader.parse(&buffer3.at(0), root );
    std::cout  << "after parsing configuration\n" << endl;
	if ( !parsingSuccessful )
	{
	    // report to the user the failure and their locations in the document.
	    std::cout  << "Failed to parse configuration\n"
	               << reader.getFormatedErrorMessages();
	    return;
	}

	// Get the value of the member of root named 'encoding', return 'UTF-8' if there is no
	// such member.
	std::string encoding = root.get("encoding", "UTF-8" ).asString();
	// Get the value of the member of root named 'encoding', return a 'null' value if
	// there is no such member.
	const Json::Value plugins = root["encoding"];
	cout << "plugin.size():" << plugins.size() << endl;
	if (!plugins.size())
		loadPlugIn(plugins.asString());
	else
		for ( int index = 0; index < plugins.size(); index++ )  // Iterates over the sequence elements.
			loadPlugIn( plugins[index].asString() );

	const Json::Value indent_length = root["indent"]["length"];
	cout << "indent_length.size():" << indent_length.size() << endl;
	if (!indent_length.size())
		cout << indent_length.asInt()<<endl;


	return;
}
void createMenu()
{

    /* sub menu */
    submenu = glutCreateMenu(polygon_mode);
    glutAddMenuEntry("Filled", 1);
    glutAddMenuEntry("Wireframe", 2);

    /* sub menu2 */
   submenu2 = glutCreateMenu(control_mode);
    glutAddMenuEntry("Digging", 3);
    glutAddMenuEntry("Digging2", 4);

    /* main menu */
    glutCreateMenu(main_menu);
    glutAddMenuEntry("Quit", 999);
    glutAddSubMenu("Polygon mode", submenu);
    glutAddSubMenu("Control",submenu2);

    /* how to attach main menu */
    glutAttachMenu(GLUT_RIGHT_BUTTON);

}


int main(int argc, char ** argv)
{

    glutInit(&argc, argv);
    glutInitWindowSize(window_width, window_height); //set a window size
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE|GLUT_DEPTH);
    glutCreateWindow("SRS Viewer!!");

    //loadXML();
    loadSRSML();

    const clock_t begin_time = clock();
    myInit();
    // do something
    std::cout << "Loading models in json took :" << float( clock () - begin_time ) /  CLOCKS_PER_SEC << endl;

    glutDisplayFunc(display);
    //glutReshapeFunc(myReshape);
    glutReshapeFunc(GL_Setup);
    glutIdleFunc(idle);

//    glutIdleFunc(main_loop);

    glutMouseFunc(mouseCallBack);
    glutMotionFunc(motionCallBack);


    glutPassiveMotionFunc(passive_pers);
    glutEntryFunc(entry);
    glutMenuStatusFunc(mstatus);

    createMenu();

//    GL_Setup(window_width, window_height);
    glutSpecialFunc(keyboard);

    glutMainLoop();

    free(elements);
    free(elements2);

    return 0;

}
