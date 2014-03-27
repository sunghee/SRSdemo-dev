//============================================================================
// Name        : SRSdemo1.cpp

// Author      : Sung Hee Park @ ETRI
// Version     :
// Copyright   : (C) Copyright 2014
// Description : Hello World in C++, Ansi-style
//============================================================================

// 주석
// 주석 by j.w.jeong 2014.3.27

#include <GL/glu.h>
#include <GL/glut.h>
#include "rapidxml.hpp"

#include <fstream>
#include <vector>
#include <iostream>
#include <string.h>
#include <malloc.h>
#include <math.h>

using namespace std;

using namespace rapidxml;

#define window_width  1024
#define window_height 768

#define VESSEL		1
#define STOMACH		2
#define LIVER		3
#define INSTRUMENT1 4
#define INSTRUMENT2 5
#define STOMACH_MESH	6




#define BUFSIZE 512

char *objectNames[] =
{"Nothing", "Vessel", "Stomach", "Liver", "Instrument1", "Instrument2","stomach-mesh"};

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

//int window_width = 640;
//int window_height = 480;

#define X .525731112119133606
#define Z .850650808352039932

static GLfloat vdata[12][3] = {
   {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},
   {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},
   {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0}
};
static GLuint tindices[20][3] = {
   {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},
   {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},
   {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6},
   {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} };
int i;



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

void drawCylinder()
{
	float top = 3.0;
	float base = 3.0;
	float height = 30.0;
	float slices = 10.0;
	float stacks = 20.0;

	gluCylinder(gluNewQuadric(), base, top, height, slices, stacks);
}


void myInit(void)
{
  GLfloat light_ambient[] =
  {0.2, 0.2, 0.2, 1.0};
  GLfloat light_diffuse[] =
  {1.0, 1.0, 1.0, 1.0};
  GLfloat light_specular[] =
  {1.0, 1.0, 1.0, 1.0};
  GLfloat light_position[] =
  {1.0, 1.0, 1.0, 0.0};

  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  glEnable(GL_LIGHT0);
  glDepthFunc(GL_LESS);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);

  glSelectBuffer(BUFSIZE, selectBuf);

  glNewList(VESSEL, GL_COMPILE);
  //glutSolidTorus(0.275, 0.85, 10, 15);
  drawCylinder();
  glEndList();

  glNewList(STOMACH, GL_COMPILE);
//  glutSolidTetrahedron();
  glutSolidSphere(4.0, 16, 16);
  drawPolygon();
  glEndList();

  glNewList(LIVER, GL_COMPILE);
  //glutSolidIcosahedron();
  glutSolidSphere(2.0, 16, 16);
  glEndList();

  glNewList(INSTRUMENT1, GL_COMPILE);
  //glutSolidIcosahedron();
  glutSolidCube(2.0);
  glEndList();

  glNewList(INSTRUMENT2, GL_COMPILE);
  //glutSolidIcosahedron();
  glutSolidCube(2.0);
  glEndList();


  glNewList(STOMACH_MESH, GL_COMPILE);
  //glutSolidIcosahedron();
  drawIcosahedron();
  glEndList();

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
void main_loop()
{
	static double angle;

	GLdouble eqn[4] = {0.0, 1.0, 0.0, 0.0};
	GLdouble eqn2[4]={1.0, 0.0, 0.0, 0.0};


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(0,1,0,1);

	glColor3f(0.5,0.0,0.0);



	glLoadIdentity();
	/* light */

	gluLookAt(0.0-2*scaling,10.0,60.0+2 * scaling, 0.0, 0.0, 0.0, 0,1,0);

	glColor3f(0.5,0.5,0.0);
	glBegin(GL_LINES);
		glVertex3f(0,0,0);
		glVertex3f(30,0,0);
		glVertex3f(0,0,0);
		glVertex3f(0,30,0);
		glVertex3f(0,0,0);
		glVertex3f(0,0,30);
	glEnd();
/* vessels */
	glPushMatrix();
	glTranslatef(0,15,0);
//	glClipPlane(GL_CLIP_PLANE0, eqn);
//	glEnable(GL_CLIP_PLANE0);
	glRotatef(90, 1, 0, 0);
	drawCylinder();


	/* vessel label */
	char text[] = "Vessel - cylinder";

	float x = 0, y = 20, z = 0;
	// The color, red for me
	glColor3f(1, 0, 0);
	// Position of the text to be printer
	glRasterPos3f(x, y, z);

	for(int i = 0; text[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);

	glPopMatrix();

	/* organs */
	/* stomach */
	glPushMatrix();
	glTranslatef(5, -7, 15);
	glRotatef(45, 0, 0, 1.0);
	glScalef(2.0, 1.0, 1.0);
	glutWireSphere(4.0, 16, 16);

	/* stomach label */
	char text4[] = "stomach: ellipsoid";
	x = 5, y = -5, z = 0;
	// The color, red for me
	glColor3f(1, 0, 0);
	// Position of the text to be printer
	glRasterPos3f(x, y, z);
	for(int i = 0; text4[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text4[i]);

	glPopMatrix();

	/* liver */
	glPushMatrix();
	glTranslatef(-10,0, 0);
	glRotatef(0, 0, 0, 1.0);
	glScalef(2.0, 1.0, 1.0);
	glutWireSphere(2.0, 16, 16);

	/* liver label */
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

	/* instrument1 */
	glPushMatrix();
	glTranslatef(-10, -10, 10);
	glRotatef(45, 1, 1, 1.0);
	glScalef(3.0, 1.0, 1.0);
	glutWireCube(2.0);

	/* instrument1 label */
	char text2[] = "instrument1-stick";
	x = 10, y = 10, z = 0;
	// The color, red for me
	glColor3f(1, 0, 0);
	// Position of the text to be printer
	glRasterPos3f(x, y, z);
	for(int i = 0; text2[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text2[i]);

	glPopMatrix();

	/* instrument2 */
	glPushMatrix();
	glTranslatef(10,10, 5);
	glRotatef(30, 1,1, 1.0);
	glScalef(2.0, 1.0, 1.0);
	glutWireCube(2.0);

	/* instrument2 label */
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

void GL_Setup(int a, int b)
{
	glViewport(0,0, a, b);
	glMatrixMode(GL_PROJECTION);
	glEnable(GL_DEPTH_TEST);
	gluPerspective(45, (float)a/b, 1.1, 110);
	glMatrixMode(GL_MODELVIEW);
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
  {1.0, 0.0, 0.0, 1.0};

  glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
  glColor3f(1.0, 0.0, 0.0);
}

void highlightEnd(void)
{
	glPopAttrib();
}

void draw_front(void)
{

	glPushMatrix();
	//	glRotatef(20.0, 1.0, 0.0, 0.0);
	gluLookAt(0.0+shift, 0.0, 60.0+scaling, 0.0, 0.0,0.0, 0,1,0);
//	glScalef(1.3, 1.3, 1.3);

	////////////////
	// vessel
	///////////////
	glLoadName(1);
	glPushMatrix();
	if (theObject == 1)
		highlightBegin();

	glTranslatef(0,15,0);
//	//  if (!menu_inuse) {
//		if (axis == 0) {
		  if (theObject == 1) {
//			theObject = 1;
//	//		glRotatef(90, 0, 1, 0);
			glRotatef((pow(rotate_speed,2)+pow(scaling,2)), rotate_speed, scaling, 0);
//	//		glutPostRedisplay();
		  }
//		  axis = 2;
//		}
//	//  }
	glRotatef(90, 1, 0, 0);


//	if(menu_inuse)
//	{
//		glPushMatrix();
//		glRotatef(sqrt(pow(rotate_speed,2)+pow(scaling,2)), rotate_speed, scaling, 0);
//		glPopMatrix();
//	}
	glCallList(VESSEL);
	/* vessel label */
	char text[] = "Vessel - cylinder";
	float x = 0, y = 20, z = 0;
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


	if (theObject == 1)
		highlightEnd();
	glPopMatrix();

	//////////////
	// Stomach
	/////////////
	glLoadName(STOMACH_MESH);
	glPushMatrix();
	if (theObject == 6)
		highlightBegin();
	glTranslatef(5, -7, 15);
	//  if (!menu_inuse) {
//		if (mouse_state == GLUT_LEFT) {
		  if (theObject == 6) {
//			theObject = 2;
//			glRotatef(90, 0, 1, 0);
			glRotatef((pow(rotate_speed,2)+pow(scaling,2)), rotate_speed, scaling, 0);
	//		glutPostRedisplay();
		  }
//		}
	//  }
	glRotatef(45, 0, 0, 1.0);
	glScalef(6.0, 3.0, 3.0);
	/* stomach label */
	char text4[] = "stomach: ellipsoid";
	x = 5, y = -5, z = 0;
	// The color, red for me
	glColor3f(1, 0, 0);
	// Position of the text to be printer
	glRasterPos3f(x, y, z);
	for(int i = 0; text4[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text4[i]);

	glCallList(STOMACH_MESH);
	if (theObject == 6)
		highlightEnd();
	glPopMatrix();

	/////////////////////
	// Liver
	////////////////////
	glLoadName(3);
	glPushMatrix();
	if (theObject == 3)
		highlightBegin();
	glTranslatef(-10,0, 0);
	glRotatef(0, 0, 0, 1.0);
	glScalef(2.0, 1.0, 1.0);
	/* liver label */
	char text5[] = "liver: ellipsoid";
	x = -10, y = 5, z = 0;
	// The color, red for me
	glColor3f(1, 0, 0);
	// Position of the text to be printer
	glRasterPos3f(x, y, z);
	for(int i = 0; text5[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text5[i]);

	glCallList(LIVER);
	if (theObject == 3)
		highlightEnd();
	glPopMatrix();

	////////////////
	// Instrument 1
	////////////////
	glLoadName(4);
	glPushMatrix();
	if (theObject == 4)
		highlightBegin();
	glTranslatef(-10, -10, 10);
	glRotatef(45, 1, 1, 1.0);
	glScalef(3.0, 1.0, 1.0);
	/* instrument1 label */
	char text2[] = "instrument1-stick";
	x = 10, y = 10, z = 0;
	// The color, red for me
	glColor3f(1, 0, 0);
	// Position of the text to be printer
	glRasterPos3f(x, y, z);
	for(int i = 0; text2[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text2[i]);

	glCallList(INSTRUMENT1);
	if (theObject == 4)
		highlightEnd();
	glPopMatrix();



	////////////////
	// Instrument 2
	///////////////
	glLoadName(5);
	glPushMatrix();
	if (theObject == 5)
		highlightBegin();
	glTranslatef(10,10, 5);
	glRotatef(30, 1,1, 1.0);
	glScalef(2.0, 1.0, 1.0);

	/* instrument2 label */
	char text3[] = "instrument2 - stick";
	x = -20, y = -15, z = 0;
	// The color, red for me
	glColor3f(1, 0, 0);
	// Position of the text to be printer
	glRasterPos3f(x, y, z);
	for(int i = 0; text3[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text3[i]);

	glCallList(INSTRUMENT2);
	if (theObject == 5)
		highlightEnd();
	glPopMatrix();

	glPopMatrix();



}
void draw_half_left(void)
{

	glPushMatrix();
	gluLookAt(5.0, 0.0, 5.0, 0.0, 0.0,0.0, 0,1,0);
//	glScalef(1.3, 1.3, 1.3);
//	glRotatef(20.0, 1.0, 0.0, 0.0);
	////////////////
	// vessel
	///////////////
	glLoadName(1);
	glPushMatrix();
	if (theObject == 1)
		highlightBegin();

	glTranslatef(0,15,0);
	glRotatef(90, 1, 0, 0);
	glCallList(VESSEL);
	/* vessel label */
	char text[] = "Vessel - cylinder";
	float x = 0, y = 20, z = 0;
	// The color, red for me
	glColor3f(1, 0, 0);
	// Position of the text to be printer
	glRasterPos3f(x, y, z);
	for(int i = 0; text[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
	if (theObject == 1)
		highlightEnd();
	glPopMatrix();

	//////////////
	// Stomach
	/////////////
	glLoadName(2);
	glPushMatrix();
	if (theObject == 2)
		highlightBegin();
	glTranslatef(5, -7, 15);
	glRotatef(45, 0, 0, 1.0);
	glScalef(2.0, 1.0, 1.0);
	/* stomach label */
	char text4[] = "stomach: ellipsoid";
	x = 5, y = -5, z = 0;
	// The color, red for me
	glColor3f(1, 0, 0);
	// Position of the text to be printer
	glRasterPos3f(x, y, z);
	for(int i = 0; text4[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text4[i]);

	glCallList(STOMACH);
	if (theObject == 2)
		highlightEnd();
	glPopMatrix();

	/////////////////////
	// Liver
	////////////////////
	glLoadName(3);
	glPushMatrix();
	if (theObject == 3)
		highlightBegin();
	glTranslatef(-10,0, 0);
	glRotatef(0, 0, 0, 1.0);
	glScalef(2.0, 1.0, 1.0);
	/* liver label */
	char text5[] = "liver: ellipsoid";
	x = -10, y = 5, z = 0;
	// The color, red for me
	glColor3f(1, 0, 0);
	// Position of the text to be printer
	glRasterPos3f(x, y, z);
	for(int i = 0; text5[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text5[i]);

	glCallList(LIVER);
	if (theObject == 3)
		highlightEnd();
	glPopMatrix();

	////////////////
	// Instrument 1
	////////////////
	glLoadName(4);
	glPushMatrix();
	if (theObject == 4)
		highlightBegin();
	glTranslatef(-10, -10, 10);
	glRotatef(45, 1, 1, 1.0);
	glScalef(3.0, 1.0, 1.0);
	/* instrument1 label */
	char text2[] = "instrument1-stick";
	x = 10, y = 10, z = 0;
	// The color, red for me
	glColor3f(1, 0, 0);
	// Position of the text to be printer
	glRasterPos3f(x, y, z);
	for(int i = 0; text2[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text2[i]);

	glCallList(INSTRUMENT1);
	if (theObject == 4)
		highlightEnd();
	glPopMatrix();



	////////////////
	// Instrument 2
	///////////////
	glLoadName(5);
	glPushMatrix();
	if (theObject == 5)
		highlightBegin();
	glTranslatef(10,10, 5);
	glRotatef(30, 1,1, 1.0);
	glScalef(2.0, 1.0, 1.0);

	/* instrument2 label */
	char text3[] = "instrument2 - stick";
	x = -20, y = -15, z = 0;
	// The color, red for me
	glColor3f(1, 0, 0);
	// Position of the text to be printer
	glRasterPos3f(x, y, z);
	for(int i = 0; text3[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text3[i]);

	glCallList(INSTRUMENT2);
	if (theObject == 5)
		highlightEnd();
	glPopMatrix();

	glPopMatrix();



}
void draw_half_right(void)
{

	glPushMatrix();
	gluLookAt(-5.0, 0.0, 5.0, 0.0, 0.0,0.0, 0,1,0);
//	glScalef(1.3, 1.3, 1.3);
//	glRotatef(20.0, 1.0, 0.0, 0.0);
	////////////////
	// vessel
	///////////////
	glLoadName(1);
	glPushMatrix();
	if (theObject == 1)
		highlightBegin();

	glTranslatef(0,15,0);
	glRotatef(90, 1, 0, 0);
	glCallList(VESSEL);
	/* vessel label */
	char text[] = "Vessel - cylinder";
	float x = 0, y = 20, z = 0;
	// The color, red for me
	glColor3f(1, 0, 0);
	// Position of the text to be printer
	glRasterPos3f(x, y, z);
	for(int i = 0; text[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
	if (theObject == 1)
		highlightEnd();
	glPopMatrix();

	//////////////
	// Stomach
	/////////////
	glLoadName(2);
	glPushMatrix();
	if (theObject == 2)
		highlightBegin();
	glTranslatef(5, -7, 15);
	glRotatef(45, 0, 0, 1.0);
	glScalef(2.0, 1.0, 1.0);
	/* stomach label */
	char text4[] = "stomach: ellipsoid";
	x = 5, y = -5, z = 0;
	// The color, red for me
	glColor3f(1, 0, 0);
	// Position of the text to be printer
	glRasterPos3f(x, y, z);
	for(int i = 0; text4[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text4[i]);

	glCallList(STOMACH);
	if (theObject == 2)
		highlightEnd();
	glPopMatrix();

	/////////////////////
	// Liver
	////////////////////
	glLoadName(3);
	glPushMatrix();
	if (theObject == 3)
		highlightBegin();
	glTranslatef(-10,0, 0);
	glRotatef(0, 0, 0, 1.0);
	glScalef(2.0, 1.0, 1.0);
	/* liver label */
	char text5[] = "liver: ellipsoid";
	x = -10, y = 5, z = 0;
	// The color, red for me
	glColor3f(1, 0, 0);
	// Position of the text to be printer
	glRasterPos3f(x, y, z);
	for(int i = 0; text5[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text5[i]);

	glCallList(LIVER);
	if (theObject == 3)
		highlightEnd();
	glPopMatrix();

	////////////////
	// Instrument 1
	////////////////
	glLoadName(4);
	glPushMatrix();
	if (theObject == 4)
		highlightBegin();
	glTranslatef(-10, -10, 10);
	glRotatef(45, 1, 1, 1.0);
	glScalef(3.0, 1.0, 1.0);
	/* instrument1 label */
	char text2[] = "instrument1-stick";
	x = 10, y = 10, z = 0;
	// The color, red for me
	glColor3f(1, 0, 0);
	// Position of the text to be printer
	glRasterPos3f(x, y, z);
	for(int i = 0; text2[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text2[i]);

	glCallList(INSTRUMENT1);
	if (theObject == 4)
		highlightEnd();
	glPopMatrix();



	////////////////
	// Instrument 2
	///////////////
	glLoadName(5);
	glPushMatrix();
	if (theObject == 5)
		highlightBegin();
	glTranslatef(10,10, 5);
	glRotatef(30, 1,1, 1.0);
	glScalef(2.0, 1.0, 1.0);

	/* instrument2 label */
	char text3[] = "instrument2 - stick";
	x = -20, y = -15, z = 0;
	// The color, red for me
	glColor3f(1, 0, 0);
	// Position of the text to be printer
	glRasterPos3f(x, y, z);
	for(int i = 0; text3[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text3[i]);

	glCallList(INSTRUMENT2);
	if (theObject == 5)
		highlightEnd();
	glPopMatrix();

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
	gluPerspective(45, (float)W/H, 1.1, 110);
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
      viewport[0] = 0;
      viewport[1] = 0;
      viewport[2] = W;
      viewport[3] = H;
      gluPickMatrix(x, H - y, 5.0, 5.0, viewport);
      mypers(); // this is different from locate

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
	break;
	case GLUT_KEY_LEFT:
		shift -= 1.0f;
	break;
	case GLUT_KEY_RIGHT:
		shift += 1.0f;
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

int main(int argc, char ** argv)
{

	int submenu;

    glutInit(&argc, argv);
    glutInitWindowSize(window_width, window_height); //set a window size
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE|GLUT_DEPTH);
    glutCreateWindow("SRS Viewer!!");
//    init();

//    loadXML();
    loadSRSML();
    myInit();

    glutDisplayFunc(display);
    //glutReshapeFunc(myReshape);
    glutReshapeFunc(GL_Setup);
    glutIdleFunc(idle);

//    glutIdleFunc(main_loop);

    glutMouseFunc(mouseCallBack);
    glutMotionFunc(motionCallBack);


    /* sub menu */
    submenu = glutCreateMenu(polygon_mode);
    glutAddMenuEntry("Filled", 1);
    glutAddMenuEntry("Wireframe", 2);

    /* main menu */
    glutCreateMenu(main_menu);
    glutAddSubMenu("Polygon mode", submenu);
    glutAddMenuEntry("Quit", 999);

    /* how to attach main menu */
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutPassiveMotionFunc(passive_pers);
    glutEntryFunc(entry);
    glutMenuStatusFunc(mstatus);

//    GL_Setup(window_width, window_height);
    glutSpecialFunc(keyboard);

    glutMainLoop();

    free(elements);
    free(elements2);

    return 0;

}
