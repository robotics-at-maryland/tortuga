/////////////////////////////////////////////////////////////////////////////////
//
// shadowcasterfp.frag
//
// Hamilton Chong
// (c) 2006
//
// This is an example fragment shader for shadow caster objects.  
//
/////////////////////////////////////////////////////////////////////////////////

// I N P U T   V A R I A B L E S ////////////////////////////////////////////////

// uniform constants
uniform float uDepthOffset;           // offset amount (constant in eye space)
uniform float uSTexWidth;             // shadow map texture width
uniform float uSTexHeight;            // shadow map texture height
uniform mat4  uInvModelViewProjection;// inverse model-view-projection matrix
uniform mat4  uProjection;            // projection matrix

// per fragment inputs
varying vec4 pPosition;      // position of fragment (in homogeneous coordinates)
varying vec4 pNormal;        // un-normalized normal in object space
varying vec4 pModelPos;      // coordinates of model in object space at this point


// M A I N //////////////////////////////////////////////////////////////////////

void main(void)
{
    // compute the "normalized device coordinates" (no viewport applied yet)
    vec4 postProj = pPosition / pPosition.w;

    // get the normalized normal of the geometry seen at this point
    vec4 normal = normalize(pNormal);


    // -- Computing Depth Bias Quantities -----------------------------

    // We want to compute the "depth slope" of the polygon.  
    // This is the change in z value that accompanies a change in x or y on screen
    // such that the coordinates stay on the triangle. 
    // The depth slope, dzlen below, is a measure of the uncertainty in our z value 
    // Roughly, these equations come from re-arrangement of the product rule:
    // d(uq) = d(u)q + u d(q) --> d(u) = 1/q * (d(uq) - u d(q))
    vec4 duqdx = uInvModelViewProjection * vec4(1.0/uSTexWidth,0.0,0.0,0.0);
    vec4 dudx  = pPosition.w * (duqdx - (pModelPos * duqdx.w));
    vec4 duqdy = uInvModelViewProjection * vec4(0.0,1.0/uSTexHeight,0.0,0.0);
    vec4 dudy  = pPosition.w * (duqdy - (pModelPos * duqdy.w));
    vec4 duqdz = uInvModelViewProjection * vec4(0.0,0.0,1.0,0.0);
    vec4 dudz  = pPosition.w * (duqdz - (pModelPos * duqdz.w));
    // The next relations come from the requirement dot(normal, displacement) = 0
    float denom = 1.0 / dot(normal.xyz, dudz.xyz);
    vec2  dz = -  vec2( dot(normal.xyz, dudx.xyz) * denom , 
                         dot(normal.xyz, dudy.xyz) * denom );
    float  dzlen = max(abs(dz.x), abs(dz.y)); 


    // We now compute the change in z that would signify a push in the z direction
    // by 1 unit in eye space.  Note that eye space z is related in a nonlinear way to
    // screen space z, so this is not just a constant.  
    // ddepth below is how much screen space z at this point would change for that push.
    // NOTE: computation of ddepth likely differs from OpenGL's glPolygonOffset "unit"
    //  computation, which is allowed to be vendor specific.
    vec4 dpwdz = uProjection * vec4(0.0, 0.0, 1.0, 0.0);
    vec4 dpdz = (dpwdz - (postProj * dpwdz.w)) / pPosition.w;
    float  ddepth = abs(dpdz.z);

    // -- End depth bias helper section --------------------------------   

    // We now compute the depth of the fragment.  This is the actual depth value plus
    // our depth bias.  The depth bias depends on how uncertain we are about the z value
    // plus some constant push in the z direction.  The exact coefficients to use are
    // up to you, but at least it should be somewhat intuitive now what the tradeoffs are.
    float depthval = postProj.z + (0.5 * dzlen)+ (uDepthOffset * ddepth);
    depthval = (0.5 * depthval) + 0.5; // put into [0,1] range instead of [-1,1] 

    gl_FragColor = vec4(depthval, depthval, depthval, 0.0);
}