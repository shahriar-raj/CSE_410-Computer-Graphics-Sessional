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
//Declaring All the classes at first

struct Points{
    double x, y, z;
    Points(){};
    Points(double x, double y, double z){
        this->x = x;
        this->y = y;
        this->z = z;
    }
};
//Points work as the Vector3D specified in the specs

//Addition of Vectors
Points operator+(Points a, Points b){
    return Points(a.x+b.x, a.y+b.y, a.z+b.z);
}

//Subtraction of Vectors
Points operator-(Points a, Points b){
    return Points(a.x-b.x, a.y-b.y, a.z-b.z);
}

//Dot Product of Vectors which returns a scalar value
double operator*(Points a, Points b){
    return (a.x*b.x+a.y*b.y+a.z*b.z); // I was returning a Points class and I don't know how that was working
}

//Scalar Product Of Vectors. Returns a vector (Points) object
Points operator|(Points a, Points b){
    return Points(a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x);
}

//Scalar Multiplication (I mean Multiplication of Points with a scalar value)
Points operator*(Points a, double b){
    return Points(a.x*b, a.y*b, a.z*b);
}

//Rodrigues Formula
Points Rodrigues(Points a, Points b, double theta){
    Points c = a*cos(theta) + (b|a)*sin(theta) + (b*(a*b))*(1-cos(theta));
    return c;
}

Points Normalize(Points a){
    double r = sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
    return Points(a.x/r, a.y/r, a.z/r);
}

class Color{// At First I didn't want to use color as a clas. But It was getting troublesome to maintain
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

    Color operator*(Color c){
        return Color(r*c.r, g*c.g, b*c.b);
    }

    Color operator+(Color c){
        return Color(r+c.r, g+c.g, b+c.b);
    }

    Color operator+(double a){
        return Color(r+a, g+a, b+a);
    }

    // Normalizing the color so that it doesn't exceed 1 also doesn't go below 0
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

extern vector<PointLight> pointLights;
extern vector<SpotLight> spotLights;
extern vector<Object*> objects;
extern int n_recursion;
// Ray . The Parametric equation : R = R_o + t*R_d
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

class PointLight{
public:
    Points position;
    Color color;
    PointLight(){};
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
    SpotLight(){};
    void draw(){
        glPushMatrix();
        glTranslated(pl.position.x, pl.position.y, pl.position.z);
        glColor3f(pl.color.r, pl.color.g, pl.color.b);
        glutSolidSphere(0.5, 100, 100);
        glPopMatrix();

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
        return color; // p is  the intersection point
    }

    virtual Points getNormal(Points &p){
        return Points(0, 0, 0); // p is  the intersection point
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
        for (PointLight pl: pointLights) {
            Points L = pl.position - intersectionPoint;
            double t;
            L = Normalize(L);

            Points  lRayStart = intersectionPoint + L*0.0001;
            Ray lRay(lRayStart, L);
            bool isInShadow = false;
            double t_min_og = 999999;
            Color lColor;

            for (Object* o: objects) {
                t = o->intersect(lRay, lColor, 0);
                if (t > 0 && t < t_min_og) {
                    t_min_og = t;
                }
            }
            
            if(t_min < t_min_og)
            {
                Points R = normal * (2 * (normal * L)) - L;
                R = Normalize(R);

                double lambertValue = max((normal*L), 0.0);
                double phongValue = max(pow((R * r.dir), shine), 0.0);
                
                color = color + this->getColor(intersectionPoint) * pl.color * this->coEfficients[1] * lambertValue ;
                color.Normalize();
                color = color + pl.color * this->coEfficients[2] * phongValue;
                color.Normalize();
            }
        }
        
        for(SpotLight sl: spotLights){
            Points L = sl.direction;
            double t;
            L = Normalize(L);
            Points li = intersectionPoint - sl.pl.position;
            li = Normalize(li);

            double angle = (acos(L*li))*(180/3.1416);

            if(angle>=sl.angle) continue;

            L = sl.pl.position - intersectionPoint;
            L = Normalize(L);
            Points lRayStart = intersectionPoint + L*0.0001;
            Ray lRay(lRayStart, L);

            Color lColor;
            double t_min_og = 999999;

            for (Object* o: objects) {
                t = o->intersect(lRay, lColor, 0);
                if (t > 0 && t < t_min_og) {
                    t_min_og = t;
                }
            }
                
            if(t_min < t_min_og)
            {
                Points R = normal * (2 * (normal * L)) - L;
                R = Normalize(R);

                double lambertValue = max((normal*L), 0.0);
                double phongValue = max(pow((R * r.dir), shine), 0.0);
                
                color = color + this->getColor(intersectionPoint) * sl.pl.color * this->coEfficients[1] * lambertValue ;
                color.Normalize();
                color = color + sl.pl.color * this->coEfficients[2] * phongValue;
                color.Normalize();
            }
        }

        if(level >= n_recursion) return t_min;

        Points R = r.dir - normal * 2 * (normal * r.dir);
        R = Normalize(R);
        Ray reflectedRay(intersectionPoint + R*0.0001, R);
        Color reflectedColor;
        int nearest = -1;
        double t_ref, t_min_ref = 999999;

        for (int i = 0; i < objects.size(); i++) {
            t_ref = objects[i]->intersect(reflectedRay, reflectedColor, 0);
            if (t_ref > 0 && t_ref < t_min_ref) {
                t_min_ref = t_ref;
                nearest = i;
            }
        }

        if (nearest != -1) {
            t_min_ref = objects[nearest]->intersect(reflectedRay, reflectedColor, level + 1);
            color = color + reflectedColor * coEfficients[3];
            color.Normalize();
        }

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
        //Here p is the intersection point
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
        //Here p is the intersection point
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
        //Here p is the intersection point
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
   General(double A, double B, double C, double D, double E, double F, double G, double H, double I, double J, Points referencePoint, double length, double width, double height){
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

    bool isWithinRange(Points point)
    {
        if (point.x >= referencePoint.x && point.x <= referencePoint.x + width && point.y >= referencePoint.y && point.y <= referencePoint.y + height && point.z >= referencePoint.z && point.z <= referencePoint.z + length)
            return true;
        return false;
    }

    Points getNormal(Points &p) override{
        //Here p is the intersection point
        double x = 2*A*p.x + D*p.y + E*p.z + G;
        double y = 2*B*p.y + D*p.x + F*p.z + H;
        double z = 2*C*p.z + E*p.x + F*p.y + I;
        return Normalize(Points(x, y, z));
    }

    double getTminimum(Ray &ray){
        double a = A*ray.dir.x*ray.dir.x + B*ray.dir.y*ray.dir.y + C*ray.dir.z*ray.dir.z + D*ray.dir.x*ray.dir.y + E*ray.dir.x*ray.dir.z + F*ray.dir.y*ray.dir.z;
        double b = 2*A*ray.start.x*ray.dir.x + 2*B*ray.start.y*ray.dir.y + 2*C*ray.start.z*ray.dir.z + D*ray.start.x*ray.dir.y + E*ray.start.x*ray.dir.z + F*ray.start.y*ray.dir.z + G*ray.dir.x + H*ray.dir.y + I*ray.dir.z;
        double c = A*ray.start.x*ray.start.x + B*ray.start.y*ray.start.y + C*ray.start.z*ray.start.z + D*ray.start.x*ray.start.y + E*ray.start.x*ray.start.z + F*ray.start.y*ray.start.z + G*ray.start.x + H*ray.start.y + I*ray.start.z + J;
        double d = b*b - 4*a*c;
        if(d<0) return -1;
        double t1 = (-b + sqrt(d))/(2.0*a);
        double t2 = (-b - sqrt(d))/(2.0*a);
        Points p1 = ray.start + ray.dir*t1;
        Points p2 = ray.start + ray.dir*t2;
        if(t1<0 && t2<0) return -1;
        if(t1<0 && isWithinRange(p2)) return t2;
        if(t2<0 && isWithinRange(p1)) return t1;
        return min(t1, t2);
    }

};



