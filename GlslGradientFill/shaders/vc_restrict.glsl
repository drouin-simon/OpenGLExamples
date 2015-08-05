
#extension GL_ARB_texture_rectangle : enable

//Note: we're expecting texture coordinates to be 2x fragment coordinates.
uniform sampler2DRect img;
const vec4 zero = vec4( 0.0, 0.0, 0.0, 1.0 );

void main()
{
	int nbContribs = 0;
	vec4 accum = vec4( 0.0, 0.0, 0.0, 0.0 );
	
	vec4 pix = texture2DRect(img, gl_TexCoord[0].xy+vec2(-1.0,-1.0));
	if( pix != zero )
	{
		accum += pix;
		nbContribs++;
	}
	
	pix = texture2DRect(img, gl_TexCoord[0].xy+vec2(-1.0,-0.0));
	if( pix != zero )
	{
		accum += pix;
		nbContribs++;
	}
	
	pix = texture2DRect(img, gl_TexCoord[0].xy+vec2(-1.0,1.0));
	if( pix != zero )
	{
		accum += pix;
		nbContribs++;
	}
	
	pix = texture2DRect(img, gl_TexCoord[0].xy+vec2(0.0,-1.0));
	if( pix != zero )
	{
		accum += pix;
		nbContribs++;
	}
	
	pix = texture2DRect(img, gl_TexCoord[0].xy);
	if( pix != zero )
	{
		accum += pix;
		nbContribs++;
	}
	
	pix = texture2DRect(img, gl_TexCoord[0].xy+vec2(0.0,1.0));
	if( pix != zero )
	{
		accum += pix;
		nbContribs++;
	}
	
	pix = texture2DRect(img, gl_TexCoord[0].xy+vec2(1.0,-1.0));
	if( pix != zero )
	{
		accum += pix;
		nbContribs++;
	}
	
	pix = texture2DRect(img, gl_TexCoord[0].xy+vec2(1.0,0.0));
	if( pix != zero )
	{
		accum += pix;
		nbContribs++;
	}
	
	pix = texture2DRect(img, gl_TexCoord[0].xy+vec2(1.0,1.0));
	if( pix != zero )
	{
		accum += pix;
		nbContribs++;
	}
	
	if( nbContribs > 0 )
	{
		float factor = 1.0 / float(nbContribs);
		gl_FragColor = accum * factor;
	}
	else
		gl_FragColor = zero;
	
	//use linear interp hardware:
	//gl_FragColor =
		 /*texture2DRect(img, gl_TexCoord[0].xy+vec2(-0.5,-0.5))
		+texture2DRect(img, gl_TexCoord[0].xy+vec2(-0.5, 0.5))
		+texture2DRect(img, gl_TexCoord[0].xy+vec2( 0.5, 0.5))
		+texture2DRect(img, gl_TexCoord[0].xy+vec2( 0.5,-0.5))*/
	 //(nearest-neighbor hardware version):
		/*0.25 * ( 0.25*texture2DRect(img, gl_TexCoord[0].xy+vec2(-1,-1))
		+0.50*texture2DRect(img, gl_TexCoord[0].xy+vec2( 0,-1))
		+0.25*texture2DRect(img, gl_TexCoord[0].xy+vec2( 1,-1))
		+0.50*texture2DRect(img, gl_TexCoord[0].xy+vec2(-1, 0))
		+1.00*texture2DRect(img, gl_TexCoord[0].xy)
		+0.50*texture2DRect(img, gl_TexCoord[0].xy+vec2( 1, 0))
		+0.25*texture2DRect(img, gl_TexCoord[0].xy+vec2(-1, 1))
		+0.50*texture2DRect(img, gl_TexCoord[0].xy+vec2( 0, 1))
		+0.25*texture2DRect(img, gl_TexCoord[0].xy+vec2( 1, 1)) )
		;*/
}
