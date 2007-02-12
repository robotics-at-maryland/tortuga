#ifndef __python_ois_h_11
#define __python_ois_h_11

//See best practices section in Py++ documentation

#include <vector>
#include <string>
#include <map>
#include "OIS.h"

// First we create a magic namespace to hold all our aliases
namespace pyplusplus { namespace aliases {
    
 #include "python_ois_aliases.h"
} } 

// then we exposed everything needed (and more) to ensure GCCXML makes them visible to Py++
//
namespace python_ois{ namespace details{
inline void instantiate(){
 using namespace OIS;
 #include "python_ois_sizeof.h"
 
} } }


#endif//__python_ogre_h_5B402FBF_8DE8_49C9_AF71_1CF9634B5344__
