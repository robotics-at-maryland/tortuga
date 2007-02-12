# /*
# SimpleScenes_Joints.h
# ---------------------
# A demo using Ogre and the OgreOde wrapper to demonstrate joints.
# */
from SimpleScenes import *##.py
import Ogre as ogre
import OgreOde
import OIS
# /*
# The joints test extends the base test class
# */
class SimpleScenes_Joints( SimpleScenes ):

    ## Create the scene, just a load of differently hinged doors
    def __init__ (self, world):
        SimpleScenes.__init__(self,world)
        
        self.left = self.right = self.up = False

        ## Create the player's ball
        entity = self._mgr.createEntity("Ball","ball.mesh")
        entity.setNormaliseNormals(True)
        entity.setCastShadows(True)

        node = self._mgr.getRootSceneNode().createChildSceneNode(entity.getName())

        node.attachObject(entity)
        node.setScale(0.2,0.2,0.2)
        node.setPosition(0,1.2,0)

        ei = OgreOde.EntityInformer (entity,ogre.Matrix4.getScale(node.getScale()))
        self.ball_body = ei.createSingleDynamicSphere(5.0,self._world, self._space)
        self._last_node = self.ball_body.getParentNode()
        self._bodies.append(self.ball_body)
        self._geoms.append(self.ball_body.getGeometry(0))

        ## Make the ball stay in the X/Y plane  ## AJM NEED TO CHECK
        joint2d = OgreOde.PlanarJoint(self._world)
        joint2d.attach(self.ball_body)
        self._joints.append(joint2d)

        ## Create the free standing door
        entity = self._mgr.createEntity("Free_Door","door.mesh")
        entity.setNormaliseNormals(True)
        entity.setCastShadows(True)

        node = self._mgr.getRootSceneNode().createChildSceneNode(entity.getName())

        node.attachObject(entity)
        node.setPosition(15,4,0)
        node.setOrientation(ogre.Quaternion(ogre.Degree(90),ogre.Vector3(0,1,0)))
        node.setScale(2,2,2)

        ei1 =  OgreOde.EntityInformer (entity,ogre.Matrix4.getScale(node.getScale()))
        body = ei1.createSingleDynamicBox(20.0, self._world, self._space)
        self._bodies.append(body)
        self._geoms.append(body.getGeometry(0))

        ## Create a door hinged at the self.left hand side
        entity = self._mgr.createEntity("Left_Door","door.mesh")
        entity.setNormaliseNormals(True)
        entity.setCastShadows(True)

        node = self._mgr.getRootSceneNode().createChildSceneNode(entity.getName())

        node.attachObject(entity)
        node.setPosition(-10,4.01,1.5)
        node.setOrientation(ogre.Quaternion(ogre.Degree(-90),ogre.Vector3(0,1,0)))
        node.setScale(2,2,2)

        ei2 = OgreOde.EntityInformer (entity,ogre.Matrix4.getScale(node.getScale()))
        body = ei2.createSingleDynamicBox(20.0,self._world, self._space)
        body.setDamping(0,20)

        joint = OgreOde.HingeJoint(self._world)
        joint.attach(body)
        joint.setAxis(ogre.Vector3.UNIT_Y)
        joint.setAnchor(ogre.Vector3(-10,4.01,3.5))

        self._bodies.append(body)
        self._geoms.append(body.getGeometry(0))
        self._joints.append(joint)

        ## Create a door hinged at the self.right hand side
        entity = self._mgr.createEntity("Right_Door","door.mesh")
        entity.setNormaliseNormals(True)
        entity.setCastShadows(True)

        node = self._mgr.getRootSceneNode().createChildSceneNode(entity.getName())

        node.attachObject(entity)
        node.setPosition(-15,4.01,-1.5)
        node.setOrientation(ogre.Quaternion(ogre.Degree(90),ogre.Vector3(0,1,0)))
        node.setScale(2,2,2)

        ei3 = OgreOde.EntityInformer (entity,ogre.Matrix4.getScale(node.getScale()))
        body = ei3.createSingleDynamicBox(20.0,self._world, self._space)
        body.setDamping(0,20)

        joint = OgreOde.HingeJoint(self._world)
        joint.attach(body)
        joint.setAxis(ogre.Vector3.UNIT_Y)
        joint.setAnchor(ogre.Vector3(-15,4.01,-3.5))

        self._bodies.append(body)
        self._geoms.append(body.getGeometry(0))
        self._joints.append(joint)

        ## Create a door hinged at the top
        entity = self._mgr.createEntity("Top_Door","door.mesh")
        entity.setNormaliseNormals(True)
        entity.setCastShadows(True)

        node = self._mgr.getRootSceneNode().createChildSceneNode(entity.getName())

        node.attachObject(entity)
        node.setPosition(10,4.1,0)
        node.setOrientation(ogre.Quaternion(ogre.Degree(90),ogre.Vector3(0,1,0)))
        node.setScale(2,2,2)

        ei4 = OgreOde.EntityInformer (entity,ogre.Matrix4.getScale(node.getScale()))
        body = ei4.createSingleDynamicBox(20.0,self._world, self._space)
        body.setDamping(0,20)

        joint = OgreOde.HingeJoint(self._world)
        joint.attach(body)
        joint.setAxis(ogre.Vector3.UNIT_Z)
        joint.setAnchor(ogre.Vector3(10,8.1,0))

        self._bodies.append(body)
        self._geoms.append(body.getGeometry(0))
        self._joints.append(joint)
    
    ## Standard destructor
    def __del__ ( self ):
        SimpleScenes.__del__(self)
       
        
    ## Return our name for the test application to display
    def getName( self ):
        return "Test Joints"

    ## Return a description of the keys that the user can use in this test
    def getKeys( self ):
        return "I - Bounce, J - Left, L - Right"

    ## Gets called every time the world is stepped, so we can apply forces every time step
    def addForcesAndTorques( self ):
        ## Apply a torque about the Z axis to roll the ball self.left 
        if(self.left):
            self.ball_body.wake()
            self.ball_body.addTorque(ogre.Vector3(0,0,50))

        ## Apply a torque about the Z axis to roll the ball self.right
        if(self.right):
            self.ball_body.wake()
            self.ball_body.addTorque(ogre.Vector3(0,0,-50))

        ## Apply an self.upwards force, or at least opposite to gravity
        if(self.up):
            self.ball_body.wake()
            self.ball_body.addForce(self._world.getGravity() * -10.0)


    def frameEnded(self, time, input,  mouse):
        ## Do default processing
        SimpleScenes.frameEnded(self, time, input, mouse)

        ## Set the key flags so we can apply forces at the self.right time
        self.left = input.isKeyDown(OIS.KC_J)
        self.right = input.isKeyDown(OIS.KC_L)
        self.up = input.isKeyDown(OIS.KC_I)
    
    ## Set self.up the collision properties
    def collision(self, contact):
        contact.setBouncyness(0.8)
        contact.setCoulombFriction(999999999999) ## OgreOde.Utility.Infinity)
        contact.setFrictionMode(OgreOde.Contact.Flag_FrictionPyramid)

        return True
