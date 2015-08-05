#pragma once

#include "VCycleShaders.h"
#include <vector>

using std::vector;

class VLevel 
{
public:
	VLevel() : f_tex(0), f_fb(0), f_temp_tex(0), f_temp_fb(0), rhs_tex(0), rhs_fb(0) { }
	//level of solution; want to solve:
	// stencil * F = RHS
	//describe stencil:
	float middle, edge, corner, magic_xA, magic_xB;
	unsigned f_tex;
	unsigned f_fb;
	unsigned f_temp_tex;
	unsigned f_temp_fb;
	unsigned rhs_tex;
	unsigned rhs_fb;
	unsigned int width;
	unsigned int height;
};

class ALevel 
{
public:
	//levels used for centering average.
	unsigned int width;
	unsigned int height;
	unsigned tex;
	unsigned fb;
};

class VCycle
{

public:

	VCycle();
	~VCycle();
	bool Init( unsigned width, unsigned height );
	void Iterate();
	unsigned GetInputFB() { return levels[ 0 ].rhs_fb; }
	unsigned GetOuputTex() { return levels[ 0 ].f_tex; }

	int GetNumberOfLevels() { return (int)levels.size(); }
	VLevel & GetLevel( int levelIndex ) { return levels[ levelIndex ]; }

protected:

	void Smooth( VLevel & lev, float magic_x );
	void Coarsify( bool first_level, VLevel & lev, VLevel & to );
	void Interpolate( bool first_level, VLevel & lev, VLevel & from );
	void Clean();

	vector< VLevel > levels;
	vector< ALevel > avg;

	unsigned m_width;
	unsigned m_height;

	bool m_firstIteration;

	//Graphics::ProgramObjectRef rhs_shader;			//build initial RHS

	VcResidualShader		m_residualShader;		//compute residual
	VcRestricShader			m_restrictShader;		//restrict from fine to coarse
	VcInterpolateAddShader	m_interpolateAddShader;	//interpolate from coarse to fine (and add to old version)
	VcInterpolateShader		m_interpolateShader;	//interpolate from coarse to fine (no old version required)
	VcIterShader			m_iterShader;			//step toward solution

	VcRecenterScaleShader	m_recenterScaleShader;	//null shader to avoid clamping. used when recentering.
	VcRecenterSubShader		m_recenterSubShader;	//subtract value in 1px image from final rendering.

};
