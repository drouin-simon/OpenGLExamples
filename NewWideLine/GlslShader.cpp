#include "GL/glew.h"
#include "GlslShader.h"
#include <iostream>
#include <stdio.h>
#include <stdarg.h>

using namespace std;

GlslShader::GlslShader()
	: m_glslShader(0)
	, m_glslVertexShader(0)
	, m_glslProg(0)
	, m_init( false )
{
}

GlslShader::~GlslShader()
{
	Clear();
}

void GlslShader::AddShaderMemSource( const char * src )
{
	m_memSources.push_back( std::string( src ) );
}

void GlslShader::AddVertexShaderMemSource( const char * src )
{
	m_vertexMemSources.push_back( std::string( src ) );
}

void GlslShader::Reset()
{
    m_compilationError.clear();
    m_memSources.clear();
    m_vertexMemSources.clear();
}

bool GlslShader::Init()
{
	// Fresh start
    Clear();
	
	// Load and try compiling vertex shader
    if( m_vertexMemSources.size() != 0 )
        if( !CreateAndCompileShader( GL_VERTEX_SHADER, m_glslVertexShader, m_vertexMemSources ) )
			return false;
	
	// Load and try compiling pixel shader
    if( m_memSources.size() != 0 )
        if( !CreateAndCompileShader( GL_FRAGMENT_SHADER, m_glslShader, m_memSources ) )
			return false;
	
    // Check that at least one of the shaders has been compiled
	if( m_glslVertexShader == 0 && m_glslShader == 0 )
		return false;

	// Create program object and attach shader
    m_glslProg = glCreateProgram();
	if( m_glslVertexShader )
        glAttachShader( m_glslProg, m_glslVertexShader );
	if( m_glslShader )
        glAttachShader( m_glslProg, m_glslShader );

	// Create program and link shaders
    glLinkProgram( m_glslProg );
	GLint success = 0;
    glGetProgramiv( m_glslProg, GL_LINK_STATUS, &success );
    if (!success)
    {
		GLint logLength = 0;
        glGetProgramiv( m_glslProg, GL_INFO_LOG_LENGTH, &logLength );
        GLchar * infoLog = new GLchar[ logLength + 1 ];
        glGetProgramInfoLog( m_glslProg, logLength, NULL, infoLog );
        m_compilationError += std::string( infoLog );
		delete [] infoLog;
		return false;
    }

	m_init = true;
	return true;
}

bool GlslShader::CreateAndCompileShader( unsigned shaderType, unsigned & shaderId, std::vector< std::string > & memSources )
{
	// put all the sources in an array of const GLchar*
    const GLchar ** shaderStringPtr = new const GLchar*[ memSources.size() ];
	for( unsigned i = 0; i < memSources.size(); ++i )
	{
		shaderStringPtr[i] = memSources[i].c_str();
	}
	
	// Create the shader and set its source
    shaderId = glCreateShader( shaderType );
    glShaderSource( shaderId, memSources.size(), shaderStringPtr, NULL);
	
	delete [] shaderStringPtr;
	
	// Compile the shader
	GLint success = 0;
    glCompileShader( shaderId );
    glGetShaderiv( shaderId, GL_COMPILE_STATUS, &success );
    if (!success)
    {
		GLint logLength = 0;
        glGetShaderiv( shaderId, GL_INFO_LOG_LENGTH, &logLength );
        GLchar * infoLog = new GLchar[logLength+1];
        glGetShaderInfoLog( shaderId, logLength, NULL, infoLog);
        m_compilationError += std::string( infoLog );
		delete [] infoLog;
        return false;
    }
	return true;
}

bool GlslShader::UseProgram( bool use )
{
	bool res = true;
	if( use && m_init )
	{
        glUseProgram( m_glslProg );
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
    int location = glGetUniformLocation( m_glslProg, name );
	if( location != -1 )
	{
        glUniform1i( location, value );
		return true;
	}
	return false;
}

bool GlslShader::SetVariable( const char * name, int count, int * values )
{
    int location = glGetUniformLocation( m_glslProg, name );
    if( location != -1 )
    {
        glUniform1iv( location, count, values );
        return true;
    }
    return false;
}

bool GlslShader::SetVariable( const char * name, float value )
{
    int location = glGetUniformLocation( m_glslProg, name );
	if( location != -1 )
	{
        glUniform1f( location, value );
		return true;
	}
	return false;
}

bool GlslShader::SetVariable( const char * name, double value )
{
    int location = glGetUniformLocation( m_glslProg, name );
    if( location != -1 )
    {
        glUniform1d( location, value );
        return true;
    }
    return false;
}

bool GlslShader::SetVariable( const char * name, int val1, int val2 )
{
    int location = glGetUniformLocation( m_glslProg, name );
    if( location != -1 )
    {
        glUniform2i( location, val1, val2 );
        return true;
    }
    return false;
}

bool GlslShader::SetVariable( const char * name, float val1, float val2 )
{
    int location = glGetUniformLocation( m_glslProg, name );
    if( location != -1 )
    {
        glUniform2f( location, val1, val2 );
        return true;
    }
    return false;
}

bool GlslShader::SetVariable( const char * name, float val1, float val2, float val3 )
{
    int location = glGetUniformLocation( m_glslProg, name );
    if( location != -1 )
    {
        glUniform3f( location, val1, val2, val3 );
        return true;
    }
    return false;
}

void GlslShader::Clear()
{
	if( m_glslVertexShader != 0 )
	{
        glDeleteShader( m_glslVertexShader );
		m_glslVertexShader = 0;
	}
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

