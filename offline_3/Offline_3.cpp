#include <bits/stdc++.h>
#include <cmath>

#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <windows.h>
#include <glut.h>
#endif

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

Points eye(10, 10, 10);
Points look(-1.0/sqrt(3.0), -1.0/sqrt(3.0), -1.0/sqrt(3.0));
Points up(-1.0/sqrt(2.0), 1.0/sqrt(2.0), 0);
Points right_((1.0/sqrt(6.0)), (1.0/sqrt(6.0)), (-2.0/sqrt(6.0)));
// double objAngle = 0;
double Phi = 45;
double lambda = 45;
double x,y;

void drawCheckerBox(double a, int color = 0) {
  glBegin(GL_QUADS);
  {
    if (color == 0) {
      glColor3f(0.0f, 0.0f, 0.0f); // Black
    } else {
      glColor3f(1.0f, 1.0f, 1.0f); // White
    }
    glVertex3f(0, 0, 0);
    glVertex3f(0, a, 0);
    glVertex3f(a, a, 0);
    glVertex3f(a, 0, 0);
  }
  glEnd();
}

void drawCheckers(double a) {
  for (int i = -40; i < 40; i++) {
    for (int j = -40; j < 40; j++) {
      glPushMatrix();
      glTranslatef(i * a, j * a, 0);
      drawCheckerBox(a, (i + j) % 2);
      glPopMatrix();
    }
  }
}

void idle(){
    glutPostRedisplay();
}

void display(){
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(eye.x,eye.y,eye.z, look.x+eye.x,look.y+eye.y,look.z+eye.z, up.x,up.y,up.z);
    drawCheckers(1.0);
    glutSwapBuffers();
}

void init(){
    glClearColor(0,0,0,0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 1, 0.1, 1000);
}

void specialKeyListener(int key, int x, int y){
    cout << "eye.x: " << eye.x << " eye.y: " << eye.y << " eye.z: " << eye.z << "\n";
    switch(key){
        case GLUT_KEY_DOWN:
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
        default:
            break;
    }
}

int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitWindowPosition(100, 100);   
    glutInitWindowSize(640, 640);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("1905105_Camera_Working");
    init();
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutSpecialFunc(specialKeyListener);
    glutKeyboardFunc(keyboardListener);
    // glutTimerFunc(1000, Timer, 0);
    glutMainLoop();
    return 0;
}