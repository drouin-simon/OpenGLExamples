#include "GlslShader.h"
#include "GLee.h"
#include <iostream>

using namespace std;

GlslShader::GlslShader() 
	: m_glslProg(0)
	, m_glslShader(0)
	, m_init( false )
{
}

GlslShader::~GlslShader() 
{
	Clear();
}

void GlslShader::AddShaderFilename( const char * filename )
{
	m_shaderFilenames.push_back( std::string( filename ) );
}

bool GlslShader::Init()
{
	GLint success = 0;

	// Fresh start
	Clear();

	// Create the array with all shader source
	int nbSources = m_shaderFilenames.size();
	std::vector< std::string > arraySource;
	arraySource.resize( nbSources );
	const GLchar ** shaderStringPtr = new const GLchar*[ nbSources ];
	for( int i = 0; i < nbSources; ++i )
	{
		if( !LoadOneShaderSource( m_shaderFilenames[i].c_str(), arraySource[i] ) )
			return false;
		shaderStringPtr[i] = arraySource[i].c_str();
	}
	
	// Create the shader and set its source
	m_glslShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_glslShader, nbSources, shaderStringPtr, NULL);

	// Compile the shader
	glCompileShader(m_glslShader);
    glGetShaderiv(m_glslShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
		GLint logLength = 0;
		glGetShaderiv(	m_glslShader, GL_INFO_LOG_LENGTH, &logLength );
        GLchar * infoLog = new GLchar[logLength+1];
        glGetShaderInfoLog( m_glslShader, logLength, NULL, infoLog);
		ReportError( "Error in fragment shader complilation: \n %s\n", infoLog );
		delete [] infoLog;
        return false;
    }

	// Create program object and attach shader
	m_glslProg = glCreateProgram();
	glAttachShader( m_glslProg, m_glslShader );

	// Link program
    glLinkProgram( m_glslProg );
    glGetProgramiv( m_glslProg, GL_LINK_STATUS, &success);
    if (!success)
    {
		GLint logLength = 0;
		glGetProgramiv(	m_glslProg, GL_INFO_LOG_LENGTH, &logLength );
		GLchar * infoLog = new GLchar[logLength+1];
        glGetProgramInfoLog(m_glslProg, logLength, NULL, infoLog);
		ReportError( "Error in glsl program linking: \n %s\n", infoLog );
		delete [] infoLog;
		return false;
    }

	m_init = true;
	return true;
}

bool GlslShader::UseProgram( bool use )
{
	bool res = true;
	if( use && m_init )
	{
		glUseProgram( m_glslProg );
		res = SetupVariables();
	}
	else
	{
		res = true;
		glUseProgram( 0 );
	}
	return res;
}

bool GlslShader::SetVariable( const char * name, int value )
{
	int location = glGetUniformLocationARB( m_glslProg, name );
	if( location != -1 )
	{
		glUniform1iARB( location, value );
		return true;
	}
	ReportError( "Couldn't set shader variable %s.\n", name );
	return false;
}

bool GlslShader::SetVariable( const char * name, float value )
{
	int location = glGetUniformLocationARB( m_glslProg, name );
	if( location != -1 )
	{
		glUniform1fARB( location, value );
		return true;
	}
	ReportError( "Couldn't set shader variable %s.\n", name );
	return false;
}

bool GlslShader::LoadOneShaderSource( const char * filename, std::string & shaderSource )
{
	// Open shader file for reading
	FILE * f = fopen( filename, "rb" );
	if( !f )
	{
		ReportError( "Couldn't open shader file %s\n", filename );
		return false;
	}

	// Get file size
	fseek( f, 0, SEEK_END );
    long length = ftell( f );
	fseek( f, 0, SEEK_SET );

	// Read file into shaderSource string
	char * charShaderSource = new char[ length + 1 ];
	fread( charShaderSource, 1, length, f );
	charShaderSource[length] = '\0';
	shaderSource = charShaderSource;
	delete [] charShaderSource;

	// close the file
    fclose( f );

	return true;
}

void GlslShader::Clear()
{
	if( m_glslShader != 0 )
	{
		glDeleteShader( m_glslShader );
		m_glslShader = 0;
	}
	if( m_glslProg != 0 )
	{
		glDeleteProgram( m_glslProg );
		m_glslProg = 0;
	}
	m_init = false;
}

void GlslShader::ReportError( const char * msg, ... )
{
	va_list args;
	va_start(args, msg);
	vprintf( msg, args );
	va_end(args);
}