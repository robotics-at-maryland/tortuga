#ifndef __python_FMOD_h_01
#define __python_FMOD_h_01

//See best practices section in Py++ documentation

#include <vector>
#include <string>
#include <map>
#include <iostream>


//#include "fmod.h"
#include "fmod.hpp"


// First we create a magic namespace to hold all our aliases
namespace pyplusplus { namespace aliases {
    
 #include "python_fmod_aliases.h"
 
} } 

// then we exposed everything needed (and more) to ensure GCCXML makes them visible to Py++
//
namespace python_FMOD{ namespace details{
inline void instantiate(){
    
 #include "python_fmod_sizeof.h"
 
} } }

#endif