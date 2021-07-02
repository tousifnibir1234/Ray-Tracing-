#include "bitmap_image.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <iostream>
#define CCW 1
#define CW -1
#define max(a, b) a > b ? a : b
#define min(a, b) a < b ? a : b
#include <GL/glut.h>
#define pi (2 * acos(0.0))

#define WINDOW_WIDTH 700
#define WINDOW_HEIGHT 700
#define VIEW_ANGLE 90
#define EPSILON 0.000001

#define AMBIENT 0
#define DIFFUSE 1
#define SPECULAR 2
#define REFLECTION 3
#define DEBUG 0

using namespace std;
double cameraHeight;
double cameraAngle;
double angle;
//---------------------------------------------UTIL.H--------------------------------------------------------
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
    void takeInput()
    {
        cin >> x >> y >> z;
    }
    point addPoints(point b)
    {
        return point(x + b.x, y + b.y, z + b.z);
    }
    point subPoints(point b)
    {
        return point(x - b.x, y - b.y, z - b.z);
    }
    point operator*(double val)
    {
        return point(x * val, y * val, z * val);
    }
    double distanceFrom(point b)
    {
        return sqrt((x - b.x) * (x - b.x) + (y - b.y) * (y - b.y) + (z - b.z) * (z - b.z));
    }
};
struct color
{
    double r, g, b;
};

class Vector
{
public:
    point p1;
    Vector(point p1)
    {
        this->p1 = p1;
    }

    Vector(double x, double y, double z)
    {
        this->p1.x = x;
        this->p1.y = y;
        this->p1.z = z;
    }
    Vector() {}
    Vector setVector(point p1)
    {
        this->p1 = p1;
        return *this;
    }
    double getMagnitude()
    {
        return sqrt(p1.x * p1.x + p1.y * p1.y + p1.z * p1.z);
    }
    void print()
    {
        p1.print();
    }
    point getDirectionPoint()
    {
        return p1;
    }
    Vector crossProduct(Vector v)
    {
        point p2 = v.getDirectionPoint();
        point n, ret;

        n.x = p1.y * p2.z - p1.z * p2.y;
        n.y = p1.z * p2.x - p1.x * p2.z;
        n.z = p1.x * p2.y - p1.y * p2.x;
        return Vector(n);
    }

    double dotProduct(Vector v)
    {
        double ret;
        point p2 = v.getDirectionPoint();
        ret = p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
        return ret;
    }
    double angleBetween(Vector v)
    {
        double cp = dotProduct(v);
        cp /= (getMagnitude() * v.getMagnitude());
        Vector rightVect = rotateAlong(Vector(point(0, 0, 1)), CW);
        v.print();
        rightVect.print();
        crossProduct(v).print();
        cout << v.dotProduct(rightVect) << "**" << endl;
        if (v.dotProduct(rightVect) < 0)
        {
            return acos(cp);
        }
        else
        {
            return -acos(cp);
        }
    }

    Vector rotateAlong(Vector refVector, int whichDir, double angle = 0.03)
    {
        point ref = refVector.getDirectionPoint();
        point n, ret;
        double theta = angle * whichDir;

        n.x = ref.y * p1.z - ref.z * p1.y;
        n.y = ref.z * p1.x - ref.x * p1.z;
        n.z = ref.x * p1.y - ref.y * p1.x;

        ret.x = p1.x * cos(theta) + n.x * sin(theta);
        ret.y = p1.y * cos(theta) + n.y * sin(theta);
        ret.z = p1.z * cos(theta) + n.z * sin(theta);

        return Vector(ret);
    }
    Vector normalize()
    {
        double mag = getMagnitude();
        p1.x /= mag;
        p1.y /= mag;
        p1.z /= mag;
        return *this;
    }

    Vector operator*(double val)
    {
        Vector tmp(p1.x, p1.y, p1.z);
        tmp.p1.x *= val, tmp.p1.y *= val, tmp.p1.z *= val;
        return tmp;
    }
    Vector operator+(Vector tmp)
    {
        return Vector(p1.x += tmp.p1.x, p1.y += tmp.p1.y, p1.z += tmp.p1.z);
    }
    Vector operator-(Vector tmp)
    {
        return Vector(p1.x -= tmp.p1.x, p1.y -= tmp.p1.y, p1.z -= tmp.p1.z);
    }
    Vector operator/(double val)
    {
        Vector tmp(p1.x, p1.y, p1.z);
        tmp.p1.x /= val, tmp.p1.y /= val, tmp.p1.z /= val;
        return tmp;
    }
};
//*******************************************DRAW OBJECT********************************************

//*******************************************DRAW OBJECT ENDS********************************************

void crop(double &value, double min_val, double max_val)
{
    value = min(max(min_val, value), max_val);
}

bool is_outside_range(double value, double min_val, double max_val)
{
    return (value < min_val) || (value > max_val);
}
//--------------------------------------------------------UTIL.H ENDS---------------------------------------------------------

//-----------------------------------------------------OBJECTS DEFINITION  BEGINS-----------------------------
class Ray
{
public:
    point start;
    Vector direction;
    Ray(point start, point p2)
    {
        this->start = start;
        point dirPoint = {p2.x - start.x, p2.y - start.y, p2.z - start.z};
        direction.setVector(dirPoint);
        direction.normalize();
    }
    Ray(point start, Vector direction)
    {
        this->start = start;
        this->direction = direction;
        this->direction.normalize();
    }
    Ray() {}
    void setPoints(point start, point p2)
    {
        this->start = start;
        point dirPoint = {p2.x - start.x, p2.y - start.y, p2.z - start.z};
        direction.setVector(dirPoint).normalize();
    }
    void print()
    {
        start.print();
        direction.print();
    }
};
class Object
{
public:
    char type = 'x';
    double source_factor = 1.0;
    double shine;
    double color[3];
    double co_efficients[4];

    Object() {}

    virtual void draw() = 0;
    virtual double getIntersectingT(Ray ray) = 0;
    virtual Vector getNormal(point intersection) = 0;

    void fill_color(Ray &ray, double t, double current_color[3], int level);

    void setColor(double r, double g, double b)
    {
        color[0] = r;
        color[1] = g;
        color[2] = b;
    }

    void setShine(int shine)
    {
        this->shine = shine;
    }

    void set_lighting_coefficients(double a, double d, double s, double r)
    {
        co_efficients[AMBIENT] = a;
        co_efficients[DIFFUSE] = d;
        co_efficients[SPECULAR] = s;
        co_efficients[REFLECTION] = r;
    }

    Vector get_reflected_ray_direction(Ray ray, Vector normal)
    {
        Vector out_dir = ray.direction - normal * 2.0 * ray.direction.dotProduct(normal);
        return out_dir.normalize();
    }
};
vector<Object *> objects;

class Sphere : public Object
{
public:
    point center;
    double radius;

    Sphere(point center, double radius)
    {
        this->center = center;
        this->radius = radius;
        type = 'S';
    }

    void draw()
    {
        glColor3f(color[0], color[1], color[2]);
        glPushMatrix();
        glTranslatef(center.x, center.y, center.z);
        glutSolidSphere(radius, 100, 100);
        glPopMatrix();
    }

    double getIntersectingT(Ray ray)
    {
        point c = center;
        point o = ray.start;
        Vector Ro(o.subPoints(c));
        Vector Rd = ray.direction;

        double tp = -(Ro.dotProduct(Rd));
        if (tp < 0)
        {
            return -1;
        }

        double d_2 = Ro.dotProduct(Ro) - tp * tp;
        if (d_2 > radius * radius)
            return -1;

        double t = sqrt(radius * radius - d_2);

        return min(tp - t, tp + t);
    }

    Vector getNormal(point intersection)
    {
        return Vector(intersection.subPoints(center)).normalize();
    }
};
class Triangle : public Object
{
public:
    point p1, p2, p3;
    double a, b, c, d;

    Triangle() {}
    Triangle(point a, point b, point c)
    {
        this->p1 = a, this->p2 = b, this->p3 = c;
        // calculating a, b, c, d
        // using parametric form
        Vector AB(p2.subPoints(p1));
        Vector AC(p3.subPoints(p1));

        Vector V = AB.crossProduct(AC);

        this->a = V.p1.x;
        this->b = V.p1.y;
        this->c = V.p1.z;
        this->d = this->a * p1.x + this->b * p1.y + this->c * p1.z;
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
    double getIntersectingT(Ray ray)
    {
        Vector normal = getNormal(point(0, 0, 0));

        double t;
        bool flag = false;
        double denom = normal.dotProduct(ray.direction);

        if (denom < 0.0)
        {
            normal = normal * -1.0;
            denom = normal.dotProduct(ray.direction);
        }

        // checking whether the ray intersects with the triangle's plane
        if (abs(denom) < EPSILON)
            return -1;

        t = normal.dotProduct(Vector(p1.subPoints(ray.start))) / denom;
        if (t >= 0)
            flag = true; // yes, intersects the plane

        if (!flag)
            return -1; // no

        // checking whether the point of intersection is inside the triangle
        bool b1, b2, b3;
        point intersectionPoint = ray.start.addPoints((ray.direction * t).getDirectionPoint());

        Vector N = Vector(p2.subPoints(p1)).crossProduct(Vector(p3.subPoints(p1)));

        Vector C, edge1, edge2;

        // checking if intersectionPoint is inside the triangle

        edge1 = Vector(p2.subPoints(p1));
        edge2 = Vector(intersectionPoint.subPoints(p1));
        C = edge1.crossProduct(edge2);
        if (N.dotProduct(C) < -EPSILON)
            return -1;

        edge1 = Vector(p3.subPoints(p2));
        edge2 = Vector(intersectionPoint.subPoints(p2));
        C = edge1.crossProduct(edge2);
        if (N.dotProduct(C) < -EPSILON)
            return -1;

        edge1 = Vector(p1.subPoints(p3));
        edge2 = Vector(intersectionPoint.subPoints(p3));
        C = edge1.crossProduct(edge2);
        if (N.dotProduct(C) < -EPSILON)
            return -1;

        return t; // else, yes
    }

    Vector getNormal(point intersection)
    {
        return Vector(point(a, b, c)).normalize();
    }
};
class Square : public Object
{
public:
    point a, b, c, d;
    Square(point a, point b, point c, point d)
    {
        this->a = a, this->b = b, this->c = c, this->d = d;
    }
    void draw()
    {
        glColor3f(color[0], color[1], color[2]);

        glBegin(GL_QUADS);
        {
            glVertex3f(a.x, a.y, a.z);
            glVertex3f(b.x, b.y, b.z);
            glVertex3f(c.x, c.y, c.z);
            glVertex3f(d.x, d.y, d.z);
        }
        glEnd();
    }
    double getIntersectingT(Ray ray)
    {
        return -1;
    }

    Vector getNormal(point intersection) {}
};

class Floor : public Object
{
    double tileWidth;
    int tileNum;
    point topLeftPoint;
    point origin;
    double totalWidth;

public:
    Floor(double tileWidth, int tileNum)
    {
        this->tileWidth = tileWidth;
        this->tileNum = tileNum;
        totalWidth = tileWidth * tileNum;
        topLeftPoint = point(-tileWidth * tileNum / 2.0, -tileWidth * tileNum / 2.0, 0);
    }

    void draw()
    {
        double col = 0;
        for (int i = 0; i < tileNum; i++)
        {
            col = 1 - col;

            for (int j = 0; j < tileNum; j++)
            {
                col = 1 - col;
                Square sq(point(topLeftPoint.x + tileWidth * i, topLeftPoint.y + tileWidth * j, topLeftPoint.z),
                          point(topLeftPoint.x + tileWidth * (i + 1), topLeftPoint.y + tileWidth * j, topLeftPoint.z),
                          point(topLeftPoint.x + tileWidth * (i + 1), topLeftPoint.y + tileWidth * (j + 1), topLeftPoint.z),
                          point(topLeftPoint.x + tileWidth * i, topLeftPoint.y + tileWidth * (j + 1), topLeftPoint.z));
                sq.setColor(col, col, col);
                sq.draw();
            }
        }
    }

    double getIntersectingT(Ray ray)
    {
        if (ray.direction.p1.z == 0)
            return -1;

        double t = -(ray.start.z / ray.direction.p1.z);
        point intersectionPoint = ray.start.addPoints((ray.direction * t).getDirectionPoint());

        double x = intersectionPoint.x - topLeftPoint.x;
        double y = intersectionPoint.y - topLeftPoint.y;
        if (is_outside_range(x, 0.0, totalWidth) || is_outside_range(y, 0.0, totalWidth))
            return -1;

        int pixel_x = (intersectionPoint.x - topLeftPoint.x) / tileWidth;
        int pixel_y = (intersectionPoint.y - topLeftPoint.y) / tileWidth;
        if (is_outside_range(pixel_x, 0, tileNum) || is_outside_range(pixel_y, 0, tileNum))
            return -1;

        int c = (pixel_x + pixel_y) % 2;

        color[0] = double(c);
        color[1] = double(c);
        color[2] = double(c);

        return t;
    }

    Vector getNormal(point intersection) { return Vector(0, 0, 1); }
};
//-----------------------------------------------------OBJECT DEFINITOIN ENDS  ----------------------------------

//-----------------------------------------------------INITIALIZATION PART BEGINS---------------------------------------------

int image_width, image_height;
int resolution;
int recursion_level;
vector<point> lights;
point curPos;
Vector u, r, l;

void intializeVectors()
{
    curPos = {100, 100, 58};
    u = Vector(0, 0, 1);
    l = Vector(-1 / sqrt(2), -1 / sqrt(2), 0);
    r = Vector(-1 / sqrt(2), 1 / sqrt(2), 0);
}

void create_floor()
{
    Object *floor = new Floor(30, 200);
    floor->set_lighting_coefficients(0.3, 0.3, 0.3, 0.2);
    floor->setShine(1.0);
    floor->type = 'f';
    objects.push_back(floor);
}
//-----------------------------------------------------INITIALIZATION PART ENDS---------------------------------------------

//-----------------------------------------------------FILE INPUT BEGINS ----------------------------------
void loadObjects()
{

    int n_objects, n_lights;
    string object_type;
    Object *obj;

    freopen("scene.txt", "r", stdin);

    cin >> recursion_level >> resolution >> n_objects;
    image_height = image_width;

    for (int i = 0; i < n_objects; i++)
    {
        cin >> object_type;
        double red, green, blue;
        double ambient, diffuse, specular, reflection;
        double shine;
        bool inserted = false;
        if (object_type == "sphere")
        {

            point center;  // center
            double radius; // radius

            center.takeInput();
            cin >> radius;
            obj = new Sphere(center, radius);
            cin >> red >> green >> blue;
            obj->setColor(red, green, blue);

            cin >> ambient >> diffuse >> specular >> reflection;
            obj->set_lighting_coefficients(ambient, diffuse, specular, reflection);

            cin >> shine;
            obj->setShine(shine);

            objects.push_back(obj);
        }
        if (object_type == "pyramid")
        {

            point lowCoord;     // center
            double len, height; // radius

            lowCoord.takeInput();
            cin >> len >> height;
            cin >> red >> green >> blue;
            cin >> ambient >> diffuse >> specular >> reflection;
            cin >> shine;

            point p[5];
            p[0] = lowCoord;
            p[1] = lowCoord.addPoints(point(len, 0, 0));
            p[2] = p[1].addPoints(point(0, len, 0));
            p[3] = p[0].addPoints(point(0, len, 0));
            p[4] = lowCoord.addPoints(point(len / 2.0, len / 2.0, height));
            Object *side[5];
            for (int i = 0; i < 4; i++)
            {
                side[i] = new Triangle(p[i], p[(i + 1) % 4], p[4]);
                side[i]->setColor(red, green, blue);
                objects.push_back(side[i]);
            }
            side[4] = new Square(p[0], p[1], p[2], p[3]);
            objects.push_back(side[4]);
            for (int i = 0; i < 5; i++)
            {
                side[i]->setShine(shine);
                side[i]->set_lighting_coefficients(ambient, diffuse, specular, reflection);
                side[i]->setColor(red, green, blue);
            }
        }
    }

    cin >> n_lights;
    for (int i = 0; i < n_lights; i++)
    {
        point light;
        light.takeInput();
        lights.push_back(light);
    }
}
//-----------------------------------------------------FILE INPUT BEGINS ----------------------------------
#define NEAR_PLANE 5
pair<int, double> geteClosestIntersection(Ray r)
{
    int nearest = -1;
    double t_min = __INT_MAX__;

    for (int k = 0; k < objects.size(); k++)
    {
        double tk = objects[k]->getIntersectingT(r);
        if (tk <= -EPSILON)
        {
            continue;
        }
        else if (tk + EPSILON < t_min)
        {
            // cout<<t_min<<endl;
            t_min = tk;
            nearest = k;
        }
    }

    return make_pair(nearest, t_min);
}

void Object::fill_color(Ray &ray, double t, double current_color[3], int level)
{
    point intersectionPoint = ray.start.addPoints((ray.direction * t).getDirectionPoint());
    Vector normal = getNormal(intersectionPoint).normalize();
    Vector reflection = get_reflected_ray_direction(ray, normal);

    for (int k = 0; k < 3; k++)
    {
        current_color[k] += (co_efficients[AMBIENT] * color[k]);
    }
    for (int i = 0; i < lights.size(); i++)
    {
        double lambert = 0.0, phong = 0.0;

        Vector dir = Vector(lights[i].subPoints(intersectionPoint)).normalize();

        point start = intersectionPoint.addPoints((dir * 0.2).getDirectionPoint());
        // adding epsilon so that the image itself is (not detected as a blockade
        Ray L(start, lights[i]);
        Vector R = Vector((normal * L.direction.dotProduct(normal) * 2.0) - L.direction).normalize();
        Vector V = (ray.direction * -1.0).normalize();

        bool hasBlock = false;
        double dis = intersectionPoint.distanceFrom(lights[i]);
        int n_objects = objects.size();
        for (int j = 0; j < n_objects; j++)
        {
            double t = objects[j]->getIntersectingT(L);
            if (t > 0)
            {
                hasBlock = true;
                break;
            }
        }

        if (!hasBlock)
        {
            lambert = source_factor * co_efficients[DIFFUSE] * (L.direction).dotProduct(normal);
            phong = co_efficients[SPECULAR] * pow(max(R.dotProduct(V), 0), shine);

            crop(lambert, 0.0, 1.0);
            crop(phong, 0.0, 1.0);
        }
        for (int k = 0; k < 3; k++)
        {
            current_color[k] += (lambert * color[k]) + phong;
        }
        if (level < recursion_level)
        {
            point start = intersectionPoint.addPoints((reflection * 0.02).getDirectionPoint());

            Ray reflectionRay(start, reflection);
            double reflected_color[3] = {0.0, 0.0, 0.0};

            pair<double, double> pair = geteClosestIntersection(reflectionRay);
            int nearest = pair.first;
            double t_min = pair.second;

            if (nearest != -1)
            {
                objects[nearest]->fill_color(reflectionRay, t_min, reflected_color, level + 1);
                for (int k = 0; k < 3; k++)
                {
                    current_color[k] += reflected_color[k] * co_efficients[REFLECTION];
                }
            }
        }

        for (int k = 0; k < 3; k++)
        {
            crop(current_color[k], EPSILON, 1.0 - EPSILON);
        }
    }
}

void capture()
{
    cout << " capture start" << endl;
    image_height = image_width = resolution;

    color **frameAra;
    frameAra = new color *[image_width];
    for (int i = 0; i < image_width; i++)
    {
        frameAra[i] = new color[image_height];
    }

    double plane_distance = (WINDOW_HEIGHT / 2) / tan(VIEW_ANGLE * pi / 360);
    point top_left = curPos.addPoints((l * plane_distance - r * (WINDOW_WIDTH / 2) + u * (WINDOW_HEIGHT / 2)).getDirectionPoint());

    double du = (WINDOW_WIDTH * 1.0) / image_width;
    double dv = (WINDOW_HEIGHT * 1.0) / image_height;

    // ray eqn
    for (int i = 0; i < resolution; i++)
    {
        for (int j = 0; j < resolution; j++)
        {

            point grid_point = top_left.addPoints((u * j * (-du) + r * i * du).getDirectionPoint());
            Ray ray(curPos, grid_point);
            if (DEBUG)
                grid_point.print();
            pair<int, double> pair = geteClosestIntersection(ray);
            int nearest = pair.first;
            double t_min = pair.second;
            double pixel_color[3] = {0.0, 0.0, 0.0};

            if (nearest != -1)
            {
                // continue;
                objects[nearest]->fill_color(ray, t_min, pixel_color, 1);
            }
            frameAra[i][j].r = pixel_color[0];
            frameAra[i][j].g = pixel_color[1];
            frameAra[i][j].b = pixel_color[2];
        }
        //cout << endl;
    }

    bitmap_image image(image_width, image_height);
    for (int j = 0; j < resolution; j++)
    {
        for (int i = 0; i < resolution; i++)
        {

            double r = frameAra[i][j].r;
            double g = frameAra[i][j].g;
            double b = frameAra[i][j].b;
            image.set_pixel(i, j, r * 255, g * 255, b * 255);
        }
    }

    image.save_image("outHelper1.bmp");

    cout << "\tSaved\n";
    cout << "\a";
}

void keyboardListener(unsigned char key, int x, int y)
{
    switch (key)
    {

    case '1':

        l = l.rotateAlong(u, CCW);
        r = r.rotateAlong(u, CCW);

        break;
    case '2':
        l = l.rotateAlong(u, CW);
        r = r.rotateAlong(u, CW);
        break;
    case '3':
        l = l.rotateAlong(r, CCW);
        u = u.rotateAlong(r, CCW);
        break;
    case '4':
        l = l.rotateAlong(r, CW);
        u = u.rotateAlong(r, CW);
        break;
    case '5':
        r = r.rotateAlong(l, CW);
        u = u.rotateAlong(l, CW);
        break;
    case '6':
        r = r.rotateAlong(l, CCW);
        u = u.rotateAlong(l, CCW);
        break;
    case '0':
        capture();
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
        curPos = curPos.subPoints(l.getDirectionPoint());

        break;
    case GLUT_KEY_UP: // up arrow key
        curPos = curPos.addPoints(l.getDirectionPoint());

        break;

    case GLUT_KEY_RIGHT:
        curPos = curPos.addPoints(r.getDirectionPoint());
        break;
    case GLUT_KEY_LEFT:
        curPos = curPos.subPoints(r.getDirectionPoint());

        break;
    case GLUT_KEY_PAGE_UP:
        curPos = curPos.addPoints(u.getDirectionPoint());
        break;
    case GLUT_KEY_PAGE_DOWN:
        curPos = curPos.subPoints(u.getDirectionPoint());
        break;

    case GLUT_KEY_INSERT:
        break;

    case GLUT_KEY_HOME:

    case GLUT_KEY_END:

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
    gluLookAt(curPos.x, curPos.y, curPos.z, curPos.x + l.p1.x, curPos.y + l.p1.y, curPos.z + l.p1.z, u.p1.x, u.p1.y, u.p1.z); //gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
    //again select MODEL-VIEW
    glMatrixMode(GL_MODELVIEW);

    /****************************
	/ Add your objects from here
	****************************/
    //add objects
    for (int i = 0; i < objects.size(); i++)
        objects[i]->draw();
    for (int i = 0; i < lights.size(); i++)
    {
        {
            glColor3f(1, 1, 1);
            glPushMatrix();
            glTranslatef(lights[i].x, lights[i].y, lights[i].z);
            glutSolidSphere(2, 100, 100);
            glPopMatrix();
        }
    }
    //ADD this Ray in the end --- if you use double buffer (i.e. GL_DOUBLE)
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

    cameraHeight = 150.0;
    cameraAngle = 1.0;
    angle = 0;
    intializeVectors();
    loadObjects();
    create_floor();
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
    gluPerspective(90, 1, 1, 1000.0);
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
    glutIdleFunc(animate);    //what you want to do in the idle time (when no drawing is occuring)

    glutKeyboardFunc(keyboardListener);
    glutSpecialFunc(specialKeyListener);

    glutMainLoop(); //The main loop of OpenGL

    return 0;
}