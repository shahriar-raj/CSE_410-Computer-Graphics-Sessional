// #include <iostream>
// #include <GL/glut.h>

// GLdouble px = 0, py = 0;
// GLdouble vx = 0.001;

// void display(){
//     glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
//     glPointSize(10);
//     glBegin(GL_LINES);
//         glVertex3d(-1, 0, 0);
//         glVertex3d(1, 0, 0);
//         glVertex3d(0, -1, 0);
//         glVertex3d(0, 1, 0);
//         glVertex3d(0, 0, -1);
//         glVertex3d(0, 0, 1);
//     glEnd();
//     glutSwapBuffers();
// }

// void idle(){
//     px += vx;
//     if(px>=1 || px<=-1)
//          vx *= -1;
//     glutPostRedisplay();
// }

// void  keyboard(unsigned char key, int x, int y){
//     if(key == 'a')
//         px -= 0.05;
//     else if(key == 'd')
//         px += 0.05;
//     else if(key == 'w')
//         py += 0.05;
//     else if(key == 's')
//         py -= 0.05;
//     glutPostRedisplay();
// }

// void special(int key, int x, int y){
//     if(key == GLUT_KEY_LEFT)
//         px -= 0.05;
//     else if(key == GLUT_KEY_RIGHT)
//         px += 0.05;
//     else if(key == GLUT_KEY_UP)
//         py += 0.05;
//     else if(key == GLUT_KEY_DOWN)
//         py -= 0.05;
//     glutPostRedisplay();
// }

// void mouse(int button, int state, int x, int y){
//     if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
//         px = (x-320)/320.0;
//         py = (320-y)/320.0;
//         glutPostRedisplay();
//         std::cout << px << " " << py << std::endl;
//     }
// }

// void timer(int value){
//     px += vx;
//     if(px>=1 || px<=-1)
//          vx *= -1;
//     glutPostRedisplay();
//     glutTimerFunc(10, timer, 0);
// }

// int main(int argc, char** argv){
//     glutInit(&argc, argv);
//     glutInitWindowSize(640, 640);
//     glutInitWindowPosition(100, 50);
//     glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE | GLUT_RGB);
//     glutCreateWindow("Hello World");
//     glutDisplayFunc(display);
//     glutIdleFunc(idle);
//     glutTimerFunc(0, timer, 0);
//     glutKeyboardFunc(keyboard);
//     glutSpecialFunc(special);
//     glutMouseFunc(mouse);
//     glutMainLoop();
//     return 0;
// }







#include<stdio.h>
#include<stdlib.h>
#include<math.h>

// #include <windows.h>
#include <GL/glut.h>

#define pi (2*acos(0.0))

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;

struct PT
{
	PT() {}
	double x,y,z;

	PT(double x, double y, double z) : x(x), y(y), z(z) {}
    PT(const PT &p) : x(p.x), y(p.y), z(p.z) {}

	// arithemtic operations
	PT operator +(PT b)  {return PT(x+b.x,y+b.y, z+b.z);}
    PT operator -(PT b)  {return PT(x-b.x,y-b.y, z-b.z);}
	PT operator *(double b)  {return PT(x*b,y*b, z*b);}
	PT operator /(double b)  {return PT(x/b,y/b, z/b);}
	PT operator *(PT b)  {return PT(y*b.z-z*b.y, z*b.x-x*b.z, x*b.y-y*b.x);}
};

// position of camera
PT pos(200,0,0);

// up, right and look direction
PT up(0,0,1);
PT right(0,1,0);
PT look(-1,0,0);

double ROT_ANG = pi/180;
int numSegments;

// draw axes
void drawAxes()
{
	if(drawaxes==1)
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);{
			glVertex3f( 100,0,0);
			glVertex3f(-100,0,0);

			glVertex3f(0,-100,0);
			glVertex3f(0, 100,0);

			glVertex3f(0,0, 100);
			glVertex3f(0,0,-100);
		}glEnd();
	}
}

// draws grid
void drawGrid()
{
	int i;
	if(drawgrid==1)
	{
		glColor3f(0.6, 0.6, 0.6);	//grey
		glBegin(GL_LINES);{
			for(i=-8;i<=8;i++){

				if(i==0)
					continue;	//SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i*10, -90, 0);
				glVertex3f(i*10,  90, 0);

				//lines parallel to X-axis
				glVertex3f(-90, i*10, 0);
				glVertex3f( 90, i*10, 0);
			}
		}glEnd();
	}
}

void drawPoint(double x,double y,double z)
{
	glBegin(GL_POINTS);
	glColor3f(1.0,1.0,0.0);
	glPointSize(5.0f);  // wat
	glVertex3f(x,y,z);
	glEnd();
}

void drawSquare(double a)
{
    //glColor3f(1.0,0.0,0.0);
	glBegin(GL_QUADS);{
		glVertex3f( a, a,0);
		glVertex3f( a,-a,0);
		glVertex3f(-a,-a,0);
		glVertex3f(-a, a,0);
	}glEnd();
}


void drawCircle(double radius,int segments)
{
    int i;
    struct PT points[100];
    glColor3f(0.7,0.7,0.7);
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated points
    for(i=0;i<segments;i++)
    {
        glBegin(GL_LINES);
        {
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}

void drawCone(double radius,double height,int segments)
{
    int i;
    double shade;
    struct PT points[100];
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw triangles using generated points
    for(i=0;i<segments;i++)
    {
        //create shading effect
        if(i<segments/2)shade=2*(double)i/(double)segments;
        else shade=2*(1.0-(double)i/(double)segments);
        glColor3f(shade,shade,shade);

        glBegin(GL_TRIANGLES);
        {
            glVertex3f(0,0,height);
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}

void drawSphere(double radius,int slices,int stacks)
{
	struct PT points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();
		}
	}
}


void drawSphereQuad(double radius,int slices,int stacks)
{
	struct PT points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices/4;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        // glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
        glColor3f(1,0,0);
		for(j=0;j<slices/4;j++)
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                // //lower hemisphere
                // glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				// glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				// glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				// glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();
		}
	}
}

void drawCylinder(double radius,double height,int segments)
{
	struct PT points[100];
	int i;
	double shade;
	//generate points
	for(i=0;i<=segments;i++)
	{
		points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
		points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
	}
	//draw quads using generated points
	for(i=0;i<segments;i++)
	{
		//create shading effect
		if(i<segments/2)shade=2*(double)i/(double)segments;
		else shade=2*(1.0-(double)i/(double)segments);
		glColor3f(shade,shade,shade);

		glBegin(GL_QUADS);{
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i].x,points[i].y,height);
			glVertex3f(points[i+1].x,points[i+1].y,height);
			glVertex3f(points[i+1].x,points[i+1].y,0);
		}glEnd();
	}
}

void drawCylinderQuad(double radius,double height,int segments)
{
	struct PT points[200];
	int i;
	double shade;
	//generate points
	for(i=0;i<=segments/4;i++)
	{
		points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
		points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
	}
	//draw quads using generated points
	for(i=0;i<segments/4;i++)
	{
		//create shading effect
		// if(i<segments/2)shade=2*(double)i/(double)segments;
		// else shade=2*(1.0-(double)i/(double)segments);
		// glColor3f(shade,shade,shade);
		glColor3f(0,1,0);

		glBegin(GL_QUADS);{
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i].x,points[i].y,height);
			glVertex3f(points[i+1].x,points[i+1].y,height);
			glVertex3f(points[i+1].x,points[i+1].y,0);
		}glEnd();
	}
}


void drawSS()
{
    glColor3f(1,0,0);
    drawSquare(20);

	
    glRotatef(angle,0,0,1);
    glTranslatef(110,0,0);
    glRotatef(2*angle,0,0,1);
    glColor3f(0,1,0);
    drawSquare(15);

    glPushMatrix();
    {
        glRotatef(angle,0,0,1);
        glTranslatef(60,0,0);
        glRotatef(2*angle,0,0,1);
        glColor3f(0,0,1);
        drawSquare(10);
    }
    glPopMatrix();

    glRotatef(3*angle,0,0,1);
    glTranslatef(40,0,0);
    glRotatef(4*angle,0,0,1);
    glColor3f(1,1,0);
    drawSquare(5);
}

double squareH = 20;
double sphereR = 40;

bool startRotate = 0;

void drawAll(bool isRot = 0)
{
	/*****************
	 * 
	 * 
	 * CUBE : 6 part
	 * 
	 * 
	 *****************/

	// WHITE square
	glColor3f(1,1,1);

	// draw the square one by one

	if(startRotate) glRotatef(3*angle,0,0,1);

	// right
    glPushMatrix();
	{
		glTranslatef(0,squareH+sphereR,0);
		if(isRot) glRotatef(3*angle,0,0,1);
		glRotatef(90,1,0,0);
		drawSquare(squareH);
	}
	glPopMatrix();

	// left
    glPushMatrix();
	{
		glTranslatef(0,-squareH-sphereR,0);
		if(isRot) glRotatef(3*angle,0,0,1);
		glRotatef(90,1,0,0);
		drawSquare(squareH);
	}
	glPopMatrix();

	// front
    glPushMatrix();
	{
		glTranslatef(squareH+sphereR,0,0);
		if(isRot) glRotatef(3*angle,0,0,1);
		glRotatef(90,0,1,0);
		drawSquare(squareH);
	}
	glPopMatrix();

	// back
    glPushMatrix();
	{
		glTranslatef(-squareH-sphereR,0,0);
		if(isRot) glRotatef(3*angle,0,0,1);
		glRotatef(90,0,1,0);
		drawSquare(squareH);
	}
	glPopMatrix();

	// up
	glPushMatrix();
	{
		glTranslatef(0,0,squareH+sphereR);
		if(isRot) glRotatef(3*angle,0,0,1);
		drawSquare(squareH);
	}
	glPopMatrix();

	// down
	glPushMatrix();
	{
		glTranslatef(0,0,-squareH-sphereR);
		if(isRot)glRotatef(3*angle,0,0,1);
		drawSquare(squareH);
	}
	glPopMatrix();

	/*****************
	 * 
	 * 
	 * SPHERE : 8 part
	 * 
	 * 
	 *****************/

	glColor3f(1,0,0);

	drawPoint(squareH,squareH,squareH);

	// z : up , x,y : all combination

    // UP 1
    glPushMatrix();
	{
		glTranslatef(squareH,squareH,squareH);
    	drawSphereQuad(sphereR,numSegments,numSegments);
	}
    glPopMatrix();

	// UP 2
    glPushMatrix();
	{
		glTranslatef(squareH,-squareH,squareH);
		glRotatef(270,0,0,1);
    	drawSphereQuad(sphereR,numSegments,numSegments);
	}
    glPopMatrix();

	// UP 3
    glPushMatrix();
	{
		glTranslatef(-squareH,squareH,squareH);
		glRotatef(90,0,0,1);
    	drawSphereQuad(sphereR,numSegments,numSegments);
	}
    glPopMatrix();

	// UP 4
    glPushMatrix();
	{
		glTranslatef(-squareH,-squareH,squareH);
		glRotatef(180,0,0,1);
    	drawSphereQuad(sphereR,numSegments,numSegments);
	}
    glPopMatrix();

	// // z : down , x,y : all combination

	// DOWN 1
    glPushMatrix();
	{
		glTranslatef(squareH,squareH,-squareH);
		glRotatef(90,0,1,0);
    	drawSphereQuad(sphereR,numSegments,numSegments);
	}
    glPopMatrix();

	// DOWN 2
    glPushMatrix();
	{
		glTranslatef(squareH,-squareH,-squareH);
		glRotatef(270,0,0,1);
    	glRotatef(90,0,1,0);
    	drawSphereQuad(sphereR,numSegments,numSegments);
	}
    glPopMatrix();

	// DOWN 3
    glPushMatrix();
	{
		glTranslatef(-squareH,squareH,-squareH);
		glRotatef(90,0,0,1);
    	glRotatef(90,0,1,0);
    	drawSphereQuad(sphereR,numSegments,numSegments);
	}
    glPopMatrix();

	// DOWN 4
    glPushMatrix();
	{
		glTranslatef(-squareH,-squareH,-squareH);
		glRotatef(180,0,0,1);
    	glRotatef(90,0,1,0);
    	drawSphereQuad(sphereR,numSegments,numSegments);
	}
    glPopMatrix();


	/*****************
	 * 
	 * 
	 * CYLINDER : 12 part
	 * 
	 * 
	 *****************/

	glColor3f(0,1,0);

	// + + +
	glPushMatrix();
	{
		glTranslatef(squareH,squareH,squareH);
		glRotatef(90,1,0,0);
		drawCylinderQuad(sphereR,squareH*2,numSegments);

	}
    glPopMatrix();

	// + - +
	glPushMatrix();
	{
		glTranslatef(squareH,-squareH,squareH);
		glRotatef(180,1,0,0);
		drawCylinderQuad(sphereR,squareH*2,numSegments);

	}
    glPopMatrix();

	// - + +
	glPushMatrix();
	{
		glTranslatef(-squareH,+squareH,squareH);
		glRotatef(90,1,0,0);
		glRotatef(90,0,1,0);
		drawCylinderQuad(sphereR,squareH*2,numSegments);

	}
    glPopMatrix();

	// - - +
	glPushMatrix();
	{
		glTranslatef(-squareH,-squareH,squareH);
		glRotatef(180,1,0,0);
		glRotatef(90,0,1,0);
		drawCylinderQuad(sphereR,squareH*2,numSegments);

	}
    glPopMatrix();


	// + + -
	glPushMatrix();
	{
		glTranslatef(squareH,squareH,-squareH);
		drawCylinderQuad(sphereR,squareH*2,numSegments);

	}
    glPopMatrix();

	// + - -
	glPushMatrix();
	{
		glTranslatef(squareH,-squareH,-squareH);
		glRotatef(270,0,0,1);
		drawCylinderQuad(sphereR,squareH*2,numSegments);

	}
    glPopMatrix();

	// - + -
	glPushMatrix();
	{
		glTranslatef(-squareH,+squareH,-squareH);
		glRotatef(180,0,1,0);
    	glRotatef(90,1,0,0);
		drawCylinderQuad(sphereR,squareH*2,numSegments);
	}
    glPopMatrix();

	// - - -
	glPushMatrix();
	{
		glTranslatef(-squareH,-squareH,-squareH);
		glRotatef(270,1,0,0);
    	glRotatef(90,0,1,0);
		drawCylinderQuad(sphereR,squareH*2,numSegments);
	}
    glPopMatrix();


	// + + -
	glPushMatrix();
	{
		glTranslatef(squareH,squareH,-squareH);
		// glRotatef(90,0,1,0);
    	glRotatef(90,1,0,0);
    	glRotatef(-90,0,0,1);
		drawCylinderQuad(sphereR,squareH*2,numSegments);
	}
    glPopMatrix();


	// - + -
	glPushMatrix();
	{
		glTranslatef(-squareH,+squareH,-squareH);
		glRotatef(90,0,1,0);
		drawCylinderQuad(sphereR,squareH*2,numSegments);
	}
    glPopMatrix();

	// - - -
	glPushMatrix();
	{
		glTranslatef(-squareH,-squareH,-squareH);
		glRotatef(180,0,0,1);
		drawCylinderQuad(sphereR,squareH*2,numSegments);
	}
    glPopMatrix();

	// - + -
	glPushMatrix();
	{
		glTranslatef(-squareH,+squareH,-squareH);
		glRotatef(90,0,0,1);
		drawCylinderQuad(sphereR,squareH*2,numSegments);
	}
    glPopMatrix();

	// - + +
	glPushMatrix();
	{
		glTranslatef(-squareH,+squareH,squareH);
		glRotatef(270,0,1,0);
    	glRotatef(90,1,0,0);
		drawCylinderQuad(sphereR,squareH*2,numSegments);

	}
    glPopMatrix();
	
	
}

void rotate3D(PT &vec,PT &axis,double ang)
{
	// vec = vec*cos(ang)+(vec*axis)*sin(ang);
	vec = vec*cos(ang)+(axis*vec)*sin(ang);
}

void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1':
			// rotate LEFT 
			rotate3D(right,up,ROT_ANG);
			rotate3D(look,up,ROT_ANG);
			break;
		case '2':
			// rotate right
			rotate3D(right,up,-ROT_ANG);
			rotate3D(look,up,-ROT_ANG);
			break;
		case '3':
			// rotate UP
			rotate3D(up,right,ROT_ANG);
			rotate3D(look,right,ROT_ANG);
			break;
		case '4':
			// rotate DOWN
			rotate3D(up,right,-ROT_ANG);
			rotate3D(look,right,-ROT_ANG);
			break;
		case '5':
			// tilt CLKWISE
			rotate3D(right,look,ROT_ANG);
			rotate3D(up,look,ROT_ANG);
			break;
		case '6':
			// tilt COUNTER CLKWISE
			rotate3D(right,look,-ROT_ANG);
			rotate3D(up,look,-ROT_ANG);
			break;
		case 's':
			startRotate = 1 - startRotate;
		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			// cameraHeight -= 3.0;
			pos.x += 3.0;
			break;
		case GLUT_KEY_UP:		// up arrow key
			// cameraHeight += 3.0;
			pos.x -= 3.0;
			break;

		case GLUT_KEY_RIGHT:
			// cameraAngle += 0.03;pos.y += 3.0;
			break;
		case GLUT_KEY_LEFT:
			// cameraAngle -= 0.03;
			pos.y -= 3.0;
			break;

		case GLUT_KEY_PAGE_UP:
			pos.z += 3.0;
			break;
		case GLUT_KEY_PAGE_DOWN:
			pos.z -= 3.0;
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
			if(squareH > 0)
			{
				squareH -= 2;
				sphereR += 1;
			}
			break;
		case GLUT_KEY_END:
			if(sphereR > 0)
			{
				squareH += 2;
				sphereR -= 1;
			}
			break;

		default:
			break;
	}
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				drawaxes=1-drawaxes;
			}
			break;

		case GLUT_RIGHT_BUTTON:
			//........
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}



void display(){

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
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

	//gluLookAt(100,100,100,	0,0,0,	0,0,1);
	//gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	//  gluLookAt(0,0,200,	0,0,0,	0,1,0);
	//gluLookAt(0,100,0,	0,0,0,	0,0,1);

	gluLookAt(pos.x, pos.y, pos.z, 
			pos.x + look.x, pos.y + look.y, pos.z + look.z, 
			up.x, up.y, up.z);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
	drawGrid();

    //glColor3f(1,0,0);
    //drawSquare(10);

    // drawSS();

    // drawCircle(30,24);

    // drawCone(20,50,24);

	// glColor3f(255,0,0);
	// glTranslatef(0,20,0);
	// drawSphere(20,24,20);
	drawAll();
	// drawCylinderQuad(20,50,24);




	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
	angle+=0.02;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=1;
	drawaxes=1;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=(pi)/180;
	numSegments = 36;

	//clear the screen
	glClearColor(0,0,0,0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("My OpenGL Program");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}