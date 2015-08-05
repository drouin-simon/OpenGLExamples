#include <math.h>
#include <vector>
#include "Glee.h"
#undef max
#undef min
#include <iostream>
#include <limits>
#include "glut.h"
#undef max
#undef min
#include "VCycle.h"
#include "GlslShader.h"

using namespace std;

int win = 0;
int winWidth = 1000;
int winHeight = 800;
unsigned textureId;
int mode = 0;

VCycle integrator;


class ScaleBiasShader : public GlslShader
{
public:
	ScaleBiasShader() : m_scale( 0.01f ), m_bias( 0.5f )
	{
		AddShaderFilename( "shaders/gp_scale_bias.glsl" );
	}
	~ScaleBiasShader(){}
protected:
	virtual bool SetupVariables()
	{
		bool res = true;
		res &= SetVariable( "scale", m_scale );
		res &= SetVariable( "bias", m_bias );
		return res;
	}
	float m_scale;
	float m_bias;
};

ScaleBiasShader scaleBiasShader;

class Vec2
{
public:
	Vec2() : m_x(0),m_y(0) {}
	Vec2(double x, double y) : m_x(x), m_y(y) {}
	double m_x;
	double m_y;
};

class Color
{
public:
	Color() : m_r(0), m_g(0), m_b(0), m_a(0) {}
	Color(double r, double g, double b, double a) : m_r(r), m_g(g), m_b(b), m_a(a) {}
	double m_r;
	double m_g;
	double m_b;
	double m_a;
};

class BBox
{
public:
	BBox() : m_xMin( numeric_limits<double>::max() ),
		m_xMax( numeric_limits<double>::min() ),
		m_yMin( numeric_limits<double>::max() ),
		m_yMax( numeric_limits<double>::min() ) {}
	BBox(double xMin, double xMax, double yMin, double yMax) : m_xMin(xMin), m_xMax(xMax), m_yMin(yMin), m_yMax(yMax) {}
	double GetWidth()  { return m_xMax - m_xMin; }
	double GetHeight() { return m_yMax - m_yMin; }
	double m_xMin;
	double m_xMax;
	double m_yMin;
	double m_yMax;
};

class PixRect
{
public:
	PixRect() : x(0), y(0), width(1), height(1) {}
	int x;
	int y;
	int width;
	int height;
};

PixRect textureRect;

typedef std::vector< Vec2 > Line;

Color color0( 1.0, 0.5, 0.5, 1.0 );
Line line0;
Color color1( 0.1, 0.3, 0.3, 1.0 );
Line line1;
Line lineAll;
Color fillColor( 1.0, 1.0, 0.0, 1.0 );

BBox boundingBox;

void reshape(int w, int h)
{
	winWidth = w;
	winHeight = h;
}

void ComputeBoundingBox( BBox & bbox, Line & line )
{
	
	bbox.m_xMin = numeric_limits<double>::max();
	bbox.m_xMax = numeric_limits<double>::min();
	bbox.m_yMin = numeric_limits<double>::max();
	bbox.m_yMax = numeric_limits<double>::min();

	for( unsigned i = 0; i < line.size(); ++i )
	{
		Vec2 & point = line[i];
		if( point.m_x < bbox.m_xMin )
			bbox.m_xMin = point.m_x;
		if( point.m_x > bbox.m_xMax )
			bbox.m_xMax = point.m_x;
		if( point.m_y < bbox.m_yMin )
			bbox.m_yMin = point.m_y;
		if( point.m_y > bbox.m_yMax )
			bbox.m_yMax = point.m_y;
	}
}


void DrawQuad( PixRect & rect, int texWidth, int texHeight )
{
	if( texWidth == 0 )
		texWidth = rect.width;
	if( texHeight == 0 )
		texHeight = rect.height;
	double xmin = rect.x;
	double xmax = rect.x + rect.width;
	double ymin = rect.y;
	double ymax = rect.y + rect.height;
	glBegin(GL_QUADS);
		glTexCoord2d( 0, 0 ); glVertex2d( xmin, ymin ); 
		glTexCoord2d( texWidth, 0 ); glVertex2d( xmax, ymin ); 
		glTexCoord2d( texWidth, texHeight ); glVertex2d( xmax, ymax ); 
		glTexCoord2d( 0, texHeight ); glVertex2d( xmin, ymax ); 
	glEnd ();
}

void DrawShapeToStencilBitOne( Line & line )
{
	glDisable( GL_BLEND );
	glColorMask (GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);  // Don't draw to color buffer
	glStencilMask (0x01);						// Affect only bit 1
	glStencilOp (GL_KEEP, GL_KEEP, GL_INVERT);	// Affect only stencil pixels that pass the test (invert)
	glStencilFunc (GL_ALWAYS, 0, 0x01 );		// The stencil test is always passing

	// Draw triangle fan in the stencil buffer
	glBegin( GL_TRIANGLE_FAN );
		for( unsigned i = 0; i < line.size(); ++i )
		{
			Vec2 & point = line[i];
			glVertex2d( point.m_x, point.m_y );
		}
	glEnd();
}

void EraseStencilBitOne( PixRect & rect )
{
	glColorMask (GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glStencilMask( 0x01 );
	glStencilFunc( GL_ALWAYS, 0x00, 0xFF );
	glStencilOp( GL_ZERO, GL_ZERO, GL_ZERO );
	DrawQuad( rect, 0, 0 );
}

void glColor( Color & color )
{
	glColor4d( color.m_r, color.m_g, color.m_b, color.m_a );
}

void DrawFill( )
{
	// Draw the whole shape in bit 1 of the stencil buffer
	glEnable( GL_STENCIL_TEST );
	DrawShapeToStencilBitOne( lineAll );

	glStencilMask( 0x00 );
	glStencilFunc( GL_EQUAL, 0x01, 0x01 );
	glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	DrawQuad( textureRect, 0, 0 );

	// Erase bit 1 of the stencil buffer
	EraseStencilBitOne( textureRect );
}

void DrawLine( Line & line )
{
	if( line.size() == 0 )
		return;

	glBegin( GL_LINE_STRIP );
		for( unsigned i = 0; i < line.size(); ++i )
		{
			Vec2 & point = line[i];
			glVertex2d( point.m_x, point.m_y );
		}
	glEnd();
}

void DrawLines()
{
	glColor( color0 );
	DrawLine( line0 );
	glColor( color1 );
	DrawLine( line1 );
}

bool firstTime = true;
int currentLevel = 0;

void OneIteration()
{
	//--------------------------------------------------------------
	// Draw lines to input texture of the integrator
	//--------------------------------------------------------------
	if( firstTime )
	{
		// Start drawing in texture and set viewport
		glBindTexture( GL_TEXTURE_2D, 0 );
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, integrator.GetInputFB() );
		glPushAttrib( GL_VIEWPORT_BIT );
		glViewport( 0, 0, textureRect.width, textureRect.height );

		glMatrixMode( GL_PROJECTION );
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D( textureRect.x, textureRect.x + textureRect.width, textureRect.y, textureRect.y + textureRect.height );
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();

		// Clear the texture
		glClearColor( 0.0, 0.0, 0.0, 1.0 );
		glClear( GL_COLOR_BUFFER_BIT );

		DrawLines();

		firstTime = false;

		//--------------------------------------------------------------
		// do one iteration
		//--------------------------------------------------------------
		integrator.Iterate();
	}

	//--------------------------------------------------------------
	// Draw second texture to framebuffer
	//--------------------------------------------------------------
	glViewport( 0, 0, winWidth, winHeight );       
	glMatrixMode(GL_PROJECTION);  
	glLoadIdentity();             
	glOrtho(0, winWidth, 0, winHeight, -1, 1);
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	//scaleBiasShader.UseProgram( true );

	glActiveTextureARB(GL_TEXTURE0);
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
	VLevel & level = integrator.GetLevel( currentLevel );
	glBindTexture( GL_TEXTURE_RECTANGLE_ARB, level.f_tex );
	glColor4d( 1.0, 1.0, 1.0, 1.0 );
	//DrawQuad( textureRect, level.width, level.height );
	DrawFill();

	//scaleBiasShader.UseProgram( false );
}

void display(void)
{
	glViewport( 0, 0, winWidth, winHeight );       
	glMatrixMode(GL_PROJECTION);  
	glLoadIdentity();             
	glOrtho(0, winWidth, 0, winHeight, -1, 1);

	glEnable(GL_STENCIL_TEST);
	glStencilMask( 0xFF );
	glClearStencil( 0 );
	glClearColor( 0.0, 0.0, 0.0, 1.0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
	
	if( mode <= 1 )
	{
		DrawLines();
	}
	else
	{
		OneIteration();
	}

	glutSwapBuffers();
}

static void Key(unsigned char key, int x, int y)
{
	switch (key) 
	{
	case 'o':
	case 'O':
		if( currentLevel > 0 )
		{
			currentLevel--;
			glutPostRedisplay();
		}
		break;
	case 'p':
	case 'P':
		if( currentLevel < integrator.GetNumberOfLevels() - 1 )
		{
			currentLevel++;
			glutPostRedisplay();
		}
		break;
	case 'd':
	case 'D':
		glutPostRedisplay();
		break;
	case 'q':
	case 'Q':
		glutDestroyWindow(win);
		exit(0);
		break;
	}
}

bool isDrawing = false;

void Mouse( int button, int state, int x, int y )
{
	 if( button == GLUT_LEFT_BUTTON )
	 {
		 if( state == GLUT_DOWN )
		 {
			 Vec2 newPoint( (double)x, (double)(winHeight-y-1) );
			 if( mode == 0 )
				line0.push_back( newPoint );
			 else if( mode == 1 )
				 line1.push_back( newPoint );
			 if( mode <= 1 )
				isDrawing = true;
		 }
		 else
		 {
			 isDrawing = false;
			 ++mode;
			 if( mode > 1 )
			 {
				 lineAll = line0;
				 lineAll.insert( lineAll.end(), line1.begin(), line1.end() );
				 ComputeBoundingBox( boundingBox, lineAll );
				 textureRect.x = (int)boundingBox.m_xMin - 1;
				 textureRect.y = (int)boundingBox.m_yMin - 1;
				 textureRect.width = (int)boundingBox.GetWidth() + 2;
				 textureRect.height = (int)boundingBox.GetHeight() + 2;
				 integrator.Init( textureRect.width, textureRect.height );
			 }
			 glutPostRedisplay();
		 }
	 }
}

void Motion(int x, int y)
{
	if( isDrawing )
	{
		Vec2 newPoint( (double)x, (double)(winHeight-y-1) );
		if( mode == 0 )
			line0.push_back( newPoint );
		else if( mode == 1 )
			line1.push_back( newPoint );
		glutPostRedisplay();
	}
}


bool Init()
{
	glClearColor( 0.0, 0.0, 0.0, 1.0 );
	glClearStencil( 0 );
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	//glEnable (GL_STENCIL_TEST);
	//glEnable( GL_LINE_SMOOTH );
	//glEnable( GL_BLEND );
	//glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	//glEnable( GL_TEXTURE_2D );
	//glDisable (GL_LINE_SMOOTH);

	int nbStencilBits;
	glGetIntegerv( GL_STENCIL_BITS, &nbStencilBits );
	cout << "Number of stencil bits: " << nbStencilBits << endl;

	int maxNbSamples = 1;
	glGetIntegerv(GL_MAX_SAMPLES_EXT, &maxNbSamples);

	if( !GLEE_ARB_color_buffer_float )
		return false;
	
	glClampColorARB(GL_CLAMP_VERTEX_COLOR_ARB, GL_FALSE);
	glClampColorARB(GL_CLAMP_FRAGMENT_COLOR_ARB, GL_FALSE);
	glClampColorARB(GL_CLAMP_READ_COLOR_ARB, GL_FALSE);

	if( !scaleBiasShader.Init() )
	{
		cout << "Error initializing scale bias shader" << endl;
		return false;
	}

	return true;
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_STENCIL );
	glutInitWindowSize(winWidth, winHeight);
	win = glutCreateWindow("TestStencilBoolOp");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(Key);
	glutMouseFunc( Mouse );
	glutMotionFunc( Motion );

	if( !Init() )
		return -1;

	glutMainLoop();
	return 0;
}
