#include <bits/stdc++.h>
#include <cmath>
#include "1905105_classes.h"

#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <windows.h>
#include <glut.h>
#endif

using namespace std;

Points eye(85, 85, 85);
Points look(-0.57735, -0.57735, -0.57735);
Points up(-0.389334, -0.426875, 0.816209);
Points right_(-0.717695, 0.696021, 0.0216739);
// double objAngle = 0;
double Phi = 45;
double lambda = 45;
double x,y;
int img_width = 0;
int n_recursion = 0;
 
vector<Sphere> spheres;
vector<Triangle> triangles;
vector<PointLight> pointLights;
vector<SpotLight> spotLights;

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
    drawCheckers(10.0);
    for(Sphere s: spheres){
        s.draw();
    }
    for(Triangle t: triangles){
        t.draw();
    }
    for(PointLight p: pointLights){
        p.draw();
    }
    glutSwapBuffers();
}

void init(){
    glClearColor(0,0,0,0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(80, 1, 1, 1000);
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
    cout << "up.x: " << up.x << " up.y: " << up.y << " up.z: " << up.z << "\n";
    cout << "right_.x: " << right_.x << " right_.y: " << right_.y << " right_.z: " << right_.z << "\n";
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

void loadData(){
    ifstream file;
    file.open("scene.txt");
    if(!file){
        cout << "File not found\n";
        return;
    }
    int n_objects, n_pointlights, n_spotlights;

    file >> img_width;
    file >> n_recursion;

    file >> n_objects;
    for(int i=0; i<n_objects; i++){
        string type;
        file >> type;
        if(type == "sphere"){
            Sphere s;
            cout << "YES\n";
            file >> s.referencePoint.x >> s.referencePoint.y >> s.referencePoint.z;
            file >> s.length;
            file >> s.color[0] >> s.color[1] >> s.color[2];
            file >> s.coEfficients[0] >> s.coEfficients[1] >> s.coEfficients[2] >> s.coEfficients[3];
            file >> s.shine;
            spheres.push_back(s);
        }
        else if(type == "triangle"){
            cout << "YES2\n";
            Triangle t;
            file >> t.a.x >> t.a.y >> t.a.z;
            file >> t.b.x >> t.b.y >> t.b.z;
            file >> t.c.x >> t.c.y >> t.c.z;
            file >> t.color[0] >> t.color[1] >> t.color[2];
            file >> t.coEfficients[0] >> t.coEfficients[1] >> t.coEfficients[2] >> t.coEfficients[3];
            file >> t.shine;
            triangles.push_back(t);
        }
    }
    
    file >> n_pointlights;
    for(int i=0; i<n_pointlights; i++){
        PointLight p;
        file >> p.position.x >> p.position.y >> p.position.z;
        file >> p.color[0] >> p.color[1] >> p.color[2];
        pointLights.push_back(p);
    }
    
    file >> n_spotlights;
    for(int i=0; i<n_spotlights; i++){
        SpotLight s;
        file >> s.pl.position.x >> s.pl.position.y >> s.pl.position.z;
        file >> s.pl.color[0] >> s.pl.color[1] >> s.pl.color[2];
        file >> s.direction.x >> s.direction.y >> s.direction.z;
        file >> s.angle;
        spotLights.push_back(s);
    }

    file.close();
}

int main(int argc, char** argv){
    loadData();
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