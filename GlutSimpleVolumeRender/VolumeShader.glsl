#extension GL_ARB_texture_rectangle : require

uniform sampler2DRect back_tex_id;
uniform sampler3D volume_id;
const float stepSize = float(0.002);

void main()
{
    vec4 entryBack = texture2DRect( back_tex_id, gl_FragCoord.xy );
    if( entryBack.a == 0 )
        discard;
    vec4 entryFront = gl_Color;
    vec3 diff = entryBack.rgb - entryFront.rgb;

    float intensity = 0;
    int nbSteps = int( length( diff ) / stepSize );
    vec3 pos = entryFront.rgb;
    vec3 inc = normalize( diff );
    inc = inc * stepSize;
    for( int i = 0; i < nbSteps; ++i )
    {
        intensity += 0.1 * texture3D( volume_id, pos );
        if( intensity > 1.0 )
            break;
        pos += inc;
    }
    gl_FragColor.rgb = vec3( 1.0, 1.0, 1.0 );
    gl_FragColor.a = intensity;
}
