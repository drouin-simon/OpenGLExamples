#include "GL/glew.h"

#ifndef __linux
#include <glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <cstdio>
#include "drwDrawableTexture.h"
#include "drwCacheBufferManager.h"

// return random double between 0.0 and 1.0
inline double randDouble()
{
    return( rand() / (RAND_MAX + 1.0) );
}

// original app params
int winWidth = 800;
int winHeight = 600;
int winX = 50;
int winY = 50;
const char appName[] = "Glut Template";

drwCacheBufferManager bufferManager;
#define numberOfBuffers 100
int currentBuffer = 0;
bool needInitBufferManager = true;
bool usePbo = false;

double posX = (double)winWidth / 2;
double posY = (double)winHeight / 2;
double speedX = 0.0;
double speedY = 0.0;
double maxSpeed = 1.0;
double aX = 0.0;
double aY = 0.0;
double maxA = .1;
double factorA = .05;

double red = 1.0;
double green = .5;
double blue = .75;
double colorSpeed = .001;

int timeBase = 0;
int fpsPeriod = 500;
int fpsFrame = 0;
double fps = 0.0;

void DrawString( char * s )
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, winWidth, 0.0, winHeight);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glColor3f(1.0, 0.0, 0.0);
    glRasterPos2i(10, 10);
    void * font = GLUT_BITMAP_9_BY_15;
    char * i = s;
    while( *i != '\0' )
    {
        char c = *i;
        glutBitmapCharacter(font, c);
        ++i;
    }
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void display(void)
{
    if( needInitBufferManager )
    {
        bufferManager.Init( winWidth, winHeight, numberOfBuffers, usePbo );
        needInitBufferManager = false;
    }
    
    // Draw a random triangle to texture
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, bufferManager.GetCurrentFramebufferId() );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluOrtho2D( 0, winWidth, 0, winHeight );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
	
    red = red + colorSpeed;
    if( red > 1.0 )
        red = .5;
    green = green + colorSpeed;
    if( green > 1.0 )
        green = .5;
    blue = blue + colorSpeed;
    if( blue > 1.0 )
        blue = .5;

    glColor4d( red, green, blue, 1.0 );
    aX = aX + ( randDouble() * 2 - 1.0 ) * factorA;
    if( aX > maxA )
        aX = maxA;
    if( aX < -maxA )
       aX = -maxA;
    speedX = speedX + aX;
    if( speedX > maxSpeed )
        speedX = maxSpeed;
    if( speedX < -maxSpeed )
        speedX = -maxSpeed;
    posX = posX + speedX;
    if( posX < .05 * winWidth )
    {
        posX = .05 * winWidth;
        speedX = -speedX;
    }
    if( posX > winWidth - .05 * winWidth )
    {
        posX = winWidth - .05 * winWidth;
        speedX = -speedX;
    }
    aY = aY + ( randDouble() * 2.0 - 1.0 ) * factorA;
    if( aY > maxA )
        aY = maxA;
    if( aY < -maxA )
        aY = -maxA;
    speedY = speedY + aY;
    if( speedY > maxSpeed )
        speedY = maxSpeed;
    if( speedY < -maxSpeed )
       speedY = -maxSpeed;
    posY = posY + speedY;
    if( posY < .05 * winHeight )
    {
        posY = .05 * winHeight;
        speedY = -speedY;
    }
    if( posY > winHeight - .05 * winHeight )
    {
        posY = winHeight - .05 * winHeight;
        speedY = -speedY;
    }
	glBegin( GL_TRIANGLES );
    glVertex2d( posX, posY );
    glVertex2d( posX + 100.0, posY );
    glVertex2d( posX + 100.0, posY + 100.0 );
	glEnd();

    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );

    // Draw texture on the screen
    glClearColor( 0.0, 0.0, 0.0, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT );
    bufferManager.DrawCurrentTexture();

    // Draw fps
    int time = glutGet( GLUT_ELAPSED_TIME );
    if( time - timeBase > fpsPeriod )
    {
        fps = fpsFrame * ((double)fpsPeriod) / ( time - timeBase);
        timeBase = time;
        fpsFrame = 0;
    }
    ++fpsFrame;
    char s[512];
    sprintf( s, "%f", fps );
    DrawString( s );

    glutSwapBuffers();

    // next buffer
    currentBuffer++;
    if( currentBuffer == numberOfBuffers )
        currentBuffer = 0;
    bufferManager.SetCurrentFrame( currentBuffer );
}

#ifndef __linux
#import <OpenGL/OpenGL.h>
#endif

void init(void) 
{
    glewInit();

#ifndef __linux
    //int swap_interval = 1;
    //CGLContextObj cgl_context = CGLGetCurrentContext();
    //CGLSetParameter( cgl_context, kCGLCPSwapInterval, &swap_interval );
#endif

    glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f);
    glShadeModel ( GL_FLAT );

    // enable texturing
    glEnable( GL_TEXTURE_RECTANGLE_ARB );
}

void reshape(int w, int h)
{
    winWidth = w;
    winHeight = h;
    glViewport ( 0, 0, w, h );

    needInitBufferManager = true;
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
           case 'p':
           usePbo = !usePbo;
           needInitBufferManager = true;
           break;
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
    glutPostRedisplay();
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
