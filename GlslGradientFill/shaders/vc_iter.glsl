
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect f;
//uniform sampler2DRect rhs;

//Description (rather terse, eh?) of the filter we're trying to invert:
// 'w_x' is a magic parameter...
//uniform float w_corner;
//uniform float w_edge;
//uniform float w_x;
//uniform float w_center_minus_x_inv;

void main()
{
	gl_FragColor = 0.125 * 
		  ( texture2DRect(f, gl_TexCoord[0].xy + vec2(1.0,0.0))
		  + texture2DRect(f, gl_TexCoord[0].xy + vec2(-1.0,0.0))
		  + texture2DRect(f, gl_TexCoord[0].xy + vec2(0.0,1.0))
		  + texture2DRect(f, gl_TexCoord[0].xy + vec2(0.0,-1.0))
		  + texture2DRect(f, gl_TexCoord[0].xy + vec2(1.0,1.0))
		  + texture2DRect(f, gl_TexCoord[0].xy + vec2(1.0,-1.0))
		  + texture2DRect(f, gl_TexCoord[0].xy + vec2(-1.0,1.0))
		  + texture2DRect(f, gl_TexCoord[0].xy + vec2(-1.0,-1.0)) );
	gl_FragColor.w = 1.0;
}
