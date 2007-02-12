# /*
# SimpleScenesApplication.cpp
# --------------------------
# The main applicatin that handles switching between the
# different scenes in this demo, as well as any common 
# setup and input handling.
# */

## The tests we can display
#import SimpleScenes_BoxStack.py
#include "SimpleScenes_Buggy.h"
#include "SimpleScenes_Chain.h"
#include "SimpleScenes_TriMesh.h"
#include "SimpleScenes_Crash.h"
#include "SimpleScenes_Joints.h"
#include "SimpleScenes_Zombie.h"
import  Ogre as ogre
import OgreOde
import OIS
import SampleFramework as sf
import sys

from SimpleScenes_BoxStack import *
from SimpleScenes_TriMesh import *
from SimpleScenes_Chain import *
from SimpleScenes_Joints import *
from SimpleScenes_Buggy import *
from SimpleScenes_Zombie_NoAny import *


STEP_RATE=0.01
ANY_QUERY_MASK                  = 1<<0
ZOMBIE_QUERY_MASK               = 1<<1
GEOMETRY_QUERY_MASK             = 1<<2
VEHICLE_QUERY_MASK              = 1<<3
STATIC_GEOMETRY_QUERY_MASK      = 1<<4
## We need a frame listener class
        
class SimpleScenesFrameListener ( sf.FrameListener ):
    def __init__( self, demo, renderWindow, camera ):
        sf.FrameListener.__init__(self, renderWindow, camera)
        self._demo = demo
    def __del__(self):
        print "del11"
                
    def frameStarted(self, evt):
        ## Do the default demo input handling and keep our UI display
        ## in sync with the other stuff (frame rate, logo, etc)
        show = self.statisticsOn
        bOK = sf.FrameListener.frameStarted(self, evt)
        if (self.statisticsOn != show):
            pOver = ogre.OverlayManager.getSingleton().getByName("OgreOdeDemos/Overlay")    
            if (pOver):
                if (self.statisticsOn) :
                    pOver.show()
                else :
                    pOver.hide()
                    
        ## Tell the demo application that it needs to handle input
        self._demo.frameStarted(evt, self.Keyboard, self.Mouse)  #we pass the keyboard etc to the main app framelistener
    
        ## Quit or carry on according to the normal demo input
        return bOK
    
    # 
    # The frame listener is informed after every frame
    # 
    def frameEnded(self, evt):
        ## Tell our demo that the frame's ended before doing default processing
        self._demo.frameEnded(evt, self.Keyboard, self.Mouse)
        return sf.FrameListener.frameEnded(self, evt)

# /*
# Create the scene from an ogre point of view
# and create the common OgreOde things we'll need
# */
class SimpleScenesApplication(sf.Application):
    def __init__ ( self ):
        sf.Application.__init__(self)
        self._test = 0
        self._plane = 0
        self._stepper = 0   
        self._world = 0
        self._delay=0.0
        self._spot=None
        self._time_elapsed = 0.0
        self._time_step = 0.1 ## SimpleScenes::STEP_RATE
        self._looking = _chasing = False
        self._paused = False

#     def _setUpResources( self ):
#         sf.Application._setUpResources(self) 
#         rsm = ogre.ResourceGroupManager.getSingletonPtr()
#         groups = rsm.getResourceGroups()        
#         ##if (std::find(groups.begin(), groups.end(), String("OgreOde")) == groups.end())
#         rsm.createResourceGroup("OgreOde")
#         rsm.addResourceLocation("../Media/OgreOde","FileSystem", "OgreOde")
        
    def __del__ ( self ):
        print "in __del__"
        del self._test
        del self._plane
        del self._stepper
        del self._world

        sf.Application.__del__( self )
        print "del2"
        del( self.frameListener )
        print "del3"
        
    def _createScene(self):
        global STEP_RATE, ANY_QUERY_MASK, STATIC_GEOMETRY_QUERY_MASK
        sceneManager = self.sceneManager
        ogre.MovableObject.setDefaultQueryFlags (ANY_QUERY_MASK)
        self.shadowtype=0
        ## Set up shadowing
        sceneManager.setShadowTechnique(ogre.SHADOWTYPE_TEXTURE_MODULATIVE)
        sceneManager.setShadowColour((0.5, 0.5, 0.5))
        sceneManager.setShadowFarDistance(30)
    
        if self.root.getRenderSystem().getName().startswith ('direct'): 
            sceneManager.setShadowTextureSettings(1024, 2)
        else: 
            sceneManager.setShadowTextureSettings(512, 2)
    
        ## Add some default lighting to the scene
        sceneManager.setAmbientLight( (0.25, 0.25, 0.25) )
    
        ## Create a directional light to shadow and light the bodies
        self._spot = sceneManager.createLight("Spot")
        self._spot.setType(ogre.Light.LT_DIRECTIONAL)
        self._spot.setDirection(-0.40824828,-0.81649655,-0.40824828)
        self._spot.setCastShadows(True)
        self._spot.setSpecularColour(1,1,0.8)

        ## Give us some sky
        sceneManager.setSkyBox(True,"kk3d/DesertVII",5000,True)
    
        ## Position and orient the camera
        self.camera.setPosition(13,4.5,0)
        self.camera.lookAt(0,0.5,0)
        self.camera.setNearClipDistance(0.5)

        ## Create the ODE world
        self._world = OgreOde.World(sceneManager)
    
        self._world.setGravity( (0,-9.80665,0) )
        self._world.setCFM(0.0000010 )  # 10e-5)
        self._world.setERP(0.8)
        self._world.setAutoSleep(True)
        self._world.setAutoSleepAverageSamplesCount(10)
        self._world.setContactCorrectionVelocity(1.0)
        
        ## Create something that will step the world, but don't do it automatically
        _stepper_mode_choice = 2
        _stepper_choice = 3
        time_scale = 1.7
        max_frame_time = 1.0 / 4.0
        frame_rate = 1.0 / 60.0


        if _stepper_mode_choice ==0:    stepModeType = OgreOde.StepHandler.BasicStep
        elif _stepper_mode_choice ==1:  stepModeType = OgreOde.StepHandler.FastStep
        elif _stepper_mode_choice ==2:  stepModeType = OgreOde.StepHandler.QuickStep
        else: stepModeType = OgreOde.StepHandler.QuickStep

        if _stepper_choice == 0:
            self._stepper = OgreOde.StepHandler(self._world, StepHandler.QuickStep, 
                STEP_RATE, max_frame_time,  time_scale)
        elif _stepper_choice == 1:
           self._stepper =  OgreOde.ExactVariableStepHandler(self._world, 
                stepModeType, 
                STEP_RATE,
                max_frame_time,
                time_scale)
        
        elif _stepper_choice == 2:
            self._stepper = OgreOde.ForwardFixedStepHandler(self._world, 
                stepModeType, 
                STEP_RATE,
                max_frame_time,
                time_scale)
        else:
            self._stepper = OgreOde.ForwardFixedInterpolatedStepHandler (self._world, 
                stepModeType, 
                STEP_RATE,
                frame_rate,
                max_frame_time,
                time_scale)
 
        ##_stepper.setAutomatic(OgreOde.StepHandler.AutoMode_PostFrame, mRoot)
        ##_stepper.setAutomatic(OgreOde.Stepper.AutoMode_PreFrame, mRoot)

    
        self.root.getSingleton().setFrameSmoothingPeriod(5.0)
        ##Root.getSingleton().setFrameSmoothingPeriod(0.0)
    
        ## Create a default plane to act as the ground
        self._plane = OgreOde.InfinitePlaneGeometry(ogre.Plane(ogre.Vector3(0,1,0),0),self._world, self._world.getDefaultSpace())
        s = sceneManager.createStaticGeometry("StaticFloor")
        s.setRegionDimensions((160.0, 100.0, 160.0))
        ## Set the region origin so the center is at 0 world
        s.setOrigin(ogre.Vector3.ZERO)
    
        ## Use a load of meshes to represent the floor
        i = 0
        for z in range (-80, 80, 20 ):
            for x in range (-80, 80, 20):
                name = "Plane_" + str(i)
                i += 1
                
                entity = sceneManager.createEntity(name, "plane.mesh")
                entity.setQueryFlags (STATIC_GEOMETRY_QUERY_MASK)
                entity.setUserObject(self._plane)
                entity.setCastShadows(False)
                s.addEntity(entity, ogre.Vector3(x,0,z))
            
        ## Load up our UI and display it
        pOver = ogre.OverlayManager.getSingleton().getByName("OgreOdeDemos/Overlay")    
        pOver.show()
    
        ## Initialise stuff
        self._test = 0
        self._delay = 1.0

# 
# The frame listener will notify us when a frame's 
# about to be started, so we can update stuff
# 
    def frameStarted (self, evt, Keyboard, Mouse):
        ## If we're looking at something then adjust the camera
        if (self._test and self._test.getLastNode()):
            if (self._looking):
                self.camera.lookAt(self._test.getLastNode().getPosition())
                if (self._chasing):
                    ## Thanks to Ahmed!
                    followFactor = 0.1 
                    camHeight = 5.0 
                    camDistance = 7.0 
                    q = self._test.getLastNode().getOrientation() 
                    toCam = self._test.getLastNode().getPosition() 
                    toCam.y += camHeight 
                    toCam.z -= camDistance * q.zAxis().z 
                    toCam.x -= camDistance * q.zAxis().x 
                    self.camera.move( (toCam - self.camera.getPosition()) * followFactor ) 

        ## Set the shadow distance according to how far we are from the plane that receives them
        self.sceneManager.setShadowFarDistance((abs(self.camera.getPosition().y) + 1.0) * 3.0)
    
        ## If we're running a test, tell it that a frame's ended
        if ((self._test) and (not self._paused)):
            #self._test.frameStarted(evt.timeSinceLastFrame, Keyboad, Mouse)
            self._test.frameEnded(evt.timeSinceLastFrame, Keyboard, Mouse)


# /*
# The frame listener will let us know when a frame's ended. So we
# can do stuff that we can't do in a frame started event
# e.g. delete things that we can't delete at the start of a frame,
# presumably because some processing has already been done, leaving
# things dangling, like particle systems.
# */
    def frameEnded(self, evt, Keyboard, Mouse):
        time = evt.timeSinceLastFrame
        sceneManager = self.sceneManager
        ## If we're running a test, tell it that a frame's ended
        if ((self._test) and (not self._paused)): 
            self._test.frameEnded(time, Keyboard, Mouse)
        ## Step the world and then synchronise the scene nodes with it, 
        ## we could get this to do this automatically, but we 
        ## can't be sure of what order the framelisteners will fire in
        if (self._stepper.step(time)):
            self._world.synchronise()
    
        self._delay += time
        if (self._delay > 1.0):
            changed = False
    
            ## Switch the test we're displaying
            if (Keyboard.isKeyDown(OIS.KC_F1)):
                print self._test
                del (self._test)
                self._test = SimpleScenes_BoxStack(self._world)
                changed = True
            elif (Keyboard.isKeyDown(OIS.KC_F2)):
                del self._test
                self._test = SimpleScenes_Chain(self._world)
                changed = True
            elif (Keyboard.isKeyDown(OIS.KC_F3)):
                del self._test
                self._test = SimpleScenes_Buggy(self._world)
                changed = True
            elif (Keyboard.isKeyDown(OIS.KC_F4)):
                del self._test
                self._test = SimpleScenes_TriMesh(self._world)
                changed = True
            elif (Keyboard.isKeyDown(OIS.KC_F5)):
                del self._test
                self._test = SimpleScenes_Crash(self._world)
                changed = True
            elif (Keyboard.isKeyDown(OIS.KC_F6)):
                del self._test
                self._test = SimpleScenes_Joints(self._world)
    
                if (self.camera.getPosition().z < 10.0):
                    pos = self.camera.getPosition()
                    self.camera.setPosition(pos.x,pos.y,10.0)
                    self.camera.lookAt(0,0,0)
                changed = True
            elif (Keyboard.isKeyDown(OIS.KC_F7)):
                del self._test
                self._test = SimpleScenes_Zombie(self._world)
                changed = True
    
            ## If we changed the test...
            if ((self._test) and (changed)):
                ## Register it with the stepper, so we can (for example) add forces before each step
                self._stepper.setStepListener(self._test)
    
                ## Set the UI to show the test's details
                ogre.OverlayManager.getSingleton().getOverlayElement("OgreOdeDemos/Name")\
                                .setCaption(ogre.UTFString("Name: " + self._test.getName()))
                ogre.OverlayManager.getSingleton().getOverlayElement("OgreOdeDemos/Keys")\
                                .setCaption(ogre.UTFString("Keys: " + self._test.getKeys()))
    
                self._delay = 0.0
    
            ## Switch shadows
            if (Keyboard.isKeyDown(OIS.KC_SPACE)):
                self.shadowtype += 1
                if (self.shadowtype > 5):
                    self.shadowtype = 0
                    
                if self.shadowtype == 0:
                    sceneManager.setShadowTechnique (ogre.SHADOWTYPE_NONE) 
                elif self.shadowtype == 1:
                    sceneManager.setShadowTechnique(ogre.SHADOWTYPE_TEXTURE_MODULATIVE)
                    sceneManager.setShadowColour(ogre.ColourValue(0.5, 0.5, 0.5))
                    sceneManager.setShadowFarDistance(30)
                    if self.root.getRenderSystem().getName().startswith("direct") :
                        sceneManager.setShadowTextureSettings(1024, 2)
                    else :
                        sceneManager.setShadowTextureSettings(512, 2)
                elif self.shadowtype == 2:
                    sceneManager.setShadowTechnique (ogre.SHADOWTYPE_STENCIL_ADDITIVE) 
                elif self.shadowtype == 3:
                    sceneManager.setShadowTechnique (ogre.SHADOWTYPE_STENCIL_MODULATIVE )  
                elif self.shadowtype == 4:
                    sceneManager.setShadowTechnique (ogre.SHADOWTYPE_TEXTURE_ADDITIVE )     
                    sceneManager.setShadowColour((0.5, 0.5, 0.5))
                    sceneManager.setShadowFarDistance(30)
                    if self.root.getRenderSystem().getName().startswith("direct"):
                        sceneManager.setShadowTextureSettings(1024, 2)
                    else :
                        sceneManager.setShadowTextureSettings(512, 2)
                else:
                    sceneManager.setShadowTechnique (ogre.SHADOWTYPE_NONE) 
                self._delay = 0.0
    
            ## Look at the last object, chase it, or not
            if (Keyboard.isKeyDown(OIS.KC_M)):
                if (self._looking):
                    if (self._chasing): 
                        self._looking = self._chasing = False
                    else:
                        self._chasing = True
                else: 
                    self._looking = True
                self._delay = 0.0
    
            ## Switch debugging objects on or off
            if (Keyboard.isKeyDown(OIS.KC_E)):
                self._world.setShowDebugGeometries(not self._world.getShowDebugGeometries())
                self._delay = 0.0
    
            ## Pause or unpause the simulation
            if (Keyboard.isKeyDown(OIS.KC_P)):
                self._paused = not self._paused
                self._delay = 0.0
    
                self._stepper.pause(self._paused)
    
                if self._paused:
                    timeSet = 0.0
                else:
                    timeSet = 1.0
                it = sceneManager.getMovableObjectIterator(ogre.ParticleSystemFactory.FACTORY_TYPE_NAME)
                while(it.hasMoreElements()):
                    p = it.getNext()
                    p.setSpeedFactor(timeSet)
                
    ## we need to register the framelistener
    def _createFrameListener(self):
        ## note we pass ourselves as the demo to the framelistener
        self.frameListener = SimpleScenesFrameListener(self, self.renderWindow, self.camera)
        self.root.addFrameListener(self.frameListener)




if __name__ == '__main__':
    try:
        application = SimpleScenesApplication()
        application.go()
    except ogre.Exception, e:
        print e
        print dir(e)
