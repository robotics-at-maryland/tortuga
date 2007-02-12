#ifndef __python_ode_h_01
#define __python_ode_h_01

//See best practices section in Py++ documentation

#include <vector>
#include <string>
#include <map>

#include "ode/ode.h"
// #include "ode/common.h"
#include "ode/odecpp.h"
#include "ode/odecpp_collision.h"

//#include "collision_kernel.h"
//#include "collision_std.h"
//#include "objects.h"


//namespace python_ode{ 

    inline void instantiate(){
        sizeof ( dMatrix3);
        sizeof ( dQuaternion );
        sizeof (dVector3);
//    
//        //class dxWorld {};
//                         
        sizeof (dWorldID);
        sizeof (dSpaceID);
        sizeof (dBodyID);
        sizeof (dGeomID);
        sizeof (dJointID);
        sizeof (dJointGroupID);
        
        
    }

//}

#endif