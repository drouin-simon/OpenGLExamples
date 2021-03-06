#ifndef __GlslShader_h_
#define __GlslShader_h_

#include <string>
#include <vector>

class GlslShader
{

public:

    GlslShader();
    ~GlslShader();

	void AddShaderMemSource( const char * src );
	void AddVertexShaderMemSource( const char * src );
    void Reset();

	bool Init();
	bool UseProgram( bool use );
	bool SetVariable( const char * name, int value );
    bool SetVariable( const char * name, int count, int * values );
	bool SetVariable( const char * name, float value );
    bool SetVariable( const char * name, double value );
    bool SetVariable( const char * name, int val1, int val2 );  // set a ivec2
    bool SetVariable( const char * name, float val1, float val2 );  // set a vec2
    bool SetVariable( const char * name, float val1, float val2, float val3 );  // set a vec3

protected:

    bool CreateAndCompileShader( unsigned shaderType, unsigned & shaderId, std::vector< std::string > & memSources );
	void Clear();

	std::vector< std::string > m_memSources;
	std::vector< std::string > m_vertexMemSources;
	unsigned m_glslShader;
	unsigned m_glslVertexShader;
	unsigned m_glslProg;
	bool m_init;

};

#endif
