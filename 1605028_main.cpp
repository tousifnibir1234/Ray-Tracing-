#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <bits/stdc++.h>
using namespace std;
#include <GL/glut.h>

// #include <windows.h>
// #include <glut.h>

double thetaAngle;
double gunAngle;
double gunBodyAngle;
double rotateAxis;

#define pi (2 * acos(0.0))

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;
double wall = 150;

struct point
{
	double x, y, z;
	point()
	{
	}
	point(double p, double q, double r)
	{
		x = p;
		y = q;
		z = r;
	}
	void print()
	{
		cout << x << " " << y << " " << z << endl;
	}
};
point sub(point a, point b)
{
	point temp;
	temp.x = a.x - b.x;
	temp.y = a.y - b.y;
	temp.z = a.z - b.z;
	return temp;
}
point add(point a, point b)
{
	point temp;
	temp.x = a.x + b.x;
	temp.y = a.y + b.y;
	temp.z = a.z + b.z;
	return temp;
}

point u(0, 0, 1);
point r(-1.0 / sqrt(2.0), 1.0 / sqrt(2.0), 0);
point l(-1.0 / sqrt(2), -1.0 / sqrt(2), 0);
point pos(100, 100, 0);

vector<point> box;

point multiplication(point a, point b)
{
	point temp;
	temp.x = (a.y * b.z - a.z * b.y);
	temp.y = (a.z * b.x - a.x * b.z);
	temp.z = (a.x * b.y - a.y * b.x);
	return temp;
}
point rotate(point rotating, point reference, double theta)
{
	point temp;
	point p = multiplication(reference, rotating);
	temp.x = rotating.x * cos(theta) + p.x * sin(theta);
	temp.y = rotating.y * cos(theta) + p.y * sin(theta);
	temp.z = rotating.z * cos(theta) + p.z * sin(theta);

	return temp;
}

double getdist(point a)
{
	return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}
point normalize(point a)
{
	double dist = sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
	a.x = a.x / (1.0 * dist);
	a.y = a.y / (1.0 * dist);
	a.z = a.y / (1.0 * dist);
	return a;
}
point multiply(point a, double value)
{
	a.x = a.x * value;
	a.y = a.y * value;
	a.z = a.z * value;
	return a;
}

void drawAxes()
{
	if (drawaxes == 1)
	{
		glBegin(GL_LINES);
		{
			glColor3f(1.0, 0, 0);

			glVertex3f(100, 0, 0);
			glVertex3f(-100, 0, 0);

			glColor3f(0, 1.0, 0);
			glVertex3f(0, -100, 0);
			glVertex3f(0, 100, 0);

			glColor3f(0, 0, 1);
			glVertex3f(0, 0, 100);
			glVertex3f(0, 0, -100);
		}
		glEnd();
	}
}

void drawGrid()
{
	int i;
	if (drawgrid == 1)
	{
		glColor3f(0.6, 0.6, 0.6); //grey
		glBegin(GL_LINES);
		{
			for (i = -8; i <= 8; i++)
			{

				if (i == 0)
					continue; //SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i * 10, -90, 0);
				glVertex3f(i * 10, 90, 0);

				//lines parallel to X-axis
				glVertex3f(-90, i * 10, 0);
				glVertex3f(90, i * 10, 0);
			}
		}
		glEnd();
	}
}

void keyboardListener(unsigned char key, int x, int y)
{
	switch (key)
	{

	case '1':
		l = rotate(l, u, pi / 30);
		r = rotate(r, u, pi / 30);
		break;
	case '2':
		l = rotate(l, u, -pi / 30);
		r = rotate(r, u, -pi / 30);
		break;
	case '3':
		l = rotate(l, r, pi / 30);
		u = rotate(u, r, pi / 30);
		break;
	case '4':
		l = rotate(l, r, -pi / 30);
		u = rotate(u, r, -pi / 30);
		break;
	case '5':
		r = rotate(r, l, -pi / 30);
		u = rotate(u, l, -pi / 30);
		break;
	case '6':
		r = rotate(r, l, pi / 30);
		u = rotate(u, l, pi / 30);
		break;

	case 'q':
		if (thetaAngle + 1 < 45)
		{
			thetaAngle += 1;
		}
		break;
	case 'w':
		if (thetaAngle - 1 > -45)
		{
			thetaAngle -= 1;
		}
		break;
	case 'e':
		if (gunAngle + 1 < 45)
		{
			gunAngle += 1;
		}
		break;
	case 'r':
		if (gunAngle - 1 > -45)
		{
			gunAngle -= 1;
		}
		break;
	case 'a':
		if (gunBodyAngle + 1 < 30)
		{
			gunBodyAngle += 1;
		}
		break;
	case 's':
		if (gunBodyAngle - 1 > -30)
		{
			gunBodyAngle -= 1;
		}
		break;
	case 'd':
		if (rotateAxis + 1 < 20)
		{
			rotateAxis += 1;
		}

		break;
	case 'f':
		if (rotateAxis - 1 > -20)
		{
			rotateAxis -= 1;
		}
		break;
	default:
		break;
	}
}

void specialKeyListener(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_DOWN: //down arrow key
		pos = sub(pos, l);
		break;
	case GLUT_KEY_UP: // up arrow key
		pos = add(pos, l);
		break;

	case GLUT_KEY_RIGHT:
		pos = add(pos, r);
		break;
	case GLUT_KEY_LEFT:
		pos = sub(pos, r);
		break;

	case GLUT_KEY_PAGE_UP:
		pos = add(pos, u);
		break;
	case GLUT_KEY_PAGE_DOWN:
		pos = sub(pos, u);
		break;

	case GLUT_KEY_INSERT:
		break;

	case GLUT_KEY_HOME:
		break;
	case GLUT_KEY_END:
		break;

	default:
		break;
	}
}

void mouseListener(int button, int state, int x, int y)
{ //x, y is the x-y of the screen (2D)
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		break;

	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{ // 2 times?? in ONE click? -- solution is checking DOWN or UP
			drawaxes = 1 - drawaxes;
		}
		//........
		break;

	case GLUT_MIDDLE_BUTTON:
		//........
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

	//gluLookAt(100,100,100,	0,0,0,	0,0,1);
	//gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	gluLookAt(pos.x, pos.y, pos.z, pos.x + l.x, pos.y + l.y, pos.z + l.z, u.x, u.y, u.z);

	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);

	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
	drawGrid();

	//glColor3f(1,0,0);
	//drawSquare(10);

	// drawSS();

	//drawCircle(30,24);

	//drawCone(20,50,24);

	// drawOffline();

	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}

void animate()
{
	angle += 0.05;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}
int recursionLevel, pixelDimension, objectCount;
class Object
{
public:
	point referencePoint;
	double height, width, length;
	double color[3];
	double coEfficients[4];
	int shine;
	virtual void draw() {}
	void setColor(double r, double g, double b)
	{
		color[0] = r;
		color[1] = g;
		color[2] = b;
	}
	void setShine(int s)
	{
		shine = s;
	}
	void setReferencePoint(point p)
	{
		referencePoint = p;
	}
	void setObjectFeatures(double length, double width, double height)
	{
		this->length = length;
		this->width = width;
		this->height = height;
	}
	void setCoEfficients(double ambient, double diffuse, double specular, double recursive)
	{
		coEfficients[0] = ambient;
		coEfficients[1] = diffuse;
		coEfficients[2] = specular;
		coEfficients[3] = recursive;
	}
	Object() {}
};

class sphere : public Object
{
public:
	point center;
	double radius;
	sphere(point center, double radius)
	{
		this->center = center;
		this->radius = radius;
	}
	void draw(int slices = 50, int stacks = 50)
	{
		glColor3f(color[0], color[1], color[2]);
		glPushMatrix();
		glTranslatef(center.x, center.y, center.z);

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
				points[i][j].x = r * cos(((double)j / (double)slices) * 2 * pi);
				points[i][j].y = r * sin(((double)j / (double)slices) * 2 * pi);
				points[i][j].z = h;
			}
		}
		//draw quads using generated points
		for (i = 0; i < stacks; i++)
		{
			for (j = 0; j < slices; j++)
			{
				glColor3f(j % 2, j % 2, j % 2);

				glBegin(GL_QUADS);
				{
					//upper hemisphere
					glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
					glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z);
					glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, points[i + 1][j + 1].z);
					glVertex3f(points[i + 1][j].x, points[i + 1][j].y, points[i + 1][j].z);
					//lower hemisphere
					glVertex3f(points[i][j].x, points[i][j].y, -points[i][j].z);
					glVertex3f(points[i][j + 1].x, points[i][j + 1].y, -points[i][j + 1].z);
					glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, -points[i + 1][j + 1].z);
					glVertex3f(points[i + 1][j].x, points[i + 1][j].y, -points[i + 1][j].z);
				}
				glEnd();
			}
		}
		glPopMatrix();
	}
};
class Triangle : public Object
{
public:
	point p1, p2, p3;
	Triangle(point a, point b, point c)
	{
		this->p1 = a;
		this->p2 = b;
		this->p3 = c;
	}
	void draw()
	{
		glColor3f(color[0], color[1], color[2]);
		glPushMatrix();
		glBegin(GL_TRIANGLES);
		{
			glVertex3f(p1.x, p1.y, p1.z);
			glVertex3f(p2.x, p2.y, p2.z);
			glVertex3f(p3.x, p3.y, p3.z);
		}
		glEnd();
		glPopMatrix();
	}
};
class Floor : public Object
{
public:
	double floorWidth, tileWidth;
	Floor(double floorWidth, double tileWidth)
	{
		point ref(-floorWidth / 2.0, -floorWidth / 2.0, 0);
		this->floorWidth = floorWidth;
		this->tileWidth = tileWidth;
		this->setReferencePoint(ref);
	}
	void draw()
	{
		int color = 0;
		for (int x = referencePoint.x; x <= (-referencePoint.x); x += tileWidth)
		{
			for (int y = referencePoint.y; y <= (-referencePoint.y); y += tileWidth)
			{
				color = (color++) % 2;
				glColor3f(color, color, color);

				glBegin(GL_QUADS);
				glVertex3f(x, y, 0);
				glVertex3f(x + tileWidth, y, 0);
				glVertex3f(x, y + tileWidth, 0);
				glVertex3f(x + tileWidth, y + tileWidth, 0);
				glEnd();
			}
		}
	}
};

class General : public Object
{
public:
	double a, b, c, d, e, f, g, h, i, j;
	General(double a, double b, double c, double d, double e, double f, double g, double h,
			double i, double j)
	{
		this->a = a;
		this->b = b;
		this->c = c;
		this->d = d;
		this->e = e;
		this->f = f;
		this->g = g;
		this->h = h;
		this->i = i;
		this->j = j;
	}
};
vector<Object *> objects;
vector<point> lightArray;

void loadData()
{
	FILE *fin = freopen("scene.txt", "r", stdin);
	cin >> recursionLevel >> pixelDimension >> objectCount;
	Object *temp;
	string type;
	double red, green, blue;
	double ambient, diffuse, specular, reflection;
	int shine;
	for (int i = 0; i < objectCount; i++)
	{
		cin >> type;
		if (type == "sphere")
		{
			point c;
			double r;
			cin >> c.x >> c.y >> c.z;
			cin >> r;
			temp = new sphere(c, r);
			cin >> red >> green >> blue;
			cin >> ambient >> diffuse >> specular >> reflection;
			cin >> shine;
			temp->setColor(red, green, blue);
			temp->setCoEfficients(ambient, diffuse, specular, reflection);
			temp->setShine(shine);
			objects.push_back(temp);
		}
		else if (type == "triangle")
		{
			point p1, p2, p3;
			cin >> p1.x >> p1.y >> p1.z;
			cin >> p2.x >> p2.y >> p2.z;
			cin >> p3.x >> p3.y >> p3.z;
			temp = new Triangle(p1, p2, p3);
			cin >> red >> green >> blue;
			cin >> ambient >> diffuse >> specular >> reflection;
			cin >> shine;
			temp->setColor(red, green, blue);
			temp->setCoEfficients(ambient, diffuse, specular, reflection);
			temp->setShine(shine);
			objects.push_back(temp);
		}
		else if (type == "general")
		{
			double a, b, c, d, e, f, g, h, i, j;
			double height, width, length;
			point ref;
			cin >> a >> b >> c >> d >> e >> f >> g >> h >> i >> j;
			temp = new General(a, b, c, d, e, f, g, h, i, j);
			cin >> ref.x >> ref.y >> ref.z;
			cin >> length >> width >> height;
			temp->setReferencePoint(ref);
			temp->setObjectFeatures(length, width, height);

			cin >> red >> green >> blue;
			cin >> ambient >> diffuse >> specular >> reflection;
			cin >> shine;
			temp->setColor(red, green, blue);
			temp->setCoEfficients(ambient, diffuse, specular, reflection);
			temp->setShine(shine);
			objects.push_back(temp);
		}
	}
	int lightCount;
	cin >> lightCount;
	for (int i = 0; i < lightCount; i++)
	{
		point l;
		cin >> l.x >> l.y >> l.z;
		lightArray.push_back(l);
	}
}

void init()
{
	//codes for initialization
	drawgrid = 0;
	drawaxes = 1;
	cameraHeight = 150.0;
	cameraAngle = 1.0;
	angle = 0;
	thetaAngle = 0;
	gunAngle = 0;
	gunBodyAngle = 0;
	rotateAxis = 0;

	loadData();

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
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB); //Depth, Double buffer, RGB color

	glutCreateWindow("My OpenGL Program");

	init();

	glEnable(GL_DEPTH_TEST); //enable Depth Testing

	glutDisplayFunc(display); //display callback function
	glutIdleFunc(animate);	  //what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop(); //The main loop of OpenGL

	return 0;
}