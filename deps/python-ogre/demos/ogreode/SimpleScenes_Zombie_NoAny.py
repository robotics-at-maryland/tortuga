# /*
# SimpleScenes_RagDoll.h
# ---------------------
# A demo RagDoll shooting demo, showing ragdolls and ray intersections.
# */
# This version differece from the C++ deom code in that it doesn't take advantage of UserData due to a Python-Ogre 0.8
# limitation (can't pass a Python object to Ogre.Any() - hence can't save the data with the node
#
# However, as it's "Python" we can keep the info in a dictionary and use the object name as a key :)
#



from SimpleScenes import *
import Ogre as ogre
import OgreOde
import OIS
import random

ZOMBIE_QUERY_MASK               = 1<<1

# /*
# The zombie demo extends the base test class
# */
class SimpleScenes_Zombie ( SimpleScenes ):
 
    class ragDollData:
        def __init__(self):
            self.sSelectedMesh = 0
            self.timeSinceBirth=0
            self.timeSincePhysical=0
            
    def getUserAny ( self, ragdoll ):
        name = ragdoll.getName()
        return self.UserData[name]
        
    def setUserAny ( self, ragdoll, data ):
        name = ragdoll.getName()
        self.UserData[name] = data
      
    def createRagDoll ( self ):
        global ZOMBIE_QUERY_MASK
        self.xmlNames = [ "zombie", "zombie", "ninja", "robot" ]
        self.meshNames = ["zombie_small.mesh","zombie_small.mesh","ninja.mesh","robot.mesh"]
        self.meshAnimation = ["Walk1","Walk1","Walk","Walk"]
        self.meshScale = [  ogre.Vector3(1.0, 1.0, 1.0), ogre.Vector3(1.0, 1.0, 1.0),
                        ogre.Vector3(0.02, 0.02, 0.02), ogre.Vector3(0.05, 0.05, 0.05) ]
        self.ragdollFile = ["zombie.ogreode","zombie_auto.ogreode","ninja.ogreode","robot.ogreode"]
        self.sMaxRagdoll = 30
        self._last_ragdoll = 0.0
    
        
        self.myRagDolls = []
        self.UserData={}
        
        ragdoll = self.dotOgreOdeLoader.loadObject(self.ragdollFile[self.sSelectedMesh],
                                                         self.xmlNames[self.sSelectedMesh],
                                                         "zombie" + str(self._ragdoll_count))
        self._ragdoll_count += 1

        ragdoll.setCastShadows(True)       
        ragdoll.setQueryFlags (ZOMBIE_QUERY_MASK)

        try:
            a = self.getUserAny ( ragdoll )
        except:
            # must be "empty"
            b = self.ragDollData ()

            b.timeSinceBirth = 0.0
            b.timeSincePhysical = 0.0
            b.sSelectedMesh = self.sSelectedMesh
            self.setUserAny ( ragdoll, b )

        ## Add entity to the scene node
        ragdoll_node = self._mgr.getRootSceneNode().createChildSceneNode(ragdoll.getName() + "Node")
        ragdoll_node.attachObject(ragdoll)
        ragdoll_node.yaw(ogre.Degree(random.random() % 360))
        ragdoll_node.setPosition(0.0,0.0,0)
        ragdoll_node.setScale( self.meshScale[self.sSelectedMesh])

        self._last_node = ragdoll_node

        
        ragdoll.getAnimationState(self.meshAnimation[self.sSelectedMesh]).setEnabled(True)

        self.myRagDolls.append (ragdoll)


        ##self.sSelectedMesh = (self.sSelectedMesh + 1) % 4

    ## Constructor
    def __init__( self, world):
        global ZOMBIE_QUERY_MASK   
        SimpleScenes.__init__(self, world)

        self._over = ogre.OverlayManager.getSingleton().getByName("OgreOdeDemos/Target")    
        self._over.show()

        self._gun = self._mgr.createEntity("gun","gun.mesh")
        self._gun.setCastShadows(False)

        self._gun_node = self._mgr.getRootSceneNode().createChildSceneNode("gun")
        self._gun_node.attachObject(self._gun)

        self._camera = self._mgr.getCamera("PlayerCam")

        self._animation_speed = 1.0
        self._ragdoll_count = 0
        self.createRagDoll()

        self._shot_time = 0.0

        self.myOdeRay = OgreOde.RayGeometry(1000.0, self._world, self._space)
        self.mRayQuery = self._mgr.createRayQuery(ogre.Ray())
        self.mRayQuery.setQueryMask (ZOMBIE_QUERY_MASK)
        self.mRayQuery.setQueryTypeMask(ogre.SceneManager.ENTITY_TYPE_MASK)
    
    ## Destructor
    def __del__ ( self ):
        for  zE in self.myRagDolls:
            assert (zE.getParentNode ())
            assert (zE.getParentNode ().getParent())
# This piece of code just doesn't work/convert so I resort to a simplistic fix..            
#             (static_cast<SceneNode*> ((*zE).getParentNode ().getParent())).removeAndDestroyChild(
#                 (*zE).getParentNode ().getName ())
            self._mgr.getRootSceneNode().removeAndDestroyChild( zE.getParentNode().getName() )

            self._mgr.destroyMovableObject(zE.getName(), OgreOde.RagdollFactory.FACTORY_TYPE_NAME)
            
        del self.myOdeRay

        self._over.hide()
        self._mgr.destroySceneNode("gun")
        self._mgr.destroyEntity("gun")
        self._mgr.destroyQuery (self.mRayQuery)
        SimpleScenes.__del__( self )

    ## Return our name for the test application to display
    def getName( self ):
        return "RagDoll Shooting Gallery"

    ## Return a description of the keys that the user can use in this test
    def getKeys( self ):
        return "X - Shoot"

   
    def frameStarted(self, time, input,  mouse):
        self._last_ragdoll += time
        SimpleScenes.frameEnded(self, time, input, mouse)

        self._gun_node.setOrientation(self._camera.getOrientation())
        self._gun_node.setPosition(self._camera.getPosition() + \
                                (self._camera.getOrientation() * ogre.Vector3(0.3,-0.15,-1.1)))

        for  zE in self.myRagDolls :
            b = self.getUserAny( zE )
            if zE.isPhysical():
                if  not zE.isStatic():
                    b.timeSincePhysical += time
                    zE.update()   ## RagDoll Activated
            else:
                zE.getAnimationState(self.meshAnimation[b.sSelectedMesh]).addTime(time * self._animation_speed)

                zE.getParentNode ().translate(
                    zE.getParentNode ().getOrientation() * (Vector3.UNIT_Z * time * 2.5))
 

    def frameEnded(self, time, input, mouse):
        ## Do default processing
        SimpleScenes.frameEnded(self,time, input, mouse)
        self._shot_time -= time

        if (self._key_delay > 0 and input.isKeyDown(OIS.KC_C)):
            for zE in self.myRagDolls:
                if zE.isPhysical():
                    zE.releasePhysicalControl()
                    b = self.getUserAny( zE )
                    zE.getAnimationState(self.meshAnimation[b.sSelectedMesh]).setEnabled(True)
            self._key_delay = SimpleScenes.KEY_DELAY

        if (self._shot_time <= 0.0 and \
                        ( input.isKeyDown(OIS.KC_X) or mouse.getMouseState().buttonDown(OIS.MB_Left))):

            pickRay = self._camera.getCameraToViewportRay(0.5, 0.5)
            self.myOdeRay.setDefinition(pickRay.getOrigin(), pickRay.getDirection())

            self._shot_time = 0.2

            self.mRayQuery.setRay (pickRay)
            result = self.mRayQuery.execute()
            if len(result) > 0:
                ##self.mRayQuery.setSortByDistance (True, 1)##only one hit
                self.myOdeRay.enable ()
                for i in result:
                    self._last_ragdoll = 5.0

                    radgoll = i.movable

                   ## uncomment to see nice debug mesh of animated mesh
                   
#                   ## ray cast could be tested against that instead of ragdoll.
#                   ## depending on complexity of mesh could be simpler
#                   ei= OgreOde.EntityInformer(zE, zE.getParentNode ()._getFullTransform())
#                   self._geoms.append(ei.createStaticTriangleMesh(self._space))

                    wasPhysical = radgoll.isPhysical ()
                    if ( not wasPhysical):
                        b = self.getUserAny( radgoll )
                        radgoll.getAnimationState(self.meshAnimation[b.sSelectedMesh]).setEnabled(False)
                        
                        ## Create the ragdoll
                        radgoll.takePhysicalControl(self._world, self._space, False)
                        radgoll.setSelfCollisions(False)

                    hit_body = OgreOde.Body(self._world)
                    hit_point = ogre.Vector3()

                    is_hit = False
                    if (radgoll.pick(self.myOdeRay, hit_body, hit_point)):
                        if (hit_body):
                            hit_body.addForceAt(pickRay.getDirection() * 250000, hit_point)
                            is_hit = True
                            b = self.getUserAny( radgoll )
                            b.timeSincePhysical = 0.0

                    if ( not is_hit and not wasPhysical):
                        radgoll.releasePhysicalControl()
                        b = self.getUserAny( radgoll )
                        radgoll.getAnimationState(self.meshAnimation[b.sSelectedMesh]).setEnabled(True)
            self.myOdeRay.disable ()
        x = 0
        while (x+1) <= len ( self.myRagDolls ):
            radgoll = self.myRagDolls[x]
            b = self.getUserAny( radgoll )
            b.timeSinceBirth += time
    
            ## turn to stone to improve fps,
            ## better way to do that would be to tweak
            ## simulation parameters to be less jitty.
            ## better auto sleep
            if (radgoll.isPhysical() and  not radgoll.isStatic() \
                and radgoll.isAwake() and b.timeSincePhysical > 5.0):
                    ##radgoll.turnToStone()
                    radgoll.sleep()
            
            if ( b.timeSinceBirth > self.sMaxRagdoll):
# # #               (static_cast<SceneNode*> (radgoll.getParentNode ().getParent())).removeAndDestroyChild(
# # #                   radgoll.getParentNode ().getName ())
                self._mgr.getRootSceneNode().removeAndDestroyChild( radgoll.getParentNode().getName() )
                self._mgr.destroyMovableObject(radgoll.getName(), OgreOde.RagdollFactory.FACTORY_TYPE_NAME)
                self.myRagDolls.remove(x)
            else:
                x += 1
        if self._last_ragdoll > 5.0 and \
            self.myRagDolls.size() < self.sMaxRagdoll :
            self.createRagDoll()
