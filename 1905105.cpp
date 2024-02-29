#include <bits/stdc++.h>
#include <cmath>

#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <windows.h>
#include <glut.h>
#endif

double rotation = 0;
struct Points{
    double x, y, z;
    Points(double x, double y, double z){
        this->x = x;
        this->y = y;
        this->z = z;
    }
};

void axes(){
    glBegin(GL_LINES);{
        glColor3f(1.0, 1.0, 1.0);
        glVertex3f(0,100,0);
        glVertex3f(0,-100,0);

        glVertex3f(100,0,0);
        glVertex3f(-100,0,0);

        glVertex3f(0,0,100);
        glVertex3f(0,0,-100);
    }glEnd();
}

void drawSquare(){
    glBegin(GL_QUADS);{
        glColor3f(1,1,1);
        glVertex3f(0.2,1.3,-1);
        glVertex3f(-0.2,1.3,-1);
        glVertex3f(-0.2,1.7,-1);
        glVertex3f(0.2,1.7,-1);
    }glEnd();
    glBegin(GL_LINES);{
        glColor3f(1,1,1);
        glVertex3f(0,1.3,-1);
        glVertex3f(0,1.5,0);
        glVertex3f(0,1.7,-1);
        glVertex3f(0,1.5,0);
    }glEnd();
}

void DrawCircle(){
    double r = 1.5;
    double theta = 0;
    double prevx = r;
    double prevy = 0;
    while (theta<=360)
    {
         double thetainrad=theta*3.1416/180;
         double x=r*cos(thetainrad);
         double y=r*sin(thetainrad);
            glBegin(GL_LINES);
            {
                glColor3f(1,1,1);
                glVertex2f(prevx,prevy);
                glVertex2f(x,y);
            }
            glEnd();
            prevx=x;
            prevy=y;
            theta+=3;
   }
}

void drawSwing(){
    glPushMatrix();
    glRotatef(rotation,0,0,1);
    drawSquare();
    glPopMatrix();
    glPushMatrix();
    glRotatef(rotation+60, 0, 0, 1);
    drawSquare();
    glPopMatrix();
    glPushMatrix();
    glRotatef(rotation+120, 0, 0, 1);
    drawSquare();
    glPopMatrix();
    glPushMatrix();
    glRotatef(rotation+180, 0, 0, 1);
    drawSquare();
    glPopMatrix();
    glPushMatrix();
    glRotatef(rotation+240, 0, 0, 1);
    drawSquare();
    glPopMatrix();
    glPushMatrix();
    glRotatef(rotation+300, 0, 0, 1);
    drawSquare();
    glPopMatrix();
}

void display(){
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    // glLoadIdentity();

    // axes();   
    // glRotated(objAngle, 0, 0, 1);
    // drawObject();
    gluLookAt(5,5,5, 0, 0, 0, 0,0,1);
    axes();
    DrawCircle();
    drawSwing();
    glutSwapBuffers();
}

void init(){
    glClearColor(0,0,0,0);
    glMatrixMode(GL_PROJECTION);
//     glLoadIdentity();
    gluPerspective(45, 1, 0.1, 1000);
}

void Timer(int value)
{
    glutPostRedisplay();
    glutTimerFunc(80,Timer,0);
    rotation+=5;
    if(rotation>=360)
    {
        rotation=0;
    }
}

int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitWindowPosition(200, 200);   
    glutInitWindowSize(600, 600);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("1905105_Magic_Swing");
    init();
    glutDisplayFunc(display);
    // glutIdleFunc(idle);
    // glutSpecialFunc(specialKeyListener);
    // glutKeyboardFunc(keyboardListener);

    glutTimerFunc(0,Timer,0);
    glutMainLoop();
    return 0;
}