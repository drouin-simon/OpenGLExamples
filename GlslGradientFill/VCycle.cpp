#include "VCycle.h"
#include <sstream>
#include <fstream>
#include "GLee.h"

using std::ostringstream;
using std::ofstream;

#include "gl_helpers.h"

using std::swap;

VCycle::VCycle() 
: m_width( 1 )
, m_height( 1 )
, m_firstIteration( true )
{
}

VCycle::~VCycle()
{
}

bool VCycle::Init( unsigned width, unsigned height ) 
{
	if( !GLEE_EXT_framebuffer_object ||
		!GLEE_ARB_texture_rectangle ||
		!GLEE_ARB_texture_float ||
		!GLEE_ARB_color_buffer_float ||
		!GLEE_ARB_shader_objects ||
		!GLEE_ARB_multitexture )
		return false;

	if( width != m_width || height != m_height )
	{
		m_width = width;
		m_height = height;
		Clean();
	}

	//allocate levels:
	unsigned cur_width = m_width;
	unsigned cur_height = m_height;
	while (1) 
	{
		levels.push_back(VLevel());
		VLevel &lev = levels.back();

		// coefs:
		float s = (1 << (levels.size()-1));
		float m = -8*s*s-4;
		float e = s*s + 2;
		float c = s*s - 1;
		m /= 3*s*s;
		e /= 3*s*s;
		c /= 3*s*s;
		lev.corner = c;
		lev.edge   = e;
		lev.middle = m;
		//This is what we say in the paper:
		lev.magic_xA = -2.1532 + 0.5882 / (s*s) + 1.5070 / s;
		lev.magic_xB = 0.1138 + 1.5065 / (s*s) + 0.9529 / s;

		lev.width = cur_width;
		lev.height = cur_height;

		alloc_tex(cur_width, cur_height, lev.f_tex);
		alloc_fb(lev.f_tex, lev.f_fb);

		alloc_tex(cur_width, cur_height, lev.rhs_tex);
		alloc_fb(lev.rhs_tex, lev.rhs_fb);

		alloc_tex(cur_width, cur_height, lev.f_temp_tex);
		alloc_fb(lev.f_temp_tex, lev.f_temp_fb);

		if (cur_width < 2 && cur_height < 2) 
			break;
		cur_width = ((cur_width - 1) / 2) + 1;
		cur_height = ((cur_height - 1) / 2) + 1;
	}

	unsigned int awidth = 64;
	unsigned int aheight = 64;
	while (awidth > m_width) 
		awidth >>= 1;
	while (aheight > m_height) 
		aheight >>= 1;

	while (1) 
	{
		avg.push_back(ALevel());
		ALevel &lev = avg.back();
		lev.width = awidth;
		lev.height = aheight;

		alloc_tex(awidth, aheight, lev.tex);
		alloc_fb(lev.tex, lev.fb);

		if (awidth == 1 && aheight == 1) 
			break;

		if (awidth > 1) 
			awidth >>= 1;
		if (aheight > 1) 
			aheight >>= 1;
	}

	// Init shaders
	m_residualShader.Init();
	m_restrictShader.Init();
	m_interpolateAddShader.Init();
	m_interpolateShader.Init();
	m_iterShader.Init();
	m_recenterScaleShader.Init();
	m_recenterSubShader.Init();

	m_firstIteration = true;
}

void VCycle::Clean()
{

}

void VCycle::Smooth( VLevel & lev, float magic_x ) 
{
	bind_fb(lev.f_temp_fb, lev.width, lev.height);

	//Set up shader for this level:
	m_iterShader.UseProgram( true );

	glActiveTextureARB(GL_TEXTURE1);
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, lev.rhs_tex);
	set_nearest();

	glActiveTextureARB(GL_TEXTURE0);
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, lev.f_tex);
	set_clamp_to_edge(); 
	set_nearest();

	glBegin(GL_QUADS);
	glTexCoord2f(0,0); glVertex2f(0,0);
	glTexCoord2f(lev.width,0); glVertex2f(lev.width,0);
	glTexCoord2f(lev.width,lev.height); glVertex2f(lev.width,lev.height);
	glTexCoord2f(0,lev.height); glVertex2f(0,lev.height);
	glEnd();

	glActiveTextureARB(GL_TEXTURE1);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);

	glActiveTextureARB(GL_TEXTURE0);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);

	m_iterShader.UseProgram( false );

	unbind_fb();

	swap(lev.f_temp_fb, lev.f_fb);
	swap(lev.f_temp_tex, lev.f_tex);
}

void VCycle::Coarsify( bool firstIteration, VLevel & lev, VLevel & to ) 
{
	// downsample current solution if !firstIteration
	// downsample boundary values if firstIteration
	unsigned fromTex = firstIteration ? lev.rhs_tex : lev.f_tex;
	unsigned toFb = firstIteration ? to.rhs_fb : to.f_fb;
	
	bind_fb(toFb, to.width, to.height);

	glActiveTextureARB(GL_TEXTURE0);
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, fromTex );
	set_clamp_to_black(); //otherwise left, top edges blow up.
	set_linear();

	m_restrictShader.UseProgram(true);

	glBegin(GL_QUADS);
	glTexCoord2f(-0.5f, -0.5f); glVertex2f(0.0f, 0.0f);
	glTexCoord2f(2*to.width-0.5f, -0.5f); glVertex2f(to.width, 0.0f);
	glTexCoord2f(2*to.width-0.5f,2*to.height-0.5f); glVertex2f(to.width, to.height);
	glTexCoord2f(-0.5f,2*to.height-0.5f); glVertex2f(0.0f, to.height);
	glEnd();

	glActiveTextureARB(GL_TEXTURE0);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);

	m_restrictShader.UseProgram(false);

	unbind_fb();
}

void VCycle::Interpolate( bool first_level, VLevel & lev, VLevel & from ) 
{
	glDisable(GL_BLEND);

	bind_fb( lev.f_fb, lev.width, lev.height );

	m_interpolateAddShader.UseProgram( true );

	glActiveTextureARB(GL_TEXTURE1);
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, lev.rhs_tex);
	set_nearest();

	glActiveTextureARB(GL_TEXTURE0);
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, from.f_tex);
	set_clamp_to_edge();
	set_linear(); 

	// draw quad
	glBegin(GL_QUADS);
	glMultiTexCoord2fARB(GL_TEXTURE1, 0.0f, 0.0f );
	glTexCoord2f(0.25f, 0.25f);
	glVertex2f(0.0f, 0.0f);

	glMultiTexCoord2fARB(GL_TEXTURE1, lev.width, 0.0f );
	glTexCoord2f(0.25f+0.5f*lev.width, 0.25f);
	glVertex2f(lev.width, 0.0f);

	glMultiTexCoord2fARB(GL_TEXTURE1, lev.width, lev.height );
	glTexCoord2f(0.25f+0.5f*lev.width, 0.25f+0.5f*lev.height);
	glVertex2f(lev.width, lev.height);

	glMultiTexCoord2fARB(GL_TEXTURE1, 0.0f, lev.height );
	glTexCoord2f(0.25f, 0.25f+0.5f*lev.height);
	glVertex2f(0.0f, lev.height);
	glEnd();

	glActiveTextureARB(GL_TEXTURE1);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);

	glActiveTextureARB(GL_TEXTURE0);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);

	m_interpolateAddShader.UseProgram( false );

	unbind_fb();
}


void VCycle::Iterate() 
{
	glDisable(GL_BLEND); //just to be sure.

	//down:
	for (unsigned int l = 0; l + 1 < levels.size(); ++l) 
	{
		Coarsify( m_firstIteration, levels[l], levels[l+1] );
	}

	bind_fb(levels.back().f_fb, levels.back().width, levels.back().height);
	glClear(GL_COLOR_BUFFER_BIT);
	unbind_fb();

	//up:
	for (unsigned int l = levels.size() - 2; l < levels.size(); --l) 
	{
		//interpolate:
		Interpolate( l == 0, levels[l], levels[l+1] );
		//post-smooth:
		Smooth( levels[l], levels[l].magic_xA );
		//Smooth( levels[l], levels[l].magic_xB );
	}

	//recenter:
	//a) first come up with an 'average color':
	/*for (unsigned int a = 0; a < avg.size(); ++a) 
	{
		ALevel &lev = avg[a];
		bind_fb(lev.fb, lev.width, lev.height);

		m_recenterScaleShader.UseProgram( true );

		glEnable(GL_TEXTURE_RECTANGLE_ARB);
		unsigned int tw, th;
		if (a == 0) 
		{
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, levels[0].f_tex);
			tw = levels[0].width;
			th = levels[0].height;
		} 
		else 
		{
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, avg[a-1].tex);
			tw = avg[a-1].width;
			th = avg[a-1].height;
		}
		set_clamp_to_edge();
		set_linear();

		glBegin(GL_QUADS);
		glTexCoord2f(0,0);   glVertex2f(0, 0);
		glTexCoord2f(tw,0);  glVertex2f(lev.width, 0);
		glTexCoord2f(tw,th); glVertex2f(lev.width, lev.height);
		glTexCoord2f(0,th);  glVertex2f(0, lev.height);
		glEnd();

		glDisable(GL_TEXTURE_RECTANGLE_ARB);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);

		m_recenterScaleShader.UseProgram( false );

		unbind_fb();
	}

	//b) actually recenter:

	bind_fb(levels[0].f_temp_fb, levels[0].width, levels[0].height);

	m_recenterSubShader.UseProgram( true );

	glActiveTextureARB(GL_TEXTURE1);
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, avg.back().tex);
	set_nearest(); set_clamp_to_edge();

	glActiveTextureARB(GL_TEXTURE0);
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, levels[0].f_tex);
	set_nearest();

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2f(0, 0);
	glTexCoord2f(levels[0].width, 0); glVertex2f(levels[0].width, 0);
	glTexCoord2f(levels[0].width, levels[0].height); glVertex2f(levels[0].width, levels[0].height);
	glTexCoord2f(0, levels[0].height); glVertex2f(0, levels[0].height);
	glEnd();

	glActiveTextureARB(GL_TEXTURE1);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);

	glActiveTextureARB(GL_TEXTURE0);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);

	m_recenterSubShader.UseProgram( false );

	unbind_fb();

	swap(levels[0].f_tex, levels[0].f_temp_tex);
	swap(levels[0].f_fb, levels[0].f_temp_fb);*/
}



