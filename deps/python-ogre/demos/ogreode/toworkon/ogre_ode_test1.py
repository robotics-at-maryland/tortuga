###############################
# pyOgre pyODE test
# Version 1.0
# 2006 - Dave Webster
# http://www.dave-webster.com
###############################
# Based around the tutorials from:
# http://www.ogre3d.org/
# and
# http://pyode.sourceforge.net/
###############################

import Ogre as ogre
import SampleFramework as sf
import ODE as ode, random
from math import *

class TutorialApplication(sf.Application):
    def __init__(self):
        sf.Application.__init__(self)

        # Set up the physics world
        self.world = ode.dWorld()
        self.world.setGravity(0, -9.81, 0)
        ssid = ode.dSimpleSpace()
        
        self.space = ode.dSimpleSpace(1) # dQuadTreeSpace, dHashSpace
        self.contactgroup = ode.JointGroup()
        self.bodyList = []
    #end __init__



    def _createScene(self):


        ############
        ## Ogre Scene Stuff
        ############

        # Set up the lighting conditions
        self.sceneManager.ambientLight = (0.1, 0.1, 0.1)
        self.sceneManager.shadowTechnique = ogre.SHADOWTYPE_STENCIL_ADDITIVE

        # create a spotlight
        light = self.sceneManager.createLight("Light2")
        light.type = ogre.Light.LT_SPOTLIGHT
        light.diffuseColour = (1, 0, 0)
        light.specularColour = (1, 0, 0)
        light.direction = (-1, -1, 0)
        light.position = (300, 300, 0)
        light.setSpotlightRange(ogre.Degree(10), ogre.Degree(90))
        
        # add a skybox
        self.sceneManager.setSkyBox(True, "Examples/SpaceSkyBox")


        ############
        ## Ground
        ############
        plane = ogre.Plane((0, 1, 0), 0)
        self.floor = ode.GeomPlane(self.space, (0,1,0), 0.0)

        mm = ogre.MeshManager.getSingleton()
        mm.createPlane('ground', ogre.ResourceManager.DEFAULT_RESOURCE_GROUP_NAME,
                       plane, 1500, 1500, 20, 20, True, 1, 5, 5, (0, 0, 1))

        ent = self.sceneManager.createEntity("GroundEntity", "ground")
        self.sceneManager.rootSceneNode.createChildSceneNode().attachObject(ent)

        ent.setMaterialName("Examples/Rockwall")
        ent.castShadows = False

        ############
        ## Set up the Bodies
        ############

        # Ogre ball 1
        ent1 = self.sceneManager.createEntity("ball1", "ogrehead.mesh")
        node1 = self.sceneManager.rootSceneNode.createChildSceneNode()
        node1.attachObject(ent1)
        node1.scale = ((1.8, 1.8, 1.8))

        # ODE ball 1
        body1 = ode.Body(self.world)
        M = ode.Mass()
        M.setSphere(500, 0.05)
        body1.setMass(M)
        body1.setPosition((10, 100, 0))
        geom = ode.GeomSphere(self.space, 50.0)
        geom.setBody(body1)

        # Add ball 1 to the body list
        self.bodyList.append((body1, node1))

        #

        # Ogre ball 2
        ent1 = self.sceneManager.createEntity("ball2", "ogrehead.mesh")
        node1 = self.sceneManager.rootSceneNode.createChildSceneNode()
        node1.attachObject(ent1)
        node1.scale = ((1.8, 1.8, 1.8))

        # ODE ball 2
        body1 = ode.Body(self.world)
        M = ode.Mass()
        M.setSphere(500, 0.05)
        body1.setMass(M)
        body1.setPosition((0, 1000, 0))
        geom = ode.GeomSphere(self.space, 50.0)
        geom.setBody(body1)

        # Add ball 2 to the body list
        self.bodyList.append((body1, node1))

        # make sure the camera track this node
        # self.camera.setAutoTracking(True, node1)


        #

        # Ogre ball 3
        ent1 = self.sceneManager.createEntity("ball3", "ogrehead.mesh")
        node1 = self.sceneManager.rootSceneNode.createChildSceneNode()
        node1.attachObject(ent1)
        node1.scale = ((1.8, 1.8, 1.8))

        # ODE ball 3
        body1 = ode.Body(self.world)
        M = ode.Mass()
        M.setSphere(3000, 0.05)
        body1.setMass(M)
        body1.setPosition((300, 100, 0))
        geom = ode.GeomSphere(self.space, 50.0)
        geom.setBody(body1)

        # Add ball 3 to the body list
        self.bodyList.append((body1, node1))

        #

        # Ogre ball 4
        ent1 = self.sceneManager.createEntity("ball4", "ogrehead.mesh")
        node1 = self.sceneManager.rootSceneNode.createChildSceneNode()
        node1.attachObject(ent1)
        node1.scale = ((1.8, 1.8, 1.8))

        # ODE ball 4
        body1 = ode.Body(self.world)
        M = ode.Mass()
        M.setSphere(3000, 0.05)
        body1.setMass(M)
        body1.setPosition((-300, 100, 0))
        geom = ode.GeomSphere(self.space, 50.0)
        geom.setBody(body1)

        # Add ball 4 to the body list
        self.bodyList.append((body1, node1))

    #end _createScene

    
    def _createCamera(self):
        self.camera = self.sceneManager.createCamera("PlayerCam")
        self.camera.position = (0, 50, 800)
        self.camera.lookAt((0, 0, 0))

        self.camera.nearClipDistance = 5
    #end _createCamera

    
    def _createViewports(self):
        vp = self.renderWindow.addViewport(self.camera)
        vp.backgroundColour = (0, 0, 0)
        
        self.camera.aspectRatio = vp.actualWidth / vp.actualHeight
    #end _createViewports

    def _createFrameListener(self):
        #Create the FrameListener.
        self.frameListener = PhysicsFrameListener(self.renderWindow, self.camera, self.world, self.bodyList, self.space, self.contactgroup)
        self.frameListener.showDebugOverlay(True)
        self.root.addFrameListener(self.frameListener)
    #end _createFrameListener


#end TutorialApplication


class PhysicsFrameListener(sf.FrameListener):
        def __init__(self, renderWindow, camera, world, bodyList, space, contactgroup):
            sf.FrameListener.__init__(self, renderWindow, camera)

            self.world = world
            self.bodyList = bodyList
            self.time = 0.0
            self.dt = 1.0/60.0

            self.space = space
            self.contactgroup = contactgroup

        #end __init__


        def near_callback(self, args, geom1, geom2):

            # Check if the objects do collide
            contacts = ode.collide(geom1, geom2)

            # Create contact joints
            self.world,self.contactgroup = args
            for c in contacts:
                c.setMode(ode.ContactBounce)
                c.setBounce(0.2)
                c.setBounceVel(0.15)
                c.setMu(250.0)
                j = ode.ContactJoint(self.world, self.contactgroup, c)
                j.attach(geom1.getBody(), geom2.getBody())

        #end near_callback


        def frameStarted(self, frameEvent):

            self.space.collide((self.world, self.contactgroup), self.near_callback)

            # Step the physics
            self.world.step(self.dt)
            self.time += self.dt

            self.contactgroup.empty()

            # Update the positions of objects
            for body, node in self.bodyList:

                pos = body.getPosition()
                node.position = ogre.Vector3(*pos)
                Q = body.getQuaternion()
                node.orientation = ogre.Quaternion(*Q)

            return sf.FrameListener.frameStarted(self, frameEvent)

        #end frameStarted


#end PhysicsFrameListener


if __name__ == "__main__":
    app = TutorialApplication()
    app.go()
