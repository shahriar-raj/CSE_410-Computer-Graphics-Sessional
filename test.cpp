#include <bits/stdc++.h>
using namespace std;

#ifdef __linux__
    #include <GL/glut.h>
#elif WIN32
    #include <windows.h>
    #include <glut.h>

#endif

struct Point
{
    double x, y;
  
};

void axes(){
    glBegin(GL_LINES);{
        glColor3f(0,1,0);
        glVertex2f(-1,0);
        glVertex2f(1,0);

        glColor3f(1,0,0);
        glVertex2f(0,1);
        glVertex2f(0,-1);
    }glEnd();
}

void DrawCircle(){
    double r = 0.5;
    double theta = 0;
    double prevx = r - 0.3;
    double prevy = 0;
    while (theta<=360)
    {
         double thetainrad=theta*3.1416/180;
         double x=r*cos(thetainrad)-0.3;
         double y=r*sin(thetainrad);
            glBegin(GL_LINES);
            {
                glColor3f(0,0,1);
                glVertex2f(prevx,prevy);
                glVertex2f(x,y);
            }
            glEnd();
            prevx=x;
            prevy=y;
            theta+=3;
   }
}

void display(){
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    axes();
    DrawCircle();
    glFlush();
}

void init(){
    glClearColor(0, 0,0 , 1);
}

void Timer(int value)
{
    glutPostRedisplay();
    glutTimerFunc(80,Timer,0);
    rotation+=5;
    sinemove-=5;
    if(rotation>=360)
    {
        rotation=0;
    }
    double x= r*cos(rotation*3.1416/180);
    double y= r*sin(rotation*3.1416/180);
    trace.x=x;
    trace.y=y;
   
}

int main(int argc, char **argv)
{

    glutInit(&argc, argv);
    glutInitWindowPosition(500, 100);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Online");
    glutDisplayFunc(display);
    init();

   
    glutDisplayFunc(display);
    glutTimerFunc(0,Timer,0);
    glutMainLoop();

    return 0;
}
