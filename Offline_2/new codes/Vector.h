#include <bits/stdc++.h>

using namespace std;

struct Vector{
    double x, y, z;
    Vector(double x, double y, double z){
        this->x = x;
        this->y = y;
        this->z = z;
    }
};

Vector operator+(Vector a, Vector b){
    return Vector(a.x+b.x, a.y+b.y, a.z+b.z);
}

Vector operator-(Vector a, Vector b){
    return Vector(a.x-b.x, a.y-b.y, a.z-b.z);
}

Vector operator*(Vector a, Vector b){
    return Vector(a.x*b.x, a.y*b.y, a.z*b.z);
}

Vector operator|(Vector a, Vector b){
    return Vector(a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x);
}

Vector operator*(Vector a, double b){
    return Vector(a.x*b, a.y*b, a.z*b);
}

Vector Rodrigues(Vector a, Vector b, double theta){
    Vector c = a*cos(theta) + (b|a)*sin(theta) + ((a*b)*b)*(1-cos(theta));
    return c;
}

Vector Normalize(Vector a){
    double r = sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
    return Vector(a.x/r, a.y/r, a.z/r);
}
