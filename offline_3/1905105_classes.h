#include <bits/stdc++.h>
#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <windows.h>
#include <glut.h>
#endif

using namespace std;

class Ray;
class Object;
class Sphere;
class Triangle;
class Floor;
class General;
class PointLight;
class SpotLight;
class Color;

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

double operator*(Points a, Points b){
    return (a.x*b.x+a.y*b.y+a.z*b.z);
}

Points operator|(Points a, Points b){
    return Points(a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x);
}

Points operator*(Points a, double b){
    return Points(a.x*b, a.y*b, a.z*b);
}

Points Rodrigues(Points a, Points b, double theta){
    Points c = a*cos(theta) + (b|a)*sin(theta) + (b*(a*b))*(1-cos(theta));
    return c;
}

Points Normalize(Points a){
    double r = sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
    return Points(a.x/r, a.y/r, a.z/r);
}

class Color{
public:
    double r, g, b;
    Color(){};
    
    Color(double r, double g, double b){
        this->r = r;
        this->g = g;
        this->b = b;
    }

    Color (const Color &c){
        this->r = c.r;
        this->g = c.g;
        this->b = c.b;
    }
    
    Color operator*(double a){
        return Color(r*a, g*a, b*a);
    }

    Color operator+(Color c){
        return Color(r+c.r, g+c.g, b+c.b);
    }

    void Normalize()
	{
		if (r < 0)
			r = 0;
		else if (r > 1)
			r = 1;
        if (g < 0)
			g = 0;
		else if (g>1)
            g = 1;
        if (b < 0)
			b = 0;
		else if (b>1)
            b = 1;  
    }
};

class Ray{
public:
    Points start, dir;
    Ray(){};
    
    Ray(Points start, Points dir){
        this->start = start;
        this->dir = Normalize(dir);
    }

    Ray(const Ray &r){
        this->start = r.start;
        this->dir = r.dir;
    }
};

class Object{
public:
    Points referencePoint;
    double height, width, length;
    //double color[3];
    Color color;
    double coEfficients[4];
    int shine;
    Object(){};
    virtual void draw(){};

    void setColor(double r, double g, double b){
        color.r = r;
        color.g = g;
        color.b = b;
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

    virtual double getTminimum(Ray &ray)
	{
		return -1.0;
	}

    virtual Color getColor(Points &p){
        return color;
    }

    virtual Points getNormal(Points &p){
        return Points(0, 0, 0);
    }

    double intersect (Ray &r, Color &color, int level) {
        double t_min = getTminimum(r);
        if (level == 0) return t_min;
        Points intersectionPoint = r.start + r.dir*t_min;
        Points normal = getNormal(intersectionPoint);
        normal = Normalize(normal);
        color = getColor(intersectionPoint) * coEfficients[0];
        color.Normalize();
        // cout << "Color: " << color.r << " " << color.g << " " << color.b << endl;
        return t_min;
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
        glColor3f(color.r, color.g, color.b);
        glutSolidSphere(length, 100, 100);
        glPopMatrix();
    }

    Color getColor(Points &p){
        return color;
    }

    Points getNormal(Points &p){
        return Normalize(p - referencePoint);
    }

    double getTminimum(Ray &ray){
        Points R_o = ray.start - referencePoint;
        double a = 1;
        double b = 2.0*(ray.dir*R_o);
        double c = (R_o*R_o) - length*length;
        double d = b*b - 4*a*c;
        if(d<0) return -1;
        double t1 = (-b + sqrt(d))/(2*a);
        double t2 = (-b - sqrt(d))/(2*a);
        if(t1<0 && t2<0) return -1;
        if(t1<0) return t2;
        if(t2<0) return t1;
        return min(t1, t2); // As sqrt(d) is always positive, t2 is always smaller than t1
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
        glColor3f(color.r, color.g, color.b);
        glVertex3f(a.x, a.y, a.z);
        glVertex3f(b.x, b.y, b.z);
        glVertex3f(c.x, c.y, c.z);
        glEnd();
        glPopMatrix();
    }

    Points getNormal(Points &p){
        Points v1 = b - a;
        Points v2 = c - a;
        return Normalize(v1|v2);
    }

    double getTminimum(Ray &ray){
        Points e1 = b - a;
        Points e2 = c - a;
        Points q = ray.dir|e2;
        double a1 = e1*q;
        if(a1>-0.00001 && a1<0.00001) return -1;
        double f = 1.0/a1;
        Points s = ray.start - a;
        double u = f*(s*q);
        if(u<0.0 || u>1.0) return -1;
        Points r = s|e1;
        double v = f*(ray.dir*r);
        if(v<0.0 || u+v>1.0) return -1;
        double t = f*(e2*r);
        if(t>0.00001) return t;
        return -1;
    }
};

class Floor: public Object{
public:
    double floorWidth, tileWidth;
    Floor(){};
    Floor(double floorWidth, double tileWidth){
        this->floorWidth = floorWidth;
        this->tileWidth = tileWidth;
        this -> referencePoint = Points(-floorWidth/2, -floorWidth/2, 0);
        this -> length = tileWidth;
    }

    void draw(){
        glPushMatrix();
        glBegin(GL_QUADS);
        for(int i=0; i<floorWidth; i+=tileWidth){
            for(int j=0; j<floorWidth; j+=tileWidth){
                if((int)((i+j)/tileWidth)%2== 0){
                    glColor3f(1, 1, 1);
                }
                else{
                    glColor3f(0, 0, 0);
                }
                glVertex3f(referencePoint.x+i, referencePoint.y+j, referencePoint.z);
                glVertex3f(referencePoint.x+i+tileWidth, referencePoint.y+j, referencePoint.z);
                glVertex3f(referencePoint.x+i+tileWidth, referencePoint.y+j+tileWidth, referencePoint.z);
                glVertex3f(referencePoint.x+i, referencePoint.y+j+tileWidth, referencePoint.z);
            }
        }
        glEnd();
        glPopMatrix();
    }

    Points getNormal(Points &p) override{
        return Points(0, 0, 1);
    }

    Color getColor(Points &p) override{
        int x = (int) ((p.x - referencePoint.x)/tileWidth);
        int y = (int) ((p.y - referencePoint.y)/tileWidth);
        if((x+y)%2==0){
            return Color(1, 1, 1);
        }
        else{
            return Color(0, 0, 0);
        }
    }

    double getTminimum(Ray &ray){
        double t = -ray.start.z/ray.dir.z;
        Points p = ray.start + ray.dir*t;
        if(p.x < referencePoint.x || p.x > -referencePoint.x || p.y < referencePoint.y || p.y > -referencePoint.y) return -1;
        return t;
    }
};

class General : public Object{
public:
   // General is for Quadratic Surfaces
   double A, B, C, D, E, F, G, H, I, J;
   General(){};
   General(double A, double B, double C, double D, double E, double F, double G, double H, double I, double J, Points referencePoint, double height, double width, double length){
       this->A = A;
       this->B = B;
       this->C = C;
       this->D = D;
       this->E = E;
       this->F = F;
       this->G = G;
       this->H = H;
       this->I = I;
       this->J = J;
       this->referencePoint = referencePoint;
       this->height = height;
       this->width = width;
       this->length = length;
   }

    void draw() override{
         // cout << "Drawing General\n";
    }

};

class PointLight{
public:
    Points position;
    Color color;
    void draw(){
        glPushMatrix();
        glTranslated(position.x, position.y, position.z);
        glColor3f(color.r, color.g, color.b);
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
        glColor3f(pl.color.r, pl.color.g, pl.color.b);
        glutSolidSphere(0.5, 100, 100);
        glPopMatrix();

    }
};


extern vector<PointLight> pointLights;
extern vector<SpotLight> spotLights;
extern vector<Object*> objects;