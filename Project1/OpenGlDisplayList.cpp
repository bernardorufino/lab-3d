// OpenGlDisplayList.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <GL/glut.h>
using namespace std;

#define PI 3.14159265358979323846
#define sqr(x) ((x)*(x))
#define ROTX 1
#define ROTY 2
#define ROTZ 3
#define DIRX 4
#define DIRY 5
#define DIRZ 6
#define FRONT 7
#define BACK 8

class CPoint3D {
public:
	float x, y, z;
	CPoint3D()
	{};
	CPoint3D(float x1, float y1, float z1)
	{
		x = x1; y = y1; z = z1;
	}
	void Set(float x1, float y1, float z1)
	{
		x = x1; y = y1; z = z1;
	}
	CPoint3D Normalize()
	{
		float L = Abs();
		return (L > 0.0f) ? (*this) / L : (*this);
	}

	CPoint3D Dir() 
	{
		return Normalize();
	}

	// angle in radians
	CPoint3D Rotate(float angle, CPoint3D dir) 
	{
		CPoint3D base = (*this) * cos(angle);
		CPoint3D elev = dir.Dir() * sin(angle);
		//cout << "orig = " + (*this).ToString() + ", base = " + base.ToString() + ", elev = " + elev.ToString() << "\n";
		return base + elev;
	}

	CPoint3D RotateDeg(float angleDegrees, CPoint3D dir)
	{
		return Rotate(angleDegrees * PI / 180, dir);
	}

	CPoint3D CrossProduct(CPoint3D p) 
	{
		return CPoint3D(y * p.z - z * p.y, z * p.x - x * p.z, x * p.y - y * p.x);
	}

	float Abs() 
	{
		return sqr(x) + sqr(y) + sqr(z);
	}

	CPoint3D operator +(CPoint3D p)
	{
		return CPoint3D(x + p.x, y + p.y, z + p.z);
	}
	CPoint3D operator -(CPoint3D p)
	{
		return CPoint3D(x - p.x, y - p.y, z - p.z);
	}
	CPoint3D operator *(CPoint3D p)
	{
		return CPoint3D(x * p.x, y * p.y, z * p.z);
	}
	CPoint3D operator /(CPoint3D p)
	{
		return CPoint3D(x / p.x, y / p.y, z / p.z);
	}
	CPoint3D operator *(float f) 
	{
		return CPoint3D(x * f, y * f, z * f);
	}
	CPoint3D operator /(float f) 
	{
		return CPoint3D(x / f, y / f, z / f);
	}
	CPoint3D operator - () {
		return CPoint3D(-x, -y, -z);
	}

	string ToString() {
		return "(" + to_string(x) + ", " + to_string(y) + ", " + to_string(z) + ")";
	}

};

class CCamera{
public:
	CPoint3D P0, At, Up;
	float AngX, AngY, AngZ;

	CCamera()
	{
		P0.Set(0.0f, 0.0f, 0.0f); At.Set(0.0f, 0.0f, -1.0f); Up.Set(0.0f, 1.0f, 0.0f); AngX = 0.0f; AngY = 0.0f;  AngZ = 0.0f;
	}

	CCamera(CPoint3D p0, CPoint3D p, CPoint3D up)
	{
		P0 = p0; At = p; Up = up; AngX = 0.0f; AngY = 0.0f;  AngZ = 0.0f;
	}

	void Set(CPoint3D p0, CPoint3D p, CPoint3D up)
	{
		P0 = p0; At = p; Up = up; AngX = 0.0f; AngY = 0.0f;  AngZ = 0.0f;
	}

	//TODO // RotateCamera can be a method of CCamera
	void RotateCamera(int rotMode, float ang) {
		
	}

	void Forward(float delta) {
		CPoint3D d = -(At - P0).Dir() * delta;
		glTranslatef(d.x, d.y, d.z);
		//At = At + d;
		//P0 = P0 + d;
	}

	void Pitch(float ang) {
		RotateCam(ang, Right());
		CPoint3D rotDir = -(At - P0).Dir();
	}

	void Yaw(float ang) {
		RotateCam(ang, Up);
		At = P0 + (At - P0).RotateDeg(ang, Right());
	}

	void Roll(float ang) {
		RotateCam(ang, (At - P0).Dir());
		Up = Up.RotateDeg(ang, Right());
	}

	void LookAt()  // call gluLookAt
	{
		gluLookAt((GLdouble)P0.x, (GLdouble)P0.y, (GLdouble)P0.z,
			(GLdouble)At.x, (GLdouble)At.y, (GLdouble)At.z,
			(GLdouble)Up.x, (GLdouble)Up.y, (GLdouble)Up.z);
	}

	void Update()
	{
		glLoadIdentity();
		LookAt();
	}
	string ToString()
	{
		return "P0 = " + P0.ToString() + ", At = " + At.ToString() + ", Up = " + Up.ToString() + ", dir = " + (At - P0).ToString();
	}
private:

	CPoint3D Right() {
		return (At - P0).CrossProduct(Up).Dir();
	}

	void RotateCam(float ang, CPoint3D axis) {
		glTranslatef(P0.x, P0.y, P0.z);
		glRotatef(ang, axis.x, axis.y, axis.z);
		glColor3f(1.0f, 0.1f, 0.1f);
		glutSolidSphere(0.05, 10, 10);
		glTranslatef(-P0.x, -P0.y, -P0.z);
	}

};
float angle = 0.0;
float x = 0.0f, y = 1.75f, z = 5.0f;
float lx = 0.0f, ly = 0.0f, lz = -1.0f;

float ratio = 1.0;
int frame, time, timebase = 0;
char s[30];
CCamera cam;
GLuint DLid;

GLuint createDL(void);

void changeSize(int w, int h)	{

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;

	ratio = 1.0f * w / h;
	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the clipping volume
	gluPerspective(45, ratio, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
	//TO DO // Set camera initial position 
		// this is for the snow man scenario 
	cam.Set(
		CPoint3D(x, y, z),
		CPoint3D(x + lx, y + ly, z + lz),
		CPoint3D(0.0f, 1.0f, 0.0f));
	cam.Update();
}

void initScene() {

	glEnable(GL_DEPTH_TEST);
	// Load or call scenario
	DLid = createDL();
}


void drawSnowMan() {


	glColor3f(1.0f, 1.0f, 1.0f);

	// Draw Body	
	glTranslatef(0.0f, 0.75f, 0.0f);
	glutSolidSphere(0.75f, 20, 20);


	// Draw Head
	glTranslatef(0.0f, 1.0f, 0.0f);
	glutSolidSphere(0.25f, 20, 20);

	// Draw Eyes
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 0.0f);
	glTranslatef(0.05f, 0.10f, 0.18f);
	glutSolidSphere(0.05f, 10, 10);
	glTranslatef(-0.1f, 0.0f, 0.0f);
	glutSolidSphere(0.05f, 10, 10);
	glPopMatrix();

	// Draw Nose
	glColor3f(1.0f, 0.5f, 0.5f);
	glRotatef(0.0f, 1.0f, 0.0f, 0.0f);
	glutSolidCone(0.08f, 0.5f, 10, 2);
}



GLuint createDL() {
	GLuint snowManDL, loopDL;

	snowManDL = glGenLists(1);
	loopDL = glGenLists(1);

	glNewList(snowManDL, GL_COMPILE);
	drawSnowMan();
	glEndList();

	glNewList(loopDL, GL_COMPILE);

	for (int i = -3; i < 3; i++)
		for (int j = -3; j < 3; j++) {
		glPushMatrix();
		glTranslatef(i*10.0f, 0, j * 10.0f);
		glCallList(snowManDL);
		glPopMatrix();
		}
	glEndList();

	return(loopDL);
}


void renderScene(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw ground

	glColor3f(0.9f, 0.9f, 0.9f);
	glBegin(GL_QUADS);
	glVertex3f(-100.0f, 0.0f, -100.0f);
	glVertex3f(-100.0f, 0.0f, 100.0f);
	glVertex3f(100.0f, 0.0f, 100.0f);
	glVertex3f(100.0f, 0.0f, -100.0f);
	glEnd();

	// Draw 36 SnowMen

	glCallList(DLid);
	frame++;
	time = glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000) {
		sprintf_s(s, "FPS:%4.2f", frame*1000.0 / (time - timebase));
		glutSetWindowTitle(s);
		timebase = time;
		frame = 0;
	}
	glutSwapBuffers();
}

/*//TODO // MoveCamera can be method also can deal with speed
void MoveCamera(int diretion, ToDo) {
	switch (diretion){
		// do some transformation if needed 

	case DIRX: x = x + ...; break;
	case DIRY: y = y + ..; break;
	case DIRZ: z = z + ....; break;
	case FRONT: // move forward following camera direction
	case BACK: // move backward following camera direction
	}
	ToDo// call camera SetPosition;
		// call camera Update;
}*/

float da = 0.5f;

void inputKey(unsigned char c, int x, int y) {
	int a = c;
	if ('A' <= c && c <= 'Z') z = z - 'A' + 'a';
	switch (c) {
	case 'w': // UP
		cam.Forward(2.5);
		break;
	case 'd': // RIGHT
		cam.Yaw(2.5);
		break;
	case 's': // DOWN
		cam.Forward(-2.5);
		break;
	case 'a': // LEFT
		cam.Yaw(-2.5);
		break; 
	case 'i':
		cam.Pitch(2.5);
		break;
	case 'k':
		cam.Pitch(-2.5);
		break;
	case 'j':
		cam.Roll(2.5);
		break;
	case 'l':
		cam.Roll(-2.5);
		break;
	}
}

void mouse(int button, int state, int x, int y)
{
	switch (button) {
		//  button can be GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, or GLUT_RIGHT_BUTTON.
		// state can be r GLUT_UP or GLUT_DOWN (pressed)
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) {
			//printf("Forward\n");
			//cam.Forward(5);
		}
		break;

	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN) {
			//printf("Backward\n");
			//cam.Backward(5);
		}
		break;

	default:
		break;
	}
}



int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(400, 400);
	glutCreateWindow("Lab4");

	initScene();

	glutKeyboardFunc(inputKey);

	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);

	glutReshapeFunc(changeSize);
	glutMouseFunc(mouse);
	glutMainLoop();

	return(0);
}



