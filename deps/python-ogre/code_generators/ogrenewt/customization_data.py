def aliases( Version ):
    return {
    "set<Ogre::SceneQuery::WorldFragmentType, std::less<Ogre::SceneQuery::WorldFragmentType>, std::allocator<Ogre::SceneQuery::WorldFragmentType> >"
    : "SetWorldFragment",
    "vector<OgreNewt::Collision*,std::allocator<OgreNewt::Collision*> >"
    : "VectorofCollision"

     }

def header_files( Version ):
    return [ 'ogre.h',
             'ogrenewt.h',
             #'return_pointee_value.hpp'

             ]

def huge_classes( Version ):
    return []

