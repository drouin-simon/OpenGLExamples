#pragma once

#include "GlslShader.h"

class VcResidualShader : public GlslShader
{
public:
	VcResidualShader();
	~VcResidualShader();
	void SetParams( float corner, float edge, float center );
protected:
	virtual bool SetupVariables();
	float m_corner;
	float m_edge;
	float m_center;
};

class VcRestricShader : public GlslShader
{
public:
	VcRestricShader();
	~VcRestricShader();
protected:
	virtual bool SetupVariables();
};

class VcInterpolateAddShader : public GlslShader
{
public:
	VcInterpolateAddShader();
	~VcInterpolateAddShader();
protected:
	virtual bool SetupVariables();
};

class VcInterpolateShader : public GlslShader
{
public:
	VcInterpolateShader();
	~VcInterpolateShader();
protected:
	virtual bool SetupVariables();
};

class VcIterShader : public GlslShader
{
public:
	VcIterShader();
	~VcIterShader();
protected:
	virtual bool SetupVariables();
};

class VcRecenterScaleShader : public GlslShader
{
public:
	VcRecenterScaleShader();
	~VcRecenterScaleShader();
protected:
	virtual bool SetupVariables();
};

class VcRecenterSubShader : public GlslShader
{
public:
	VcRecenterSubShader();
	~VcRecenterSubShader();
protected:
	virtual bool SetupVariables();
};

