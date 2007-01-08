#ifndef __python_ogrenewt_h_01
#define __python_ogrenewt_h_01

//See best practices section in Py++ documentation

#include <vector>
#include <string>
#include <map>
#include "ogrenewt.h"

//namespace OgreNewt{ 

    inline void instantiate(){
        //sizeof ( EventCallback);
//        sizeof ( OgreNewt::NewtonHingeSliderUpdateDescTag );
        sizeof ( NewtonJoint);
        sizeof ( NewtonBody );
        sizeof ( NewtonCollision );
        sizeof ( NewtonWorld );
        
/*        sizeof ( Ogre::Radian );
        sizeof ( Ogre::Node );
        sizeof ( Ogre::Vector3);
        sizeof ( Ogre::Quaternion ); */
                
    }

//}

#endif