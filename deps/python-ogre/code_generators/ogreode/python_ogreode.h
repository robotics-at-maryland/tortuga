#ifndef __python_ogreode_h_01
#define __python_ogreode_h_01

#include "ode/ode.h"
#include "ode/odecpp.h"
#include "../ogre/python_ogre_masterlist.h"

//See best practices section in Py++ documentation
// NOTE:  I had to manually add these includes instead of relying on OgreOde_Core as the "order"
// of them is wrong in Ogreode_Core.h and gccxml complains...
//
//#include "OgreOde_Core.h"

#include "OgreOdePreReqs.h"
#include "OgreOdeBody.h"
#include "OgreOdeGeometry.h"
#include "OgreOdeUtility.h"
#include "OgreOdeWorld.h"
#include "OgreOdeMass.h"
#include "OgreOdeJoint.h"
#include "OgreOdeSpace.h"
#include "OgreOdeCollision.h"
#include "OgreOdeStepper.h"
#include "OgreOdeEntityInformer.h"
#include "OgreOdeDebugObject.h"


#include "OgreOde_Prefab.h"
#include "OgreOde_Loader.h"

// First we create a magic namespace to hold all our aliases
namespace pyplusplus { namespace aliases {
    
 #include "python_ogreode_aliases.h"
} } 

// then we exposed everything needed (and more) to ensure GCCXML makes them visible to Py++
//
namespace python_ogreode{ namespace details{
inline void instantiate(){
 using namespace OgreOde;
 #include "python_ogreode_sizeof.h"
 
} } }





#endif