/////////////////////////////////////////////////////////////////////////////////
//
// SimpleShaderFP.frag
//
// Hamilton Chong
// (c) 2006
//
//
/////////////////////////////////////////////////////////////////////////////////

// I N P U T   V A R I A B L E S ////////////////////////////////////////////////

// uniform constants
uniform sampler2D    TextureMap;

// per fragment inputs
varying vec4   pTexCoord;    	// vertex position in texture map coordinates
varying float  pDiffuse;        // diffuse shading value

// M A I N //////////////////////////////////////////////////////////////////////

void main(void)
{
    vec2 texCoord = pTexCoord.xy / pTexCoord.w;
    gl_FragColor = texture2D(TextureMap, texCoord);
    // Ogre is automatically handling diffuse lighting for us.
    // gl_FragColor *= pDiffuse;
}