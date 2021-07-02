#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include<iostream>
#define CCW 1
#define CW -1
#define max(a,b) a>b?a:b
#define min(a,b) a<b?a:b
#include <GL/glut.h>

#define pi (2 * acos(0.0))
using namespace std;
double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;

//-----------------------
struct point
{
	double x, y, z;
	point(double x, double y, double z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	void print()
	{
		cout << x << " " << y << " " << z << endl;
	}
	point() {}
	point addPoints(point b)
	{
		return point(x + b.x, y + b.y, z + b.z);
	}
};
point addPoints(point a, point b)
{
	point ret;
	ret.x = a.x + b.x, ret.y = a.y + b.y, ret.z = a.z + b.z;
	return ret;
}

point subPoints(point a, point b)
{
	point ret;
	ret.x = a.x - b.x, ret.y = a.y - b.y, ret.z = a.z - b.z;
	return ret;
}

point rotate(point toRotate, point ref, int whichDir)
{
	point n, ret;
	double theta = 0.03 * whichDir;

	n.x = ref.y * toRotate.z - ref.z * toRotate.y;
	n.y = ref.z * toRotate.x - ref.x * toRotate.z;
	n.z = ref.x * toRotate.y - ref.y * toRotate.x;

	ret.x = toRotate.x * cos(theta) + n.x * sin(theta);
	ret.y = toRotate.y * cos(theta) + n.y * sin(theta);
	ret.z = toRotate.z * cos(theta) + n.z * sin(theta);

	return ret;
}


//-----------------------------

void drawAxes()
{
	if (drawaxes == 1)
	{
		//glColor3f(1.0, 1.0, 1.0);
		glColor3f(0,0,1);
		glBegin(GL_LINES);
		{
			glVertex3f(100, 0, 0);
			glVertex3f(-100, 0, 0);

			glVertex3f(0, -100, 0);
			glVertex3f(0, 100, 0);

			glVertex3f(0, 0, 100);
			glVertex3f(0, 0, -100);
		}
		glEnd();
	}
}



void drawSquare(double a)
{
	glColor3f(1, 1, 1);

	glBegin(GL_QUADS);
	{
		glVertex3f(0, 0, 0);
		glVertex3f(a, 0, 0);
		glVertex3f(a, a, 0);
		glVertex3f(0, a, 0);
	}
	glEnd();
}

void drawCylinder(double radius, double height, int slices)
{
	glColor3f(0, 1, 0);

	int i;
	double shade;
	struct point points[100];
	//generate points
	for (i = 0; i <= slices; i++)
	{
		points[i].x = radius * cos(((double)i / (double)slices) * pi / 2);
		points[i].y = radius * sin(((double)i / (double)slices) * pi / 2);
	}
	//draw triangles using generated points
	for (i = 0; i < slices; i++)
	{

		glBegin(GL_QUADS);
		{
			glVertex3f(points[i].x, points[i].y, 0);
			glVertex3f(points[i + 1].x, points[i + 1].y, 0);
			glVertex3f(points[i + 1].x, points[i + 1].y, height);
			glVertex3f(points[i].x, points[i].y, height);
		}
		glEnd();
	}
}

void drawSpheres(double radius, int slices, int stacks)
{
	glColor3f(1, 0, 0);
	struct point points[100][100];
	int i, j;
	double h, r;
	//generate points
	for (i = 0; i <= stacks; i++)
	{
		h = radius * sin(((double)i / (double)stacks) * (pi / 2));
		r = radius * cos(((double)i / (double)stacks) * (pi / 2));
		for (j = 0; j <= slices; j++)
		{
			points[i][j].x = r * cos(((double)j / (double)slices) * pi / 2);
			points[i][j].y = r * sin(((double)j / (double)slices) * pi / 2);
			points[i][j].z = h;
		}
	}
	//draw quads using generated points
	for (i = 0; i < stacks; i++)
	{
		// glColor3f((double)i / (double)stacks, (double)i / (double)stacks, (double)i / (double)stacks);
		for (j = 0; j < slices; j++)
		{
			glBegin(GL_QUADS);
			{
				//upper hemisphere
				glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
				glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z);
				glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, points[i + 1][j + 1].z);
				glVertex3f(points[i + 1][j].x, points[i + 1][j].y, points[i + 1][j].z);
				//lower hemisphere
				// glVertex3f(points[i][j].x, points[i][j].y, -points[i][j].z);
				// glVertex3f(points[i][j + 1].x, points[i][j + 1].y, -points[i][j + 1].z);
				// glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, -points[i + 1][j + 1].z);
				// glVertex3f(points[i + 1][j].x, points[i + 1][j].y, -points[i + 1][j].z);
			}
			glEnd();
		}
	}
}

float squareSide = 40;
float cylRad = 10;
point curPos,u,r,l;

void intializeVectors(){
	curPos = {100,100,0};
	u = {0,0,1};
	l = {-1/sqrt(2), -1/sqrt(2), 0};
	r = {-1/sqrt(2), 1/sqrt(2),0};
}

void drawAllSpheres()
{
	//upper front right
	glPushMatrix();
	{
		glTranslatef(cylRad + squareSide, cylRad + squareSide, cylRad + squareSide);
		drawSpheres(cylRad, 30, 30);
	}
	glPopMatrix();

	//upper back right
	glPushMatrix();
	{
		glTranslatef(cylRad, cylRad + squareSide, cylRad + squareSide);
		glRotatef(90, 0, 0, 1);
		drawSpheres(cylRad, 30, 30);
	}
	glPopMatrix();

	//upper back left
	glPushMatrix();
	{
		glTranslatef(cylRad, cylRad, cylRad + squareSide);
		glRotatef(180, 0, 0, 1);
		drawSpheres(cylRad, 30, 30);
	}
	glPopMatrix();

	//upper front left
	glPushMatrix();
	{
		glTranslatef(cylRad + squareSide, cylRad, cylRad + squareSide);
		glRotatef(-90, 0, 0, 1);
		drawSpheres(cylRad, 30, 30);
	}
	glPopMatrix();

	//lower front right
	glPushMatrix();
	{
		glTranslatef(cylRad + squareSide, cylRad + squareSide, cylRad);
		glRotatef(180, 1, 1, 0);
		drawSpheres(cylRad, 30, 30);
	}
	glPopMatrix();

	//lower back right
	glPushMatrix();
	{
		glTranslatef(cylRad, cylRad + squareSide, cylRad);
		glRotatef(90, 0, 0, 1);
		glRotatef(180, 1, 1, 0);
		drawSpheres(cylRad, 30, 30);
	}
	glPopMatrix();

	//lower back left
	glPushMatrix();
	{
		glTranslatef(cylRad, cylRad, cylRad);
		glRotatef(180, 0, 0, 1);
		glRotatef(180, 1, 1, 0);

		drawSpheres(cylRad, 30, 30);
	}
	glPopMatrix();

	//lower front left
	glPushMatrix();
	{
		glTranslatef(cylRad + squareSide, cylRad, cylRad);
		glRotatef(180, 1, -1, 0);
		glRotatef(-90, 0, 0, 1);
		drawSpheres(cylRad, 30, 30);
	}
	glPopMatrix();
}

void drawAllSquareSides()
{
	//left Square
	glPushMatrix();
	{
		glTranslatef(cylRad, 0, cylRad);
		glRotatef(90, 1, 0, 0);
		drawSquare(squareSide);
	}
	glPopMatrix();

	//right square
	glPushMatrix();
	{
		glTranslatef(cylRad, squareSide + 2 * cylRad, cylRad);
		glRotatef(90, 1, 0, 0);
		drawSquare(squareSide);
	}
	glPopMatrix();

	// lower square
	glPushMatrix();
	{
		glTranslatef(cylRad, cylRad, 0);
		drawSquare(squareSide);
	}
	glPopMatrix();

	// upper square
	glPushMatrix();
	{
		glTranslatef(cylRad, cylRad, squareSide + 2 * cylRad);
		drawSquare(squareSide);
	}
	glPopMatrix();

	//front square
	glPushMatrix();
	{
		glTranslatef(squareSide + 2 * cylRad, cylRad, cylRad);
		glRotatef(-90, 0, 1, 0);
		drawSquare(squareSide);
	}
	glPopMatrix();

	//back square
	glPushMatrix();
	{
		glTranslatef(0, cylRad, cylRad);
		glRotatef(-90, 0, 1, 0);
		drawSquare(squareSide);
	}
	glPopMatrix();
}

void drawAllCylinders()
{
	glColor3f(1, 0, 0);

	// lower left
	glPushMatrix();
	{
		glTranslatef(cylRad, cylRad, cylRad);
		glRotatef(90, 0, 1, 0);
		glRotatef(-90, 0, 0, 1);
		drawCylinder(cylRad, squareSide, 30);
	}
	glPopMatrix();

	//upper left
	glPushMatrix();
	{
		glTranslatef(squareSide + cylRad, cylRad, squareSide + cylRad);
		glRotatef(-90, 0, 1, 0);
		glRotatef(-90, 0, 0, 1);
		drawCylinder(cylRad, squareSide, 30);
	}
	glPopMatrix();

	//lower right
	glPushMatrix();
	{
		glTranslatef(cylRad, squareSide + cylRad, cylRad);
		glRotatef(90, 0, 1, 0);
		drawCylinder(cylRad, squareSide, 30);
	}
	glPopMatrix();

	//upper right
	glPushMatrix();
	{
		glTranslatef(squareSide + cylRad, squareSide + cylRad, squareSide + cylRad);
		glRotatef(-90, 0, 1, 0);
		drawCylinder(cylRad, squareSide, 30);
	}
	glPopMatrix();

	// front upper
	glPushMatrix();
	{
		glTranslatef(squareSide + cylRad, squareSide + cylRad, squareSide + cylRad);
		glRotatef(90, 1, 0, 0);
		drawCylinder(cylRad, squareSide, 30);
	}
	glPopMatrix();

	// front lower
	glPushMatrix();
	{
		glTranslatef(squareSide + cylRad, squareSide + cylRad, cylRad);
		glRotatef(90, 1, 0, 0);
		glRotatef(-90, 0, 0, 1);

		drawCylinder(cylRad, squareSide, 30);
	}
	glPopMatrix();

	// back upper
	glPushMatrix();
	{
		glTranslatef(cylRad, squareSide + cylRad, squareSide + cylRad);
		glRotatef(90, 1, 0, 0);
		glRotatef(90, 0, 0, 1);

		drawCylinder(cylRad, squareSide, 30);
	}
	glPopMatrix();

	// back lower
	glPushMatrix();
	{
		glTranslatef(cylRad, squareSide + cylRad, cylRad);
		glRotatef(90, 1, 0, 0);
		glRotatef(180, 0, 0, 1);

		drawCylinder(cylRad, squareSide, 30);
	}
	glPopMatrix();

	// vertical cylinder
	// front right
	glPushMatrix();
	{
		glTranslatef(squareSide + cylRad, squareSide + cylRad, cylRad);
		drawCylinder(cylRad, squareSide, 30);
	}
	glPopMatrix();

	// back right
	glPushMatrix();
	{
		glTranslatef(cylRad, cylRad + squareSide, cylRad);
		glRotatef(90, 0, 0, 1);
		drawCylinder(cylRad, squareSide, 30);
	}
	glPopMatrix();

	// back left
	glPushMatrix();
	{
		glTranslatef(cylRad, cylRad, cylRad);
		glRotatef(180, 0, 0, 1);
		drawCylinder(cylRad, squareSide, 30);
	}
	glPopMatrix();

	// front left
	glPushMatrix();
	{
		glTranslatef(squareSide + cylRad, cylRad, cylRad);
		glRotatef(-90, 0, 0, 1);
		drawCylinder(cylRad, squareSide, 30);
	}
	glPopMatrix();

	
}


void keyboardListener(unsigned char key, int x, int y)
{
	switch (key)
	{

	case '1':
		l = rotate(l, u, CCW);
		r = rotate(r, u, CCW);
		break;
	case '2':
		l = rotate(l, u, CW);
		r = rotate(r, u, CW);
		break;
	case '3':
		l = rotate(l, r, CCW);
		u = rotate(u, r, CCW);
		break;
	case '4':
		l = rotate(l, r, CW);
		u = rotate(u, r, CW);
		break;	
	case '5':
		r = rotate(r, l, CW);
		u = rotate(u, l, CW);
		break;	
	case '6':
		r = rotate(r, l, CCW);
		u = rotate(u, l, CCW);
		break;		
	default:
		break;
	}
}

float delta = 0.5;
float totalLen = squareSide + 2*cylRad;
void specialKeyListener(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_DOWN: //down arrow key
			curPos = subPoints(curPos, l);

		break;
	case GLUT_KEY_UP: // up arrow key		
	curPos = addPoints(curPos, l);

		break;

	case GLUT_KEY_RIGHT:	
		curPos = addPoints(curPos, r);

		break;
	case GLUT_KEY_LEFT:	
		curPos = subPoints(curPos, r);

		break;
	case GLUT_KEY_PAGE_UP:
		curPos = addPoints(curPos, u);
		break;
	case GLUT_KEY_PAGE_DOWN:
		curPos = subPoints(curPos, u);
		break;

	case GLUT_KEY_INSERT:
		break;

	case GLUT_KEY_HOME:
		squareSide = max(squareSide - 2*delta,0);
		cylRad = min(cylRad + delta, totalLen/2);
		break;
	case GLUT_KEY_END:
		cylRad = max(cylRad - delta,0);
		squareSide = min(squareSide + 2*delta, totalLen);
		break;
		break;

	default:
		break;
	}
}



void display()
{

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 0); //color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?
	// intializeVectors();
	gluLookAt(curPos.x, curPos.y, curPos.z,curPos.x + l.x, curPos.y + l.y, curPos.z + l.z,u.x, u.y, u.z);	//gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	//gluLookAt(0,0,200,	0,0,0,	0,1,0);
	//gluLookAt(curPos.x, curPos.y, curPos.z,0,0,1,u.x, u.y, u.z);	//gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);

	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
	//glPushMatrix();
	float shift = (squareSide+2*cylRad)/2;
	{
		glTranslatef(-shift,-shift,-shift);
	}
	//glPopMatrix();
	drawAllCylinders();
	drawAllSquareSides();
	drawAllSpheres();
	
	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}

void animate()
{
	angle += 0.05;

	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init()
{
	//codes for initialization
	drawgrid = 0;
	drawaxes = 1;
	cameraHeight = 150.0;
	cameraAngle = 1.0;
	angle = 0;
	intializeVectors();
	//clear the screen
	glClearColor(0, 0, 0, 0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80, 1, 1, 1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB); //Depth, Double buffer, RGB color

	glutCreateWindow("My OpenGL Program");

	init();

	glEnable(GL_DEPTH_TEST); //enable Depth Testing

	glutDisplayFunc(display); //display callback function
	glutIdleFunc(animate);	//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);

	glutMainLoop(); //The main loop of OpenGL

	return 0;
}