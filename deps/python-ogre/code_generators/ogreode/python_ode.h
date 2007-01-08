#ifndef __python_ode_h_01
#define __python_ode_h_01

//See best practices section in Py++ documentation

#include <vector>
#include <string>
#include <map>

#include "ode/ode.h"
#include "ode/common.h"
#include "ode/odecpp.h"
#include "ode/odecpp_collision.h"


//namespace python_ode{ 

    inline void instantiate(){
        struct dxWorld;		/* dynamics world */
        struct dxSpace;		/* collision space */
        struct dxBody;		/* rigid body (dynamics object) */
        struct dxGeom;		/* geometry (collision object) */
        struct dxJoint;
        struct dxJointNode;
        struct dxJointGroup;

        typedef struct dxWorld *dWorldID;
        typedef struct dxSpace *dSpaceID;
        typedef struct dxBody *dBodyID;
        typedef struct dxGeom *dGeomID;
        typedef struct dxJoint *dJointID;
        typedef struct dxJointGroup *dJointGroupID;

       //class dxWorld {};
        sizeof ( dxSpace *);
        sizeof ( dxBody *);
        sizeof ( dxJoint *);
        sizeof ( dxJointNode*);
        sizeof ( dxJointGroup*) ;
                
        sizeof (dWorldID);
        sizeof (dSpaceID);
        sizeof (dBodyID);
        sizeof (dGeomID);
        sizeof (dJointID);
        sizeof (dJointGroupID);
        
    }

//}

#endif