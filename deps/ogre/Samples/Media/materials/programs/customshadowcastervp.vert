//////////////////////////////////////////////////////////////////
//
// shadowcastervp.vert
//
// Hamilton Chong
// (c) 2006
//
// This is an example vertex shader for shadow caster objects.  
//
//////////////////////////////////////////////////////////////////


// I N P U T   V A R I A B L E S /////////////////////////////////

uniform mat4 uModelViewProjection;   // modelview projection matrix

// O U T P U T   V A R I A B L E S ///////////////////////////////

varying vec4 pPosition;      // post projection position coordinates
varying vec4 pNormal;        // normal in object space (to be interpolated)
varying vec4 pModelPos;      // position in object space (to be interpolated) 

// M A I N ///////////////////////////////////////////////////////

void main()
{
    // Transform vertex position into post projective (homogenous screen) space.
    gl_Position = uModelViewProjection * gl_Vertex;
    pPosition   = uModelViewProjection * gl_Vertex;
   
    // copy over data to interpolate using perspective correct interpolation
    pNormal   = vec4(gl_Normal.x, gl_Normal.y, gl_Normal.z, 0.0);
    pModelPos = gl_Vertex;
}