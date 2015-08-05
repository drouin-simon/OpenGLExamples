#include <glut.h>
#include <stdlib.h>

// original app params
int winWidth = 800;
int winHeight = 600;
int winX = 50;
int winY = 50;
const char appName[] = "Glut Template";

void display(void)
{
	glClear( GL_COLOR_BUFFER_BIT );
	
	glColor4d( 1.0, 0.0, 0.0, 0.0 );
	glBegin( GL_TRIANGLES );
	glVertex2d( 50, 50 );
	glVertex2d( 100, 50 );
	glVertex2d( 100, 100 );
	glEnd();

    glutSwapBuffers();
}


void init(void) 
{
   glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f);
   //glShadeModel ( GL_SMOOTH );
   glShadeModel ( GL_FLAT );

   // enable texturing
   glEnable( GL_TEXTURE_2D );
}


void reshape(int w, int h)
{
    winWidth = w;
    winHeight = h;
    glViewport ( 0, 0, w, h );   
	gluOrtho2D( 0, w, 0, h );
}


void mouse(int button, int state, int x, int y) 
{
   switch (button) 
   {
   case GLUT_LEFT_BUTTON:
     break;
   }
   glutPostRedisplay();
}


void keyboard (unsigned char key, int x, int y)
{
   switch (key) 
   {
   case 27:
	   exit(0);
	   break;
	}
   glutPostRedisplay();
}


void special(int key, int x, int y)
{
    switch (key) 
    {
    case GLUT_KEY_UP:
        break;
    case GLUT_KEY_LEFT:
		break;
    case GLUT_KEY_RIGHT:
        break;
    case GLUT_KEY_DOWN:
        break;
    }
}


void special_up(int key, int x, int y)
{
   switch (key) 
    {
    case GLUT_KEY_UP:
        break;
    case GLUT_KEY_LEFT:
        break;
    case GLUT_KEY_RIGHT:
        break;
    case GLUT_KEY_DOWN:
        break;
    }
}


void Idle(void)
{
}
   
int main(int argc, char** argv)
{
	glutInit( &argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB );
	glutInitWindowSize (winWidth, winHeight); 
	glutInitWindowPosition (winX, winY);
	glutCreateWindow (appName);
	init ();
	glutDisplayFunc(display); 
	glutReshapeFunc(reshape); 
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(Idle);
    glutSpecialFunc( special );
    glutSpecialUpFunc( special_up );
	//glutFullScreen();
	glutMainLoop();
	return 0;
}
