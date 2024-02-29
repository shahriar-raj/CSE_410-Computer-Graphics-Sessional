#include <bits/stdc++.h>
#include <cmath>

#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <windows.h>
#include <glut.h>
#endif
#define c_angle 70.5287794

using namespace std;

struct Points{
    double x, y, z;
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

int counter = 0;
Points eye(4, 4, 4);
Points look(-1.0/sqrt(3.0), -1.0/sqrt(3.0), -1.0/sqrt(3.0));
Points up(-1.0/sqrt(2.0), 1.0/sqrt(2.0), 0);
Points right_((1.0/sqrt(6.0)), (1.0/sqrt(6.0)), (-2.0/sqrt(6.0)));
double Radius=0;
double objAngle = 0;

vector<vector<Points>> drawUnitPositiveXSphere()
{
    const float DEG2RAD = acos(-1) / 180.0f;
    int subdivision = 3;
    vector<vector<Points>> vertices;
    float n1[3];        // normal of longitudinal plane rotating along Y-axis
    float n2[3];        // normal of latitudinal plane rotating along Z-axis
    float v[3];         // direction vector intersecting 2 planes, n1 x n2
    float a1;           // longitudinal angle along Y-axis
    float a2;           // latitudinal angle along Z-axis

    // compute the number of vertices per row, 2^n + 1
    int pointsPerRow = (int)pow(2, subdivision) + 1;

    // rotate latitudinal plane from 45 to -45 degrees along Z-axis (top-to-bottom)
    for(unsigned int i = 0; i < pointsPerRow; ++i)
    {
        // normal for latitudinal plane
        // if latitude angle is 0, then normal vector of latitude plane is n2=(0,1,0)
        // therefore, it is rotating (0,1,0) vector by latitude angle a2
        a2 = DEG2RAD * (45.0f - 90.0f * i / (pointsPerRow - 1));
        n2[0] = -sin(a2);
        n2[1] = cos(a2);
        n2[2] = 0;
        vertices.push_back(vector<Points>());
        // rotate longitudinal plane from -45 to 45 along Y-axis (left-to-right)
        for(unsigned int j = 0; j < pointsPerRow; ++j)
        {
            // normal for longitudinal plane
            // if longitude angle is 0, then normal vector of longitude is n1=(0,0,-1)
            // therefore, it is rotating (0,0,-1) vector by longitude angle a1
            a1 = DEG2RAD * (-45.0f + 90.0f * j / (pointsPerRow - 1));
            n1[0] = -sin(a1);
            n1[1] = 0;
            n1[2] = -cos(a1);

            // find direction vector of intersected line, n1 x n2
            v[0] = n1[1] * n2[2] - n1[2] * n2[1];
            v[1] = n1[2] * n2[0] - n1[0] * n2[2];
            v[2] = n1[0] * n2[1] - n1[1] * n2[0];

            // normalize direction vector
            float scale = 1 / sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
            v[0] *= scale;
            v[1] *= scale;
            v[2] *= scale;

            // add a vertex into 2D vector
            Points A(v[0],v[1],v[2]);
            vertices[i].push_back(A);
        }
    }

    glBegin(GL_QUADS);
        for(int i=0; i<pointsPerRow-1; i++){
            for(int j=0; j<pointsPerRow-1; j++){
                glVertex3f(vertices[i][j].x, vertices[i][j].y, vertices[i][j].z);
                glVertex3f(vertices[i][j+1].x, vertices[i][j+1].y, vertices[i][j+1].z);
                glVertex3f(vertices[i+1][j+1].x, vertices[i+1][j+1].y, vertices[i+1][j+1].z);
                glVertex3f(vertices[i+1][j].x, vertices[i+1][j].y, vertices[i+1][j].z);
            }
        }
    glEnd();
    return vertices;
}

void axes(){
    glBegin(GL_LINES);{
        glColor3f(0, 1.0, 0);
        glVertex3f(0,100,0);
        glVertex3f(0,-100,0);

        glColor3f(1.0, 0, 0);
        glVertex3f(100,0,0);
        glVertex3f(-100,0,0);

        glColor3f(1.0, 1.0, 1.0);
        glVertex3f(0,0,100);
        glVertex3f(0,0,-100);
    }glEnd();

}

void drawCompleteCubeSphere(){
    GLfloat r = 1-sqrt(2)*Radius;
    
    glColor3f(1, 0, 0);
    glPushMatrix();{
        glTranslatef(r, 0, 0);
        glScalef(Radius, Radius, Radius);
        drawUnitPositiveXSphere();
    }glPopMatrix();

    glPushMatrix();{
            glTranslatef(-r, 0, 0);
            glScalef(Radius, Radius, Radius);
            glRotatef(180, 0, 1, 0);
            drawUnitPositiveXSphere();
    }glPopMatrix();

    glColor3f(0, 1, 0);
    glPushMatrix();{
            glTranslatef(0, 0, r);
            glScalef(Radius, Radius, Radius);
            glRotatef(270, 0, 1, 0);
            drawUnitPositiveXSphere();
    }glPopMatrix();

    glPushMatrix();{
            glTranslatef(0, 0, -r);
            glScalef(Radius, Radius, Radius);
            glRotatef(90, 0, 1, 0);
            drawUnitPositiveXSphere();
    }glPopMatrix();

    glColor3f(0, 0, 1);
    glPushMatrix();{
            glTranslatef(0, r, 0);
            glScalef(Radius, Radius, Radius);
            glRotatef(90, 0, 0, 1);
            drawUnitPositiveXSphere();
    }glPopMatrix();

    glPushMatrix();{
            glTranslatef(0, -r, 0);
            glScalef(Radius, Radius, Radius);
            glRotatef(270, 0, 0, 1);
            drawUnitPositiveXSphere();
    }glPopMatrix();  
}

void drawCylinder(double height){
    int numSegments = 100;
    double segmentAngle = c_angle / numSegments;
    double halfAngleRadians = (c_angle / 2.0) * 3.1416 / 180.0;

    // glColor3f(0.5, 0.5, 0.5);  // Gray color
    glBegin(GL_TRIANGLE_STRIP);{
        glColor3f(1,1,0);
        for (int i = 0; i <= numSegments; ++i) {
            double angle = -halfAngleRadians + i * segmentAngle * 3.1416 / 180.0;
            double x = Radius * cos(angle);
            double y = Radius * sin(angle);

            glVertex3f(x, y, -height / 2.0);
            glVertex3f(x, y, height/2);
        }
    }glEnd();
}

void drawAllCylinder(){
    GLfloat r = 1-sqrt(2)*Radius;
    GLfloat h = sqrt(2) - 2*Radius; 
    
    for(int i = 0; i < 4; i++){
        glPushMatrix();{
            glRotated(90*i, 0, 1, 0);
            glTranslated(0, r/2, r/2);
            glRotated(45, 1, 0, 0);
            glRotated(90, 0, 0, 1);
            drawCylinder(h);
        }glPopMatrix();
    }

    for(int i = 0; i < 4; i++){
        glPushMatrix();{
            glRotated(90*i, 0, 1, 0);
            glTranslated(r/2, 0, r/2);
            glRotated(-45, 0, 1, 0);
            drawCylinder(h);
        }glPopMatrix();
    }

    for(int i = 0; i < 4; i++){
        glPushMatrix();{
            glRotated(90*i, 0, 1, 0);
            glTranslated(0, -r/2, r/2);
            glRotated(-45, 1, 0, 0);
            glRotated(-90, 0, 0, 1);
            drawCylinder(h);
        }glPopMatrix();
    }
}

void drawTriangle(){
    glBegin(GL_TRIANGLES);{
        glVertex3f(1,0,0);
        glVertex3f(0,1,0);
        glVertex3f(0,0,1);
    }glEnd();
}

// void drawRevTriangle(){
//     glBegin(GL_TRIANGLES);{
//         glVertex3f(1,0,0);
//         glVertex3f(0,1,0);
//         glVertex3f(0,0,-1);
//     }glEnd();
// }

void drawSquare(double a){
    glBegin(GL_QUADS);{
        glVertex3f(a,a,0);
        glVertex3f(a,-a,0);
        glVertex3f(-a,-a,0);
        glVertex3f(-a,a,0);
    }glEnd();
}

void drawPyramid(){
    GLfloat scale = 1 - sqrt(2) * Radius;
    GLfloat translate_factor = (Radius)/sqrt(3);

    glColor3f(0,1,1);
    for(int i=0; i<4; i+=2){
        glPushMatrix();{
            glRotatef(90*i, 0, 0, 1);
            glTranslatef(translate_factor, translate_factor, translate_factor);
            glScalef(scale, scale, scale);
            drawTriangle();
        }glPopMatrix();
    }

    glColor3f(1,0,1);
    for(int i=1; i<4; i+=2){
        glPushMatrix();{
            glRotatef(90*i, 0, 0, 1);
            glTranslatef(translate_factor, translate_factor, translate_factor);
            glScalef(scale, scale, scale);
            drawTriangle();
        }glPopMatrix();
    }
}

void drawOctahedron(){
    drawPyramid();
    glPushMatrix();{
        glRotatef(180, 1, 0, 0);
        drawPyramid();
    }glPopMatrix();
}

void drawObject(){
    drawCompleteCubeSphere();
    drawOctahedron();
    drawAllCylinder();
}

void idle(){
    glutPostRedisplay();
}

void display(){
    counter++;
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(eye.x,eye.y,eye.z, look.x+eye.x,look.y+eye.y,look.z+eye.z, up.x,up.y,up.z);

    // axes();   
    glRotated(objAngle, 0, 0, 1);
    drawObject();
    glutSwapBuffers();
}

void init(){
    glClearColor(0,0,0,0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 1, 0.1, 1000);
}

void Timer(int value){
    glutPostRedisplay();
    glutTimerFunc(1000, Timer, 0);
}

void specialKeyListener(int key, int x, int y){
    cout << "eye.x: " << eye.x << " eye.y: " << eye.y << " eye.z: " << eye.z << "\n";
    switch(key){
        case GLUT_KEY_DOWN:
            // r = sqrt(eye.x*eye.x + eye.y*eye.y + eye.z*eye.z);
            // theta = asin(eye.z/r);
            // r = r+5;
            // eye.z = r*sin(theta);
            // phi = atan(eye.y/eye.x);
            // p = r*cos(theta);
            // eye.x = p*cos(phi);
            // eye.y = p*sin(phi);
            eye = eye - look;
            break;
        case GLUT_KEY_UP:
            eye = eye + look;
            break;
        case GLUT_KEY_RIGHT:
            eye = eye + right_;
            break;
        case GLUT_KEY_LEFT:
            eye = eye - right_;
            break;
        case GLUT_KEY_PAGE_UP:
            eye = eye + up;
            break;
        case GLUT_KEY_PAGE_DOWN:
            eye = eye - up;
            break;
        default:
            break;
    }
}

void keyboardListener(unsigned char key, int x, int y){
    cout << "look.x: " << look.x << " look.y: " << look.y << " look.z: " << look.z << "\n";
    switch(key){
        case '1':
            look = Rodrigues(look, up, 0.1);
            look = Normalize(look);
            right_ = look|up;
            right_ = Normalize(right_);
            break;
        case '2':
            look = Rodrigues(look, up, -0.1);
            look = Normalize(look);
            right_ = look|up;
            right_ = Normalize(right_);
            break;
        case '3':
            look = Rodrigues(look, right_, 0.1);
            look = Normalize(look);
            up = right_|look;
            up = Normalize(up);
            break;
        case '4':
            look = Rodrigues(look, right_, -0.1);
            look = Normalize(look);
            up = right_|look;
            up = Normalize(up);
            break;
        case '5':
            right_ = Rodrigues(right_, look, 0.1);
            right_ = Normalize(right_);
            up = right_|look;
            up = Normalize(up);
            break;
        case '6':
            right_ = Rodrigues(right_, look, -0.1);
            right_ = Normalize(right_);
            up = right_|look;
            up = Normalize(up);
            break;
        case '.':
        Radius -= 0.045;
        if(Radius<0)
            Radius = 0;
            break;
        case ',':
        Radius += 0.045;
        if(Radius>1/sqrt(2))
            Radius = 1/sqrt(2);
            break;
        case 'a':
            objAngle -= 5;
            break;
        case 'd':
            objAngle += 5;
            break;
        default:
            break;
    }
}

int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitWindowPosition(100, 100);   
    glutInitWindowSize(500, 500);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("1905105_Magic_Cube");
    init();
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutSpecialFunc(specialKeyListener);
    glutKeyboardFunc(keyboardListener);
    // glutTimerFunc(1000, Timer, 0);
    glutMainLoop();
    return 0;
}