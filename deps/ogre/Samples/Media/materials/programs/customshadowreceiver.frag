/////////////////////////////////////////////////////////////////////////////////
//
// shadowreceiver.frag
//
// Hamilton Chong
// (c) 2006
//
/////////////////////////////////////////////////////////////////////////////////

// I N P U T   V A R I A B L E S ////////////////////////////////////////////////

// uniform constants
uniform sampler2D    uShadowMap;
uniform float        uSTexWidth;
uniform float        uSTexHeight;

// per fragment inputs
varying vec4   pShadowCoord;    // vertex position in shadow map coordinates
varying float  pDiffuse;        // diffuse shading value

// M A I N //////////////////////////////////////////////////////////////////////

void main(void)
{
    // compute the shadow coordinates for texture lookup
    // NOTE: texture_viewproj_matrix maps z into [0,1] range, not [-1,1], so
    //  have to make sure shadow caster stores depth values with same convention.
    vec4 scoord = pShadowCoord / pShadowCoord.w;


    // -- "Percentage Closest Filtering" ----------------------------------------- 

    // One could use scoord.xy to look up the shadow map for depth testing, but
    // we'll be implementing a simple "percentage closest filtering" algorithm instead.
    // This mimics the behavior of turning on bilinear filtering on NVIDIA hardware
    // when also performing shadow comparisons.  This causes bilinear filtering of
    // depth tests.  Note that this is NOT the same as bilinear filtering the depth
    // values and then doing the depth comparison.  The two operations are not 
    // commutative.  PCF is explicitly about filtering the test values since
    // testing filtered z values is often meaningless.  

    // Real percentage closest filtering should sample from the entire footprint
    // on the shadow map, not just seek the closest four sample points.  Such 
    // an improvement is for future work.

    
    // NOTE: Assuming OpenGL convention for texture lookups with integers in centers.
    //  DX convention is to have integers mark sample corners
    vec2 tcoord;
    tcoord.x = (scoord.x * uSTexWidth) - 0.5;
    tcoord.y = (scoord.y * uSTexHeight) - 0.5;
    float x0 = floor(tcoord.x);
    float x1 = ceil(tcoord.x);
    float fracx = fract(tcoord.x);
    float y0 = floor(tcoord.y);
    float y1 = ceil(tcoord.y);
    float fracy = fract(tcoord.y);
    
    // sample coordinates in [0,1]^2 domain
    vec2 t00, t01, t10, t11;
    float invWidth  = 1.0 / uSTexWidth;
    float invHeight = 1.0 / uSTexHeight;
    t00 = vec2((x0+0.5) * invWidth, (y0+0.5) * invHeight);
    t10 = vec2((x1+0.5) * invWidth, (y0+0.5) * invHeight);
    t01 = vec2((x0+0.5) * invWidth, (y1+0.5) * invHeight);
    t11 = vec2((x1+0.5) * invWidth, (y1+0.5) * invHeight);
    
    // grab the samples
    float z00 = texture2D(uShadowMap, t00).x;
    float viz00 = (z00 <= scoord.z) ? 0.0 : 1.0;
    float z01 = texture2D(uShadowMap, t01).x;
    float viz01 = (z01 <= scoord.z) ? 0.0 : 1.0;
    float z10 = texture2D(uShadowMap, t10).x;
    float viz10 = (z10 <= scoord.z) ? 0.0 : 1.0;
    float z11 = texture2D(uShadowMap, t11).x;
    float viz11 = (z11 <= scoord.z) ? 0.0 : 1.0;

    // determine that all geometry outside the shadow test frustum is lit
    viz00 = ((abs(t00.x - 0.5) <= 0.5) && (abs(t00.y - 0.5) <= 0.5)) ? viz00 : 1.0;
    viz01 = ((abs(t01.x - 0.5) <= 0.5) && (abs(t01.y - 0.5) <= 0.5)) ? viz01 : 1.0;
    viz10 = ((abs(t10.x - 0.5) <= 0.5) && (abs(t10.y - 0.5) <= 0.5)) ? viz10 : 1.0; 
    viz11 = ((abs(t11.x - 0.5) <= 0.5) && (abs(t11.y - 0.5) <= 0.5)) ? viz11 : 1.0;

    // bilinear filter test results
    float v0 = (1.0 - fracx) * viz00 + fracx * viz10;
    float v1 = (1.0 - fracx) * viz01 + fracx * viz11;
    float visibility = (1.0 - fracy) * v0 + fracy * v1;

    // ------------------------------------------------------------------------------

    // Non-PCF code (comment out above section and uncomment the following three lines)

    //float zvalue = texture2D(uShadowMap, scoord.xy).x;
    //float visibility = (zvalue <= scoord.z) ? 0.0 : 1.0;
    //visibility = ((abs(scoord.x - 0.5) <= 0.5) && (abs(scoord.y - 0.5) <= 0.5)) ? visibility : 1.0;
    
    // ------------------------------------------------------------------------------

    visibility *= pDiffuse;
    gl_FragColor = vec4(visibility, visibility, visibility, 0.0);
}