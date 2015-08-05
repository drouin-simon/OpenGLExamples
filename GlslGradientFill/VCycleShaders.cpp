#include "VCycleShaders.h"
#include "GLee.h"

//---------------------------------------------------------
// VcResidualShader
//---------------------------------------------------------

VcResidualShader::VcResidualShader()
: m_corner( 4.0f )
, m_edge( 5.0f )
, m_center( 0.0f )
{
	AddShaderFilename( "shaders/vc_residual.glsl" );
}

VcResidualShader::~VcResidualShader()
{
}

void VcResidualShader::SetParams( float corner, float edge, float center )
{
	m_corner = corner;
	m_edge = edge;
	m_center = center;
}

bool VcResidualShader::SetupVariables()
{
	bool res = true;
	res &= SetVariable( "f", (int)0 );
	res &= SetVariable( "rhs", (int)1 );
	res &= SetVariable( "w_corner", m_corner );
	res &= SetVariable( "w_edge", m_edge );
	res &= SetVariable( "w_center", m_center );
	return res;
}

//---------------------------------------------------------
// VcRestricShader
//---------------------------------------------------------

VcRestricShader::VcRestricShader()
{
	AddShaderFilename( "shaders/vc_restrict.glsl" );
}

VcRestricShader::~VcRestricShader()
{
}

bool VcRestricShader::SetupVariables()
{
	return SetVariable( "img", (int)0 );
}

//---------------------------------------------------------
// VcInterpolateAddShader
//---------------------------------------------------------

VcInterpolateAddShader::VcInterpolateAddShader()
{
	AddShaderFilename( "shaders/vc_interpolate_add.glsl" );
}

VcInterpolateAddShader::~VcInterpolateAddShader()
{
}

bool VcInterpolateAddShader::SetupVariables()
{
	bool res = true;
	res &= SetVariable( "small", (int)0 );
	res &= SetVariable( "add", (int)1 );
	return res;
}

//---------------------------------------------------------
// VcInterpolateShader
//---------------------------------------------------------

VcInterpolateShader::VcInterpolateShader()
{
	AddShaderFilename( "shaders/vc_interpolate.glsl" );
}

VcInterpolateShader::~VcInterpolateShader()
{
}

bool VcInterpolateShader::SetupVariables()
{
	return SetVariable( "small", (int)0 );
}


//---------------------------------------------------------
// VcIterShader
//---------------------------------------------------------

VcIterShader::VcIterShader()
{
	AddShaderFilename( "shaders/vc_iter.glsl" );
}

VcIterShader::~VcIterShader()
{
}

bool VcIterShader::SetupVariables()
{
	bool res = true;
	res &= SetVariable( "f", (int)0 );
	return res;
}


//---------------------------------------------------------
// VcRecenterScaleShader
//---------------------------------------------------------

VcRecenterScaleShader::VcRecenterScaleShader()
{
	AddShaderFilename( "shaders/vc_recenter_scale.glsl" );
}

VcRecenterScaleShader::~VcRecenterScaleShader()
{
}

bool VcRecenterScaleShader::SetupVariables()
{
	return SetVariable( "img", (int)0 );
}

//---------------------------------------------------------
// VcRecenterSubShader
//---------------------------------------------------------

VcRecenterSubShader::VcRecenterSubShader()
{
	AddShaderFilename( "shaders/vc_recenter_sub.glsl" );
}

VcRecenterSubShader::~VcRecenterSubShader()
{
}

bool VcRecenterSubShader::SetupVariables()
{
	bool res = true;
	res &= SetVariable( "img", (int)0 );
	res &= SetVariable( "sub", (int)1 );
	return res;
}
