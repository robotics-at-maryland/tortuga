# /*
# SimpleScenes_Chain.h
# ----------------------
# A reimplementation of the ODE spherical chain demo 
# using Ogre and the OgreOde wrapper.
# */

from SimpleScenes import *##.py
import Ogre as ogre
import OgreOde
import OIS

# /*
# The chain test extends the base test class
# */
class SimpleScenes_Chain (SimpleScenes):
    ## Standard constructor, creates everything in the demo
    def __init__ (self, world):
        SimpleScenes.__init__(self, world)
        ## Keep track of the top of the chain so we can throw it around
        self._first_body = None
        self._box_body = None
        self._force = 0
        self._force_to_apply = 0

        ## Set up the sizes of stuff
        link_size = 0.4
        geom_size = 0.4
        adjust = 0.1
        last_body = 0
        num_links = 10

        self._first_body = 0
        self._force = 1

        ## Create all the links in the chain
        for i in range (num_links, 0, -1 ):
            np = i * (link_size + adjust)
            name = "Sphere_" + str(i)

            ## Visuals
            entity = self._mgr.createEntity(name,"ball.mesh")
            entity.setNormaliseNormals(True)
            entity.setCastShadows(True)

            node = self._mgr.getRootSceneNode().createChildSceneNode(entity.getName())
            node.attachObject(entity)
            node.setScale(link_size * 0.2,link_size * 0.2,link_size * 0.2)
            node.setPosition(ogre.Vector3(np,np + 0.4,np))

            ## Physicals
            ei= OgreOde.EntityInformer (entity,ogre.Matrix4.getScale(node.getScale()))
            body = ei.createSingleDynamicSphere(1.0,self._world, self._space)
            self._bodies.append(body)
            self._geoms.append(body.getGeometry(0))

            ## Join the current body to the last one (if there was a last one)
            if not self._first_body:
                self._last_node = node
                self._first_body = body
            if last_body:
                joint = OgreOde.BallJoint(self._world)
                joint.attach(body,last_body)
                ja = (i + 0.5) * (link_size + adjust)
                joint.setAnchor(ogre.Vector3(ja,ja + 0.4,ja))

                self._joints.append(joint)

            last_body = body

        ## Create some static boxes to bump into
        self.createBox(0,2,ogre.Vector3(4,1,4))
        self.createBox(1,2,ogre.Vector3(-4,1,4))
        self.createBox(2,2,ogre.Vector3(-4,1,-4))
        self.createBox(3,2,ogre.Vector3(4,1,-4))

    def frameEnded(self, time, input,  mouse):
        SimpleScenes.frameEnded(self, time, input, mouse)
        self._force_to_apply = ogre.Vector3.ZERO

        right = self._mgr.getCamera("PlayerCam").getRight()
        forward = right.crossProduct(ogre.Vector3.UNIT_Y)
    
        ## Up
        if (input.isKeyDown(OIS.KC_X)): self._force_to_apply += ogre.Vector3.UNIT_Y * self._force

        ## Left/right
        elif (input.isKeyDown(OIS.KC_J)): self._force_to_apply -= right * self._force
        elif (input.isKeyDown(OIS.KC_L)): self._force_to_apply += right * self._force

        ## Forward/back
        elif (input.isKeyDown(OIS.KC_K)): self._force_to_apply += forward * self._force
        elif (input.isKeyDown(OIS.KC_I)): self._force_to_apply -= forward * self._force
        else: self._force_to_apply = 0

        self._mgr.getCamera("PlayerCam")

    ## Apply the forces before every time step
    def addForcesAndTorques( self ):
        ## Apply the force we calculated in the key handler
        self._first_body.addForce(self._force_to_apply)
    
    ## Return our name for the test application to display
    def getName(self):
        return "Test Chain"

    ## Tell the user what keys they can use
    def getKeys( self ):
        return "J/L, I/K, X - Throw the chain around"

    ## Override the collision callback to set our own parameters
    def collision(self, contact):
        ## Set the floor to be a bit slippy
        contact.setCoulombFriction(10.0)
        return True

    ## Use the destructor to delete the crate scene nodes
    ## everything else gets deleted automatically
    def __del__ ( self ):
        for i in range ( 4 ):
            if (i != 2):
                name = "Crate_" + str(i)
                self._mgr.destroySceneNode(name)
                self._mgr.destroyEntity(name)
        SimpleScenes.__del__(self)
                

    ## Utility method to create a static box
    def createBox(self, id,size, position):
        ## Visual
        name = "Crate_" + str(id)

        entity = self._mgr.createEntity(name,"crate.mesh")
        entity.setNormaliseNormals(True)
        entity.setCastShadows(True)

        node = self._mgr.getRootSceneNode().createChildSceneNode(entity.getName())
        node.attachObject(entity)
        node.setScale(size * 0.1,size * 0.1,size * 0.1)
        node.setPosition(position)

        ## Make one of them dynamic, the others are static
        ei= OgreOde.EntityInformer (entity,ogre.Matrix4.getScale(node.getScale()))
        if ((position.x < 0)and(position.z < 0)):
            self._box_body = ei.createSingleDynamicBox(2.0, self._world, self._space) 
            self._bodies.append(self._box_body)
            self._geoms.append(self._box_body.getGeometry(0))
        else:
            ## Collision geometry
            self._geoms.append(ei.createSingleStaticBox(self._world, self._space))

