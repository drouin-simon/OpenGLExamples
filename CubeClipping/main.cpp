#include "GL/glew.h"

// Cross-platform include GLUT
#if defined(__APPLE__) && defined(__MACH__)
#include <glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <math.h>

// original app params
int winWidth = 800;
int winHeight = 600;
int winX = 50;
int winY = 50;
const char appName[] = "CubeClipping";

// Camera params
double cameraDistance = 800.0;
double cameraAngle1 = 0.0;
double cameraAngle2 = 0.0;
double maxCamAngle2 = 1.57;

bool showFront = true;

void DrawCube()
{
    glBegin( GL_QUADS );
    {
        // front ( x+ normal )
        glColor3d( 1.0, 0.0, 0.0 ); glVertex3d( 100, -100, -100 );
        glColor3d( 1.0, 1.0, 0.0 ); glVertex3d( 100, 100, -100 );
        glColor3d( 1.0, 1.0, 1.0 ); glVertex3d( 100, 100, 100 );
        glColor3d( 1.0, 0.0, 1.0 ); glVertex3d( 100, -100, 100 );

        // right ( y+ normal )
        glColor3d( 1.0, 1.0, 0.0 ); glVertex3d( 100, 100, -100 );
        glColor3d( 0.0, 1.0, 0.0 ); glVertex3d( -100, 100, -100 );
        glColor3d( 0.0, 1.0, 1.0 ); glVertex3d( -100, 100, 100 );
        glColor3d( 1.0, 1.0, 1.0 ); glVertex3d( 100, 100, 100 );

        // back ( x- normal )
        glColor3d( 0.0, 1.0, 0.0 ); glVertex3d( -100, 100, -100 );
        glColor3d( 0.0, 0.0, 0.0 ); glVertex3d( -100, -100, -100 );
        glColor3d( 0.0, 0.0, 1.0 ); glVertex3d( -100, -100, 100 );
        glColor3d( 0.0, 1.0, 1.0 ); glVertex3d( -100, 100, 100 );

        // left ( y- normal )
        glColor3d( 0.0, 0.0, 0.0 ); glVertex3d( -100, -100, -100 );
        glColor3d( 1.0, 0.0, 0.0 ); glVertex3d( 100, -100, -100 );
        glColor3d( 1.0, 0.0, 1.0 ); glVertex3d( 100, -100, 100 );
        glColor3d( 0.0, 0.0, 1.0 ); glVertex3d( -100, -100, 100 );

        // top ( z+ normal )
        glColor3d( 1.0, 0.0, 1.0 ); glVertex3d( 100, -100, 100 );
        glColor3d( 1.0, 1.0, 1.0 ); glVertex3d( 100, 100, 100 );
        glColor3d( 0.0, 1.0, 1.0 ); glVertex3d( -100, 100, 100 );
        glColor3d( 0.0, 0.0, 1.0 ); glVertex3d( -100, -100, 100 );

        // bottom ( z- normal )
        glColor3d( 1.0, 0.0, 0.0 ); glVertex3d( 100, -100, -100 );
        glColor3d( 0.0, 0.0, 0.0 ); glVertex3d( -100, -100, -100 );
        glColor3d( 0.0, 1.0, 0.0 ); glVertex3d( -100, 100, -100 );
        glColor3d( 1.0, 1.0, 0.0 ); glVertex3d( 100, 100, -100 );
    }
    glEnd();
}

void display(void)
{
    glEnable( GL_CULL_FACE );

    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f);
	glClear( GL_COLOR_BUFFER_BIT );

    // Position camera ( look at (0, 0, 0) )
    glMatrixMode(GL_MODELVIEW);
    double x = cameraDistance * cos(cameraAngle1) * cos(cameraAngle2);
    double y = cameraDistance * sin(cameraAngle1) * cos(cameraAngle2);
    double z = cameraDistance * sin(cameraAngle2);
    glLoadIdentity();
    gluLookAt( x, y , z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0 );

    // Set projection
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    double ratio  = ((double)winHeight) / winWidth;
    double lensAngle = .5235; // 30 degrees in grad
    double near = 20.0;
    double far = 5000.0;
    double side = near * tan( lensAngle );
    double left = -side;
    double right = side;
    double bottom = -side * ratio;
    double top = side * ratio;
    glFrustum( left, right, bottom, top, near, far );

    glMatrixMode( GL_MODELVIEW );
    glColor4d( 1.0, 0.0, 0.0, 1.0 );

    if( showFront )
        glCullFace( GL_BACK );
    else
        glCullFace( GL_FRONT );

    DrawCube();

    glutSwapBuffers();
}




void reshape( int w, int h )
{
    winWidth = w;
    winHeight = h;
    glViewport ( 0, 0, w, h );
}

bool isRotatingCamera = false;
bool isZoomingCamera = false;
int lastX = 0;
int lastY = 0;

void UpdateCameraAngle( int x, int y )
{
    int diffX = x - lastX;
    cameraAngle1 -= .01 * diffX;
    int diffY = y - lastY;
    cameraAngle2 += .01 * diffY;
    if( cameraAngle2 > maxCamAngle2 )
        cameraAngle2 = maxCamAngle2;
    if( cameraAngle2 < -maxCamAngle2 )
        cameraAngle2 = -maxCamAngle2;
    lastX = x;
    lastY = y;
}

void UpdateCameraDistance( int x, int y )
{
    int diffY = y - lastY;
    cameraDistance += (double)diffY;
    lastX = x;
    lastY = y;
}

void mouse(int button, int state, int x, int y) 
{
    if( button == GLUT_LEFT_BUTTON )
    {
        if( state == GLUT_DOWN )
        {
            isRotatingCamera = true;
            lastX = x;
            lastY = y;
        }
        else
        {
            UpdateCameraAngle( x, y );
            isRotatingCamera = false;
        }
    }
    else if( button = GLUT_RIGHT_BUTTON )
    {
        if( state == GLUT_DOWN )
        {
            isZoomingCamera = true;
            lastX = x;
            lastY = y;
        }
        else
        {
            UpdateCameraDistance( x, y );
            isZoomingCamera = false;
        }
    }
    glutPostRedisplay();
}

void mouseMove( int x, int y )
{
    if( isRotatingCamera )
       UpdateCameraAngle( x, y );
    else if( isZoomingCamera )
        UpdateCameraDistance( x, y );
    glutPostRedisplay();
}

void keyboard (unsigned char key, int x, int y)
{
   switch (key) 
   {
   case 27:
	   exit(0);
	   break;
   case 'f':
       showFront = !showFront;
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

void init(void)
{
   glewInit();
}

   
int main(int argc, char** argv)
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );
    glutInitWindowSize( winWidth, winHeight );
    glutInitWindowPosition( winX, winY );
	glutCreateWindow (appName);
	init ();
    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
    glutMouseFunc( mouse );
    glutMotionFunc( mouseMove );
    glutKeyboardFunc( keyboard );
    //glutIdleFunc(Idle);
    glutSpecialFunc( special );
    glutSpecialUpFunc( special_up );
	//glutFullScreen();
	glutMainLoop();
	return 0;
}
