# /*
# SimpleScenes.cpp
# ---------------
# Code for the base scene class from which the specific scenes are derived
# */

# 
# base class 
# 
class SimpleScenes (OgreOde.CollisionListener,OgreOde.StepListener):
    KEY_DELAY = 1.0
    STEP_RATE = 0.01
    def __init__ ( self ):
        OgreOde.CollisionListener.__init__(self)
        OgreOde.StepListener.__init__(self)
        self._ragdollFactory = OgreOde_Prefab.RagdollFactory()
        Ogre.Root.getSingletonPtr().addMovableObjectFactory(self._ragdollFactory) 
        setInfoText("")
    
        self._world = OgreOde.World.getSingletonPtr()
        self._mgr = _world.getSceneManager()
        self._world.setCollisionListener(this)
        self._space = _world.getDefaultSpace()
        self._key_delay = KEY_DELAY
        self._last_node = 0
        self.RagdollList=[]
        self.bodies=[]  # an array to keep objects around in (like c++ "new" )

    # 
    # Called by OgreOde whenever a collision occurs, so 
    # that we can modify the contact parameters
    # 
    def collision( self, contact) :
        ## Check for collisions between things that are connected and ignore them
        g1 = contact.getFirstGeometry()
        g2 = contact.getSecondGeometry()
    
        if (g1 and g2):
            b1 = g1.getBody()
            b2 = g2.getBody()
            if (b1 and b2 and OgreOde.Joint.areConnected(b1, b2)) 
               return False 
    
        ## Set the friction at the contact
        contact.setCoulombFriction(OgreOde.Utility.Infinity)
    
        ## Yes, this collision is valid
        return True

# 
# Handle key presses
# 
    def frameEnded(self, time,keyinput):
        self._key_delay += time
        for d in self.RagdollList:
            d.update()  # ??

    # 
    # Utility method to set the information string in the UI
    # 
    def setInfoText(self,  text)
        Ogre.OverlayManager.getSingleton().getOverlayElement("OgreOdeDemos/Info").setCaption(String("Info: ") + text)

xmlNames = [
    "zombie",
    "zombie",
    "ninja",
    "robot"
    ]
    
meshNames = [
    "zombie_small.mesh",
    "zombie_small.mesh",
    "ninja.mesh",
    "robot.mesh"
    ]
meshAnimation = [
    "Walk1",
    "Walk1",
    "Walk",
    "Walk"
    ]
meshScale = [
    Ogre.Vector3(1.0, 1.0, 1.0),
    Ogre.Vector3(1.0, 1.0, 1.0),
    Ogre.Vector3(1.0, 1.0, 1.0),
    Ogre.Vector3(1.0, 1.0, 1.0)
    ]

static const String ragdollFile = [
    "zombie.ogreode",
    "zombie_auto.ogreode",
    "ninja.ogreode",
    "robot.ogreode"
    ]
sSelectedMesh = 0
_ragdoll_count = 0

# /*
# Create a ragdoll
# */
    def createRagDoll( self ):
        if (self._key_delay < KEY_DELAY): 
            return
     
        self._key_delay = 0.0
    
        params = Ogre.NameValuePairList()
    
        params["mesh"] = meshNames[sSelectedMesh]
    
        RagDollName = "zombie" + str(self._ragdoll_count)
        self._ragdoll_count += 1
        _ragdoll = self._mgr.createMovableObject(RagDollName, 
            OgreOde_Prefab.RagdollFactory.FACTORY_TYPE_NAME,
            params)
            
        _ragdoll.setCastShadows(True)
    
        _ragdoll_node = self._mgr.getRootSceneNode().createChildSceneNode(_ragdoll.getName() + "Node")
        _ragdoll_node.attachObject(_ragdoll)
    
        ##_ragdoll_node.yaw(Degree(rand() % 360))
        ##_ragdoll_node.pitch(Degree(rand() % 360))
    
    
        _ragdoll_node.setOrientation(Quaternion(Ogre.Radian(OgreOde.Utility.randomReal() * 10.0 - 5.0),
                                        Ogre.Vector3(OgreOde.Utility.randomReal() * 2.0 - 1.0,
                                        OgreOde.Utility.randomReal() * 2.0 - 1.0,
                                        OgreOde.Utility.randomReal() * 2.0 - 1.0)))
    
        _ragdoll_node.setPosition(Ogre.Vector3((OgreOde.Utility.randomReal() * 10.0) - 5.0,
                                    OgreOde.Utility.randomReal() + 5,
                                    (OgreOde.Utility.randomReal() * 10.0) - 5.0))
    
    
        _ragdoll_node.setScale( meshScale[sSelectedMesh])
    
    
        _ragdoll.getAnimationState(meshAnimation[sSelectedMesh]).setEnabled(False)
    
        if (ragdollFile[sSelectedMesh] != "" ):
            dotOgreOdeLoader.loadRagdoll(ragdollFile[sSelectedMesh], 
                                        _ragdoll,
                                        xmlNames[sSelectedMesh])
    
        ## Create the ragdoll
        _ragdoll.takePhysicalControl(_space, False)
        _ragdoll.setSelfCollisions(False)
    
        self.RagdollList.append(_ragdoll)

    # /*
    # Create a randomly sized box, sphere or capsule for dropping on things
    # */ 
    def self.createRandomObject(self, objectClass):
        if (self._key_delay < self..KEY_DELAY) 
            return 0
    
        if objectClass == OgreOde.Geometry.Class_Box:
            typeName = "crate"
        elif objectClass == OgreOde.Geometry.Class_Sphere:
            typeName = "ball" 
        elif objectClass == OgreOde.Geometry.Class_Capsule:
            typeName = "capsule" 
        elif objectClass == OgreOde.Geometry.Class_TriangleMesh:
            typeName = "gun" 
        else:
            typeName = "unknown" 
            
        ## Create the visual representation (the Ogre entity and scene node)
        name = typeName + str(_bodies.size())
        entity = self._mgr.createEntity(name, typeName + ".mesh")
        node = self._mgr.getRootSceneNode().createChildSceneNode(name)
        node.attachObject(entity)
        entity.setNormaliseNormals(True)
        entity.setCastShadows(True)
    
        ## Pick a size
        size = Ogre.Vector3((OgreOde.Utility.randomReal() * 0.5 + 0.1) * 2.0,
                    (OgreOde.Utility.randomReal() * 0.5 + 0.1) * 2.0,
                    (OgreOde.Utility.randomReal() * 0.5 + 0.1) * 2.0)
    
        ## Create a body associated with the node we created
        body = OgreOde.Body()
        node.attachObject(body)
    
        ## Set the mass and geometry to match the visual representation
        if objectClass == OgreOde.Geometry.Class_Box:
            size *= 1.75
    
            mass = OgreOde.BoxMass(1.0,size)
            mass.setDensity(5.0,size)
            
            geom = (OgreOde.Geometry*)new OgreOde.BoxGeometry(size,_space)
            self.bodies.append 9 
            node.setScale(size.x * 0.1,size.y * 0.1,size.z * 0.1)
            body.setMass(mass)
    
        elif objectClass == OgreOde.Geometry.Class_Sphere:
            mass= OgreOde.SphereMass(1.0,size.x)
            mass.setDensity(5.0,size.x)
    
            geom = OgreOde.SphereGeometry(size.x,_space)
            node.setScale(size.x * 0.2,size.x * 0.2,size.x * 0.2)
            body.setMass(mass)
    
        elif objectClass == OgreOde.Geometry.Class_Capsule:
                size.x *= 0.5
    
                mass = OgreOde.CapsuleMass(1.0,size.x,Vector3.UNIT_Z,size.y)
                mass.setDensity(5.0,size.x,Vector3.UNIT_Z,size.y)
                
                geom = OgreOde.CapsuleGeometry(size.x,size.y,_space)
                node.setScale(size.x,size.x,(size.y + (size.x * 2.0)) * 0.25)
                body.setMass(mass)
    
        elif objectClass == OgreOde.Geometry.Class_TriangleMesh:
                ei = OgreOde.EntityInformer (entity, node._getFullTransform())
                geom = ei.createStaticTriangleMesh(_space)
    
                mass = OgreOde.BoxMass (1.0, ei.getSize())
                mass.setDensity(5.0, ei.getSize())
    
                ##node.setScale(size.x,size.x,(size.y + (size.x * 2.0)) * 0.25)
                body.setMass(mass)
    
        ## Tie the collision geometry to the physical body
        geom.setBody(body)
    
        ## Keep track of the body
        self._bodies.append(body)
        self._geoms.append(geom)
        
        self._key_delay = 0.0
    
        ## If we created something position and orient it randomly
        if (body) :
            body.setOrientation(Ogre.Quaternion(Ogre.Radian(OgreOde.Utility.randomReal() * 10.0 - 5.0),
                                    Vector3(OgreOde.Utility.randomReal() * 2.0 - 1.0,
                                            OgreOde.Utility.randomReal() * 2.0 - 1.0,
                                            OgreOde.Utility.randomReal() * 2.0 - 1.0)))
            body.setPosition(Ogre.Vector3((OgreOde.Utility.randomReal() * 10.0) - 5.0,
                                        OgreOde.Utility.randomReal() + 5,
                                        (OgreOde.Utility.randomReal() * 10.0) - 5.0))
    
            ## Set the last body we created to be looked at
            _last_node = body.getParentNode()
    
        return body
    
    # 
    # Destructor, delete everything we're keeping track of
    # 
    def __del__ ( self ):
        ## Stop listening for collisions
        if (self._world.getCollisionListener() == self) 
            _world.setCollisionListener(0)
    
        ## Delete all the joints
        for i in self._joints:
            del (i)
        clearList = []
        ## Run through the list of bodies we're monitoring
        for i in self._bodies:
            ## Get the node this body controls
            node = i.getParentNode()
            if (node):
                ## Get its name and remember all the things attached to it
                name = node.getName()
                num = node.numAttachedObjects()
                for cur in range ( num ) :
                    obj = node.getAttachedObject(cur)
                    if (obj.getMovableType() != OgreOde.Body.MovableType) 
                        clearList.append(obj)
    
                ## Destroy the node by name
                self._mgr.getRootSceneNode().removeAndDestroyChild(name)
            ## Delete the body
            del (i)
    
        ## Remove all the entities we found attached to scene nodes we're controlling
        for i in clearList:
            if (i.getMovableType() == "Entity") :
                self._mgr.destroyMovableObject(i)
            elif (i.getMovableType() == "ParticleSystem")  :
                _mgr.destroyParticleSystem(i)
    
        ## Delete all the collision geometries
        for i in self._geoms:
            del (i)
        ## Remove all the entities we found attached to scene nodes we're controlling
        for i in self.RagdollList:
            assert ((*i).getParentNode ())
            assert ((*i).getParentNode ().getParent())
            i.getParentNode().getParent().removeAndDestroyChild(
                i.getParentNode().getName ())
    
            self._mgr.destroyMovableObject(i.getName(), OgreOde_Prefab.RagdollFactory.FACTORY_TYPE_NAME)
        Ogre.Root.getSingletonPtr().removeMovableObjectFactory(_ragdollFactory)
        del _ragdollFactory 
