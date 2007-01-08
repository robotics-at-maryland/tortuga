def aliases( Version ):
    return {
    "map<std::string, std::string, std::less<std::string>, std::allocator<std::pair<std::string const, std::string> > >"
        : "MapOfStringPair"
    ,"list<Ogre::Plane, std::allocator<Ogre::Plane> >"
        : "ListOfPlane"
    ,"CircularBuffer<OgreOde::BodyState*>"
        : "CircularBufferBodyStatePtr"
    ,"MaintainedList<OgreOde::Geometry>"
        : "MaintainedListGeometry"
    ,"MaintainedList<OgreOde::Body>"
        : "MaintainedListBody"
    ,"MaintainedList<OgreOde::Space>"
        : "MaintainedListSpace"
    ,"MaintainedList<OgreOde::JointGroup>"
        : "MaintainedListJointGroup"
    , "MaintainedList<OgreOde::Joint>"
        : "MaintainedListJoint"
        

         }

def header_files( Version ):
    return [ 
    "OgreOde_Core.h",
    "OgreOde_Prefab.h"
             ]

def huge_classes( Version ):
    return []

