
typedef std::map<std::string, std::string, std::less<std::string>, std::allocator<std::pair<std::string const, std::string> > > \
        MapOfStringPair;
typedef std::list<Ogre::Plane, std::allocator<Ogre::Plane> > ListOfPlane;
typedef OgreOde::CircularBuffer<OgreOde::BodyState*> CircularBufferBodyStatePtr;
typedef OgreOde::MaintainedList<OgreOde::Geometry> MaintainedListGeometry;
//typedef OgreOde::MaintainedListIterator<OgreOde::Geometry> MaintainedListIteratorGeometry;
typedef OgreOde::MaintainedList<OgreOde::Body> MaintainedListBody;
//typedef OgreOde::MaintainedListIterator<OgreOde::Body> MaintainedListIteratorBody;
typedef OgreOde::MaintainedList<OgreOde::Space> MaintainedListSpace;
//typedef OgreOde::MaintainedListIterator<OgreOde::Space> MaintainedListIteratorSpace;
typedef OgreOde::MaintainedList<OgreOde::JointGroup> MaintainedListJointGroup;
//typedef OgreOde::MaintainedListIterator<OgreOde::JointGroup> MaintainedListIteratorJointGroup;
typedef OgreOde::MaintainedList<OgreOde::Joint> MaintainedListJoint;
//typedef OgreOde::MaintainedListIterator<OgreOde::Joint> MaintainedListIteratorJoint;


typedef std::vector<std::string> VectorString;
typedef std::vector<unsigned short> VectorUnsignedShort;
typedef std::set<Ogre::Entity*> SetEntityPtr;
typedef std::list<OgreOde::Geometry*>   GeometryList;
typedef std::vector<OgreOde::Geometry*> GeometryArray;
typedef std::vector<Ogre::Vector3> Vector3Array;
typedef std::vector<Ogre::Vector4> Vector4Array;
typedef std::map<unsigned char,OgreOde::Vector3Array* > BoneMapping;
typedef std::pair<unsigned short,OgreOde::Vector3Array* > BoneMappingKey;
typedef unsigned long MaterialID;
typedef std::map<OgreOde::MaterialID,OgreOde::Contact*> MaterialMap;
typedef std::pair<OgreOde::MaterialID,OgreOde::MaterialMap* > MaterialMapPair;

typedef std::vector<OgreOde_Prefab::Ragdoll::PhysicalBone*> RagdollPhysicalBoneList;
typedef std::list<OgreOde_Prefab::Object  *>   ObjectList;



        
         
         