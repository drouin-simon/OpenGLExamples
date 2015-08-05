
#extension GL_ARB_texture_rectangle : enable

//small is approx 2x smaller than this.
// Assume: Texture coordinates aready set.
// (basically, this is to get around possible clamping if I use fixed-function)
uniform sampler2DRect small;
uniform sampler2DRect add;

const vec4 zero = vec4( 0.0, 0.0, 0.0, 1.0 );

void main()
{
	vec4 lineColor = texture2DRect(add, gl_TexCoord[1].xy);
	if( lineColor != zero )
		gl_FragColor = lineColor;
	else
		gl_FragColor = texture2DRect(small, gl_TexCoord[0].xy);
}
