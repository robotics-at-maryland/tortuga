#ifndef __python_ogreal_h_5B402FBF_8DE8_49C9_AF71_1CF9634B5344__
#define __python_ogreal_h_5B402FBF_8DE8_49C9_AF71_1CF9634B5344__

//See best practices section in Py++ documentation
//#include "../ogre/python_ogre_masterlist.h"
#include "OgreAL.h"

// First we create a magic namespace to hold all our aliases
namespace pyplusplus { namespace aliases {
    
 #include "python_ogreal_aliases.h"
} } 

// then we exposed everything needed (and more) to ensure GCCXML makes them visible to Py++
//
namespace python_ogreal{ namespace details{
inline void instantiate(){
 //using namespace OgreAL;
 #include "python_ogreal_sizeof.h"
 
} 
} 
}


#endif//__python_ogreal_h_5B402FBF_8DE8_49C9_AF71_1CF9634B5344__
