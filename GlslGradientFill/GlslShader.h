#pragma once

#include <string>
#include <vector>

class GlslShader
{

public:

	GlslShader();
	~GlslShader();

	void AddShaderFilename( const char * filename );
	bool Init();
	bool UseProgram( bool use );
	bool SetVariable( const char * name, int value );
	bool SetVariable( const char * name, float value );

protected:

	virtual bool SetupVariables() = 0;

	bool LoadOneShaderSource( const char * filename, std::string & shaderSource );
	void Clear();
	void ReportError( const char * msg, ... );

	std::vector< std::string > m_shaderFilenames;
	unsigned m_glslShader;
	unsigned m_glslProg;
	bool m_init;

};