#include <GL/glew.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include "DrawableTexture.h"
#include "GlslShader.h"
#include "SVL.h"

struct Volume
{
    Volume() : voxels(0) { size[0] = 0; size[1] = 0; size[2] = 0; start[0] = 0.0; start[1] = 0.0; start[2] = 0.0; step[0] = 1.0; step[1] = 1.0; step[2] = 1.0; }
    ~Volume() { if( voxels ) delete voxels; }
    void ComputeBounds( double bounds[6] )
    {
        for( int i = 0; i < 3; ++i )
        {
            bounds[2*i] = start[i] - .5 * step[i];
            bounds[2*i+1] = start[i] + ( (double)size[i] - .5 ) * step[i];
        }
    }
    void ComputeCenter( double center[3] )
    {
        for( int i = 0; i < 3; i++ )
            center[i] = start[i] + (double)(size[i]-1) * .5 * step[i];
    }

    int size[3];
    double start[3];
    double step[3];
    float * voxels;
};

Volume mainVolume;

bool Create3DTextureFromVolume();
bool ReadMincVolume( const char * filename );
void DrawTexturedQuad();

// original app params
int winWidth = 1000;
int winHeight = 800;
int winX = 50;
int winY = 50;
const char appName[] = "Glut Volume Render";

// Camera params
double cameraTarget[3];
double cameraDistance = 800.0;
double cameraAngle1 = 0.0;
double cameraAngle2 = 0.0;
double maxCamAngle2 = 1.57;
double lensAngle = .5235; // 30 degrees in grad

double offsetAmount = 3.0;
double offsetSign = 1;

bool jiggle = true;

DrawableTexture texFront;
DrawableTexture texBack;

unsigned volumeTextureId = 0;

GlslShader volumeShader;

void Vertex( double xTex, double yTex, double zTex, double x, double y, double z )
{
    glTexCoord3f( xTex, yTex, zTex );
    glColor3d( xTex, yTex, zTex );
    glVertex3d( x, y, z );
}

void DrawCube()
{
    double bounds[6];
    mainVolume.ComputeBounds( bounds );
    glBegin( GL_QUADS );
    {
        // front ( x+ normal )
        Vertex( 1.0, 0.0, 0.0, bounds[1], bounds[2], bounds[4] );
        Vertex( 1.0, 1.0, 0.0, bounds[1], bounds[3], bounds[4] );
        Vertex( 1.0, 1.0, 1.0, bounds[1], bounds[3], bounds[5] );
        Vertex( 1.0, 0.0, 1.0, bounds[1], bounds[2], bounds[5] );

        // right ( y+ normal )
        Vertex( 1.0, 1.0, 0.0, bounds[1], bounds[3], bounds[4] );
        Vertex( 0.0, 1.0, 0.0, bounds[0], bounds[3], bounds[4] );
        Vertex( 0.0, 1.0, 1.0, bounds[0], bounds[3], bounds[5] );
        Vertex( 1.0, 1.0, 1.0, bounds[1], bounds[3], bounds[5] );

        // back ( x- normal )
        Vertex( 0.0, 1.0, 0.0, bounds[0], bounds[3], bounds[4] );
        Vertex( 0.0, 0.0, 0.0, bounds[0], bounds[2], bounds[4] );
        Vertex( 0.0, 0.0, 1.0, bounds[0], bounds[2], bounds[5] );
        Vertex( 0.0, 1.0, 1.0, bounds[0], bounds[3], bounds[5] );

        // left ( y- normal )
        Vertex( 0.0, 0.0, 0.0, bounds[0], bounds[2], bounds[4] );
        Vertex( 1.0, 0.0, 0.0, bounds[1], bounds[2], bounds[4] );
        Vertex( 1.0, 0.0, 1.0, bounds[1], bounds[2], bounds[5] );
        Vertex( 0.0, 0.0, 1.0, bounds[0], bounds[2], bounds[5] );

        // top ( z+ normal )
        Vertex( 1.0, 0.0, 1.0, bounds[1], bounds[2], bounds[5] );
        Vertex( 1.0, 1.0, 1.0, bounds[1], bounds[3], bounds[5] );
        Vertex( 0.0, 1.0, 1.0, bounds[0], bounds[3], bounds[5] );
        Vertex( 0.0, 0.0, 1.0, bounds[0], bounds[2], bounds[5] );

        // bottom ( z- normal )
        Vertex( 1.0, 0.0, 0.0, bounds[1], bounds[2], bounds[4] );
        Vertex( 0.0, 0.0, 0.0, bounds[0], bounds[2], bounds[4] );
        Vertex( 0.0, 1.0, 0.0, bounds[0], bounds[3], bounds[4] );
        Vertex( 1.0, 1.0, 0.0, bounds[1], bounds[3], bounds[4] );
    }
    glEnd();
}

void SetupCamera()
{
    // Position camera
    Vec3 camPosition( 0.0, 0.0, 0.0 );
    camPosition[0] = cameraDistance * cos(cameraAngle1) * cos(cameraAngle2) + cameraTarget[0];
    camPosition[1] = cameraDistance * sin(cameraAngle1) * cos(cameraAngle2) + cameraTarget[1];
    camPosition[2] = cameraDistance * sin(cameraAngle2) + cameraTarget[2];

    Vec3 camTarget( cameraTarget );
    Vec3 camFarTarget = camTarget;
    if( jiggle )
    {
        Vec3 camAxis = camTarget - camPosition;

        camFarTarget += .2 * camAxis;

        camAxis.Normalise();
        Vec3 camUp( 0.0, 0.0, 1.0 );
        Vec3 camRight = cross( camAxis, camUp );
        camPosition += camRight * offsetAmount * offsetSign;
        //camTarget += camRight * offsetAmount * offsetSign;
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt( camPosition[0], camPosition[1] , camPosition[2], camFarTarget[0], camFarTarget[1], camFarTarget[2], 0.0, 0.0, 1.0 );

    // Set projection
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    double ratio  = ((double)winHeight) / winWidth;

    double near = 10.0;
    double far = 5000.0;
    double side = near * tan( lensAngle );
    double left = -side;
    double right = side;
    double bottom = -side * ratio;
    double top = side * ratio;
    glFrustum( left, right, bottom, top, near, far );
}

void display(void)
{
    SetupCamera();

    glMatrixMode( GL_MODELVIEW );
    glEnable( GL_CULL_FACE );

    // Draw to the front texture
    glCullFace( GL_FRONT );
    texBack.DrawToTexture( true );
    glClearColor( 0.0, 0.0, 0.0, 0.0 );
    glClear( GL_COLOR_BUFFER_BIT );
    DrawCube();
    texBack.DrawToTexture( false );

    // Draw front of cube and do raycasting in the shader
    glCullFace( GL_BACK );
    glClearColor( 0.0, 0.0, 0.7, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT );
    glColor4d( 1.0, 1.0, 1.0, 1.0 );

    // Bind back texture in texture unit 0
    glActiveTextureARB( GL_TEXTURE0 );
    glEnable( GL_TEXTURE_RECTANGLE_ARB );
    glBindTexture( GL_TEXTURE_RECTANGLE_ARB, texBack.GetTexId() );

    // Bind volume texture to texture unit 1
    glActiveTextureARB( GL_TEXTURE1 );
    glEnable( GL_TEXTURE_3D );
    glBindTexture( GL_TEXTURE_3D, volumeTextureId );

    volumeShader.UseProgram( true );
    bool res = volumeShader.SetVariable( "back_tex_id", int(0) );
    res &= volumeShader.SetVariable( "volume_id", int(1) );
    if( !res )
    {
        std::cout << "Error: Can't set shader variables" << std::endl;
    }

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    DrawCube();
    glDisable( GL_BLEND );

    // Unbind volume texture from tex unit 1 and disable texture 3D in the unit
    glBindTexture( GL_TEXTURE_3D, 0 );
    glDisable( GL_TEXTURE_3D );

    // unbind back texture in tex unit 0
    glActiveTextureARB( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_RECTANGLE_ARB, 0 );
    glDisable( GL_TEXTURE_RECTANGLE_ARB );

    volumeShader.UseProgram( false );

    glutSwapBuffers();
}

bool init(void)
{
    bool result = Create3DTextureFromVolume();

    // set default camera target to center of the volume and compute default camera distance
    mainVolume.ComputeCenter( cameraTarget );
    double offsetY = mainVolume.size[1] * mainVolume.step[1] * .5 * 1.5;
    double offsetX = mainVolume.size[0] * mainVolume.step[0] * .5;
    cameraDistance = offsetY / tan( lensAngle ) + offsetX;

    result &= texFront.Init( 1, 1 );
    result &= texBack.Init( 1, 1 );

    volumeShader.AddShaderFilename( "VolumeShader.glsl" );
    result &= volumeShader.Init();

    return result;
}


void reshape( int w, int h )
{
    winWidth = w;
    winHeight = h;
    glViewport ( 0, 0, w, h );

    texFront.Resize( w, h );
    texBack.Resize( w, h );
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
       break;
	}
   glutPostRedisplay();
}

void Idle()
{
    if( offsetSign == 1.0 )
        offsetSign = -1.0;
    else
        offsetSign = 1.0;

    glutPostRedisplay();
}
   
int main(int argc, char** argv)
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );
    glutInitWindowSize( winWidth, winHeight );
    glutInitWindowPosition( winX, winY );
	glutCreateWindow (appName);

    // Init glew lib
    GLenum err = glewInit();
    if( err != GLEW_OK )
    {
        std::cerr << "Failed to initialize glew!" << std::endl;
        exit(-1);
    }
    std::cout << "Using GLEW Version: " << glewGetString(GLEW_VERSION) << std::endl;

    if( ! ReadMincVolume( argv[1] ) )
    {
        std::cerr << "Can't read minc volume " << argv[1] << std::endl;
        exit(-1);
    }

    if( !init() )
        exit(-1);
    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
    glutMouseFunc( mouse );
    glutMotionFunc( mouseMove );
    glutKeyboardFunc( keyboard );
    glutIdleFunc(Idle);
	glutMainLoop();
	return 0;
}

bool Create3DTextureFromVolume()
{
    bool result = true;

    glActiveTextureARB( GL_TEXTURE2 );  // Do we have to use same texture unit to define the texture and to render it?
    glEnable( GL_TEXTURE_3D );
    glGenTextures( 1, &volumeTextureId );
    glBindTexture( GL_TEXTURE_3D, volumeTextureId );
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage3D( GL_TEXTURE_3D, 0, GL_LUMINANCE, mainVolume.size[0], mainVolume.size[1], mainVolume.size[2], 0, GL_LUMINANCE, GL_FLOAT, mainVolume.voxels );
    glDisable( GL_TEXTURE_3D );

    if( glGetError() != GL_NO_ERROR )
    {
        std::cout << "Error setting 3D texture for the volume" << std::endl;
        result = false;
    }

    return result;
}

#include <minc_1_rw.h>
#include <minc_1_simple.h>
using namespace minc;

bool ReadMincVolume( const char * filename )
{
    minc_1_reader reader;
    reader.open( filename );
    reader.setup_read_float();  // we have to setup the file to a proper data type

    // todo : make sur dim is 3 and there is no vector dim
    int numberOfDimensions = reader.dim_no();
    if( numberOfDimensions != 3 )
        return false;

    mainVolume.size[0] = reader.ndim(1);  // 0 is vector dim
    mainVolume.size[1] = reader.ndim(2);
    mainVolume.size[2] = reader.ndim(3);
    mainVolume.start[0] = reader.nstart( 1 );
    mainVolume.start[1] = reader.nstart( 2 );
    mainVolume.start[2] = reader.nstart( 3 );
    mainVolume.step[0] = reader.nspacing( 1 );
    mainVolume.step[1] = reader.nspacing( 2 );
    mainVolume.step[2] = reader.nspacing( 3 );
    int nbVoxels = mainVolume.size[0] * mainVolume.size[1] * mainVolume.size[2];
    mainVolume.voxels = new float[ nbVoxels ];

    load_standard_volume<float>( reader, mainVolume.voxels );

    // rescale volume between 0 and 1
    double max = 255.0;
    double ratio = 1.0 / max;
    for( int i = 0; i < nbVoxels; ++i )
        mainVolume.voxels[i] *= ratio;

    return true;
}
