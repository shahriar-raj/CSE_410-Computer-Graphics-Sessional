#include <bits/stdc++.h>
#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <windows.h>
#include <glut.h>
#endif

using namespace std;

struct Points{
    double x, y, z;
    Points(){};
    Points(double x, double y, double z){
        this->x = x;
        this->y = y;
        this->z = z;
    }
};

Points operator+(Points a, Points b){
    return Points(a.x+b.x, a.y+b.y, a.z+b.z);
}

Points operator-(Points a, Points b){
    return Points(a.x-b.x, a.y-b.y, a.z-b.z);
}

Points operator*(Points a, Points b){
    return Points(a.x*b.x, a.y*b.y, a.z*b.z);
}

Points operator|(Points a, Points b){
    return Points(a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x);
}

Points operator*(Points a, double b){
    return Points(a.x*b, a.y*b, a.z*b);
}

Points Rodrigues(Points a, Points b, double theta){
    Points c = a*cos(theta) + (b|a)*sin(theta) + ((a*b)*b)*(1-cos(theta));
    return c;
}

Points Normalize(Points a){
    double r = sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
    return Points(a.x/r, a.y/r, a.z/r);
}

class Object{
public:
    Points referencePoint;
    double height, width, length;
    double color[3];
    double coEfficients[4];
    int shine;
    Object(){};
    virtual void draw(){};

    void setColor(double r, double g, double b){
        color[0] = r;
        color[1] = g;
        color[2] = b;
    }

    void setCoEfficients(double a, double b, double c, double d){
        coEfficients[0] = a;
        coEfficients[1] = b;
        coEfficients[2] = c;
        coEfficients[3] = d;
    }

    void setShine(int s){
        shine = s;
    }



};

class Sphere: public Object{
public:
    Sphere(){};
    Sphere(Points center, double radius){
        referencePoint = center;
        length = radius;
     }

     void draw() override {
        // cout << "YES Drawing\n";
        glPushMatrix();
        glTranslated(referencePoint.x, referencePoint.y, referencePoint.z);
        glColor3f(color[0], color[1], color[2]);
        glutSolidSphere(length, 100, 100);
        glPopMatrix();
     }
};

class Triangle: public Object{
    public:
    Points a, b, c;
    Triangle(){};
    Triangle(Points a, Points b, Points c){
        this->a = a;
        this->b = b;
        this->c = c;
    }
    void draw() override{
        glPushMatrix();
        glBegin(GL_TRIANGLES);
        glColor3f(color[0], color[1], color[2]);
        glVertex3f(a.x, a.y, a.z);
        glVertex3f(b.x, b.y, b.z);
        glVertex3f(c.x, c.y, c.z);
        glEnd();
        glPopMatrix();
    }
};

class PointLight{
public:
    Points position;
    double color[3];
    void draw(){
        glPushMatrix();
        glTranslated(position.x, position.y, position.z);
        glColor3f(color[0], color[1], color[2]);
        glutSolidSphere(1, 100, 100);
        glPopMatrix();
    }
};

class SpotLight{
public:
    PointLight pl;
    Points direction;
    double angle;
    void draw(){
        glPushMatrix();
        glTranslated(pl.position.x, pl.position.y, pl.position.z);
        glColor3f(pl.color[0], pl.color[1], pl.color[2]);
        glutSolidSphere(0.5, 100, 100);
        glPopMatrix();

    }
};

extern vector<Sphere> spheres;
extern vector<Triangle> triangles;
extern vector<PointLight> pointLights;
extern vector<SpotLight> spotLights;
extern vector<Object*> objects;