#include "GL/glew.h"

// Cross-platform include GLUT
#if defined(__APPLE__) && defined(__MACH__)
#include <glut.h>
#else
#include <GL/glut.h>
#endif

#include "SVL.h"
#include "SVLgl.h"
#include <stdlib.h>
#include <math.h>
#include "DrawableTexture.h"

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
bool showMask = false;
DrawableTexture * mask = 0;

void DrawCube()
{
    glBegin( GL_QUADS );
    {
        // front ( x+ normal )
        glVertex3d( 100, -100, -100 );
        glVertex3d( 100, 100, -100 );
        glVertex3d( 100, 100, 100 );
        glVertex3d( 100, -100, 100 );

        // right ( y+ normal )
        glVertex3d( 100, 100, -100 );
        glVertex3d( -100, 100, -100 );
        glVertex3d( -100, 100, 100 );
        glVertex3d( 100, 100, 100 );

        // back ( x- normal )
        glVertex3d( -100, 100, -100 );
        glVertex3d( -100, -100, -100 );
        glVertex3d( -100, -100, 100 );
        glVertex3d( -100, 100, 100 );

        // left ( y- normal )
        glVertex3d( -100, -100, -100 );
        glVertex3d( 100, -100, -100 );
        glVertex3d( 100, -100, 100 );
        glVertex3d( -100, -100, 100 );

        // top ( z+ normal )
        glVertex3d( 100, -100, 100 );
        glVertex3d( 100, 100, 100 );
        glVertex3d( -100, 100, 100 );
        glVertex3d( -100, -100, 100 );

        // bottom ( z- normal )
        glVertex3d( 100, -100, -100 );
        glVertex3d( -100, -100, -100 );
        glVertex3d( -100, 100, -100 );
        glVertex3d( 100, 100, -100 );
    }
    glEnd();
}

void DrawCubeWithColor()
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

#include <iostream>
#include "GlslShader.h"

using namespace std;
double epsilon = 0.0001;

GlslShader * clippingShader = 0;

const char * clippingShaderCode = " \
uniform sampler2DRect mask; \
void main() \
{ \
    vec4 maskSample = texture2DRect( mask, gl_FragCoord.xy ); \
    if( maskSample.x < 0.1 ) \
    { \
        discard; \
    } \
    gl_FragColor = gl_Color; \
} ";

void RenderClipPlane( Vec3 pos, Vec3 lookAt, Vec3 camUp, double near, double ratio, double lensAngle )
{
    if( !clippingShader )
    {
        clippingShader = new GlslShader;
        clippingShader->AddShaderMemSource( clippingShaderCode );
        clippingShader->Init();
    }

    clippingShader->UseProgram( true );
    clippingShader->SetVariable( "mask", 0 );
    glEnable( GL_TEXTURE_RECTANGLE_ARB );
    glBindTexture( GL_TEXTURE_RECTANGLE_ARB, mask->GetTexId() );

    float planeDist = near + epsilon;
    double halfWidth = planeDist * tan( lensAngle );
    double halfHeight = halfWidth * ratio;

    Vec3 camDir = lookAt - pos;
    camDir.Normalise();
    Vec3 planeCenter = pos + planeDist * camDir;
    Vec3 right = cross( camDir, camUp );
    Vec3 down = cross( camDir, right );

    Vec3 p0 = planeCenter - halfWidth * right + halfHeight * down;
    Vec3 p1 = planeCenter + halfWidth * right + halfHeight * down;
    Vec3 p2 = planeCenter + halfWidth * right - halfHeight * down;
    Vec3 p3 = planeCenter - halfWidth * right - halfHeight * down;

    glColor4d( 1.0, 1.0, 0.0, 0.0 );

    glBegin( GL_QUADS );
        glVertex( p0 );
        glVertex( p1 );
        glVertex( p2 );
        glVertex( p3 );
    glEnd();

    clippingShader->UseProgram( false );
    glDisable( GL_TEXTURE_RECTANGLE_ARB );

    //glm::mat4 t = glm::translate( glm::mat4(1.0f), glm::vec3(-0.5) );
    //glm::mat4 s = glm::scale( glm::mat4(1.0f), glm::vec3( 200.0 ) );
    //glm::mat4 cubeToWorld = s * t;
    //glm::mat4 worldToCube = glm::inverse( cubeToWorld );
    //glm::vec4 testCoord( -100.0, -100.0, -100.0, 1.0 );
    //glm::vec4 transformed = worldToCube * testCoord;
    //cout << "orig: ( " << testCoord.x << ", " << testCoord.y << ", " << testCoord.z << " ) transformed: ( " << transformed.x << ", " << transformed.y << ", " << transformed.z << " )" << endl;
}

void display(void)
{
    // Setup cam and matrices
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

    // draw cube to the texture with invert blending to create
    // a mask of areas of the front side that have been clipped.
    if( !mask )
    {
        mask = new DrawableTexture;
        mask->Init( winWidth, winHeight );
    }
    mask->Resize( winWidth, winHeight );
    mask->DrawToTexture( true );
    glClearColor( 0.0, 0.0, 0.0, 0.0 );
    glClear( GL_COLOR_BUFFER_BIT );

    glEnable( GL_CULL_FACE );
    glDisable( GL_BLEND );
    glDisable( GL_DEPTH_TEST );
    glEnable( GL_COLOR_LOGIC_OP );
    glLogicOp( GL_INVERT );

    glColor4d( 1.0, 1.0, 1.0, 1.0 );
    glCullFace( GL_FRONT );
    DrawCube();
    glCullFace( GL_BACK );
    DrawCube();

    glDisable( GL_COLOR_LOGIC_OP );
    glDisable( GL_CULL_FACE );

    mask->DrawToTexture( false );

    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glClear( GL_COLOR_BUFFER_BIT );
    if( showMask )
    {
        glEnable( GL_BLEND );
        glColor4d( 1.0, 1.0, 1.0, 1.0 );
        mask->PasteToScreen();
    }
    else
    {
        glEnable( GL_CULL_FACE );
        if( showFront )
            glCullFace( GL_BACK );
        else
            glCullFace( GL_FRONT );

        DrawCubeWithColor();

        Vec3 pos( x, y, z );
        Vec3 lookAt( 0.0, 0.0, 0.0 );
        Vec3 camUp( 0.0, 0.0, 1.0 );
        RenderClipPlane( pos, lookAt, camUp, near, ratio, lensAngle );
    }

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
   case 'm':
       showMask = !showMask;
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
