//some gl shorthand, because it was getting too duplicated.

namespace {

void set_clamp_to_black() {
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	static const float black[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glTexParameterfv(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_BORDER_COLOR, black );
}

void set_clamp_to_edge() {
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void set_nearest() {
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void set_linear() {
	#ifdef ATI_HACK
		assert(0);
	#endif
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void alloc_tex(unsigned int width, unsigned int height, GLuint &tex, bool alpha = false) 
{
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	GLenum int_fmt = alpha?GL_RGBA16F_ARB:GL_RGB16F_ARB;
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, int_fmt, width, height, 0, GL_RGB, GL_FLOAT, 0 );
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
}

bool alloc_fb(GLuint tex, GLuint &fb) 
{
	bool success = true;
	glGenFramebuffersEXT(1, &fb);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, tex, 0);

	GLenum ret = glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT );
	if (ret != GL_FRAMEBUFFER_COMPLETE_EXT )
		success = false;

	// clear the texture
	glClearColor( 0.0, 0.0, 0.0, 1.0 );
	glClear( GL_COLOR_BUFFER_BIT );

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	return success;
}

void bind_fb(GLuint framebuffer, unsigned int width, unsigned int height) 
{
	glPushAttrib(GL_VIEWPORT_BIT);

	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glTranslatef(-1.0f, -1.0f, 0.0f);
	glScalef(2.0f / width, 2.0f / height, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer);
}

void unbind_fb() 
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glPopAttrib();
}

}
