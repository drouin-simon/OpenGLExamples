#include "GL/glew.h"
#include <glut.h>
#include <stdlib.h>
#include "GlslShader.h"
#include "SVL.h"
#include "SVLgl.h"

// original app params
int winWidth = 800;
int winHeight = 600;
int winX = 50;
int winY = 50;
const char appName[] = "NewWideLine";

GlslShader * shader = 0;
Vec2 pt0( 200.0, 200.0 );
Vec2 pt1( 400.0, 400.0 );
float w0 = 50.0;
float w1 = 100.0;
float c0 = 0.5;
float c1 = 1.0;

const char * vertexCode = "#version 120\n\
void main() \
{ \
    gl_Position = ftransform(); \
    gl_TexCoord[0] = gl_MultiTexCoord0; \
    gl_FrontColor = gl_Color; \
} ";

const char * shaderCode = "#version 120\n\
\
void main() \
{ \
    float s = gl_TexCoord[0].x; \
    float t = 2.0 * abs( gl_TexCoord[0].y - 0.5 ); \
    float r = gl_TexCoord[0].z; \
    float c = gl_TexCoord[0].w; \
    float f = s + sqrt( clamp( r*r - t*t, 0.0, 1.0 ) ); \
    gl_FragColor = gl_Color; \n\
    gl_FragColor.a = f; \n\
} ";

const char * shaderCodeMin = "#version 120\n\
void main() \
{ \
    gl_FragColor = gl_Color; \n\
} ";

void display(void)
{
    glClearColor( 0.0, 0.0, 0.0, 1.0 );
	glClear( GL_COLOR_BUFFER_BIT );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    if( !shader )
    {
        shader = new GlslShader;
        shader->AddVertexShaderMemSource( vertexCode );
        shader->AddShaderMemSource( shaderCode );
        if( !shader->Init() )
        {
            std::cerr << "Cannot init shader:" << std::endl;
            std::cerr << shader->GetCompilationError() << std::endl;
            delete shader;
            shader = 0;
            return;
        }
    }
	
    // Compute coords of the quad
    float halfWidth = 0.5 * std::max( w0, w1 );
    Vec2 dir = pt1 - pt0;
    Vec2 normDir( dir );
    normDir.Normalise();
    Vec2 right( dir[1], -dir[0] );
    right.Normalise();
    Vec2 bl = pt0 - halfWidth * right - halfWidth * normDir;
    Vec2 br = pt0 + halfWidth * right - halfWidth * normDir;
    Vec2 jl = pt1 - halfWidth * right;
    Vec2 jr = pt1 + halfWidth * right;
    Vec2 tr = pt1 + halfWidth * right + halfWidth * normDir;
    Vec2 tl = pt1 - halfWidth * right + halfWidth * normDir;

    // Setup shader and vars
    float r0 = 0.5 * w0 / halfWidth;
    float r1 = 0.5 * w1 / halfWidth;
    shader->UseProgram( true );

    // TODO : switch r to world instead of 0.0-1.0

    // Draw the quad
    glColor4d( 1.0, 1.0, 1.0, 1.0 );
    glBegin( GL_QUADS );

        // body
        //glColor4d( 1.0, 1.0, 1.0, 1.0 );
        glTexCoord4d( 0.0, 0.0, r0, c0 ); glVertex( bl );
        glTexCoord4d( 0.0, 1.0, r0, c0 ); glVertex( br );
        glTexCoord4d( 1.0, 1.0, r1, c1 ); glVertex( jr );
        glTexCoord4d( 1.0, 0.0, r1, c1 ); glVertex( jl );

        // front cap
        //glColor4d( 1.0, 1.0, 0.0, 1.0 );
        glTexCoord4d( 1.0, 0.0, r1, c1 ); glVertex( jl );
        glTexCoord4d( 1.0, 1.0, r1, c1 ); glVertex( jr );
        glTexCoord4d( 1.0, 1.0, r1, c1 ); glVertex( tr );
        glTexCoord4d( 1.0, 0.0, r1, c1 ); glVertex( tl );

	glEnd();

    shader->UseProgram( false );

    glutSwapBuffers();
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

void PrintGLVersion()
{
    std::cout << "OpenGL vendor: " << glGetString(GL_VENDOR);
    std::cout << " - Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl << std::endl;
}
   
int main(int argc, char** argv)
{
	glutInit( &argc, argv);
    // OSX gives 2.1 unless you specify GLUT_3_2_CORE_PROFILE, which gives 3.3 but no compatibility
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA /*| GLUT_3_2_CORE_PROFILE */ );
	glutInitWindowSize (winWidth, winHeight); 
	glutInitWindowPosition (winX, winY);
	glutCreateWindow (appName);
	glutDisplayFunc(display); 
	glutReshapeFunc(reshape); 
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
    glutSpecialFunc( special );
    glutSpecialUpFunc( special_up );
    glewInit();
    PrintGLVersion();
	//glutFullScreen();
	glutMainLoop();
	return 0;
}
