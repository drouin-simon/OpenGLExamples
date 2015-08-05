
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect img;
uniform sampler2DRect sub;
void main()
{
	gl_FragColor = texture2DRect(img, gl_TexCoord[0].xy)
	              -texture2DRect(sub, vec2(0.5,0.5));
}
