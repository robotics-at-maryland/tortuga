# /*
# SimpleScenesApplication.cpp
# --------------------------
# The main applicatin that handles switching between the
# different scenes in this demo, as well as any common 
# setup and input handling.
# */

## The tests we can display
import SimpleScenes_BoxStack.py
#include "SimpleScenes_Buggy.h"
#include "SimpleScenes_Chain.h"
#include "SimpleScenes_TriMesh.h"
#include "SimpleScenes_Crash.h"
#include "SimpleScenes_Joints.h"
#include "SimpleScenes_Zombie.h"


## We need a frame listener class
        
class SimpleScenesFrameListener ( sf.FrameListener ):
    def __init__( self, renderWindow, camera ):
        sf.FrameListener.__init__(self, renderWindow, camera)
        
        
    def frameStarted(self, evt):
        ## Do the default demo input handling and keep our UI display
        ## in sync with the other stuff (frame rate, logo, etc)
        show = self.mStatsOn;
        bOK = sf.FrameListener.frameStarted(self, frameEvent)
        if (self.mStatsOn != show):
            pOver = Ogre.OverlayManager.getSingleton().getByName("OgreOdeDemos/Overlay");    
            if (pOver):
                if (self.mStatsOn) :
                    pOver.show();
                else :
                    pOver.hide();
                    
        ## Tell the demo application that it needs to handle input
        self._demo.frameStarted(self, evt,mInputDevice);  ### AJM
    
        ## Quit or carry on according to the normal demo input
        return bOK;
    
    # 
    # The frame listener is informed after every frame
    # 
    def frameEnded(self, evt):
        ## Tell our demo that the frame's ended before doing default processing
        self._demo.frameEnded(self,evt, self.mInputDevice);
        return sf.FrameListener.frameEnded(self, evt)

# /*
# Create the scene from an Ogre point of view
# and create the common OgreOde things we'll need
# */
class SimpleScenesApplication(sf.Application):
    def _createScene(self):
        sceneManager = self.sceneManager
        ogre.MovableObject.setDefaultQueryFlags (ogre.ANY_QUERY_MASK);

        ## Set up shadowing
        sceneManager.setShadowTechnique(ogre.SHADOWTYPE_TEXTURE_MODULATIVE);
        sceneManager.setShadowColour((0.5, 0.5, 0.5));
        sceneManager.setShadowFarDistance(30);
    
        if self.root.getRenderSystem().getName().startswith ('direct'): 
            sceneManager.setShadowTextureSettings(1024, 2);
        else: 
            sceneManager.setShadowTextureSettings(512, 2);
    
        ## Add some default lighting to the scene
        sceneManager.setAmbientLight( (0.25, 0.25, 0.25) );
    
        ## Create a directional light to shadow and light the bodies
        _spot = sceneManager.createLight("Spot");
        _spot.setType(ogre.Light.LT_DIRECTIONAL);
        _spot.setDirection(-0.40824828,-0.81649655,-0.40824828);
        _spot.setCastShadows(True);
        _spot.setSpecularColour(1,1,0.8);

        ## Give us some sky
        sceneManager.setSkyBox(True,"kk3d/DesertVII",5000,True);
    
        ## Position and orient the camera
        self.camera.setPosition(13,4.5,0);
        self.camera.lookAt(0,0.5,0);
        self.camera.setNearClipDistance(0.5);

    ## Create the ODE world
    self._world = OgreOde.World(sceneManager);

    self._world.setGravity( (0,-9.80665,0) );
    self._world.setCFM(0.0000010 )  # 10e-5);
    self._world.setERP(0.8);
    self._world.setAutoSleep(True);
    self._world.setContactCorrectionVelocity(1.0);
    
    ## Create something that will step the world, but don't do it automatically

    ## self._stepper = OgreOde.QuickStepper(time_step);
    ##self._stepper = OgreOde.ExactVariableQuickStepper(_time_step); 
    ##self._stepper = OgreOde.ForwardFixedQuickStepper(_time_step); 
    self._stepper = OgreOde.ForwardFixedInterpolatedQuickStepper (_time_step);
    self._stepper.setAutomatic(OgreOde.Stepper.AutoMode_NotAutomatic, self.root);

    ##Root.getSingleton().setFrameSmoothingPeriod(5.0);
    Root.getSingleton().setFrameSmoothingPeriod(0.0);

    ## Create a default plane to act as the ground
    self._plane = OgreOde.InfinitePlaneGeometry(ogre.Plane(ogre.Vector3(0,1,0),0),self._world.getDefaultSpace());

    ## Use a load of meshes to represent the floor
    i = 0;
    for z in range (-80, 80, 20 ):
        for x in range (-80, 80, 20):
            name = "Plane_" + str(i);
            i += 1
            
            mPlane = sceneManager.createEntity(name, "Plane.mesh");
            mPlane.setCastShadows(False);
            
            mPlaneNode = sceneManager.getRootSceneNode().createChildSceneNode(name);
            mPlaneNode.attachObject(mPlane);
            mPlaneNode.setPosition(x,0,z);

    ## Load up our UI and display it
    pOver = ogre.OverlayManager.getSingleton().getByName("OgreOdeDemos/Overlay");    
    pOver.show();

    ## Initialise stuff
    self._test = 0;
    self._delay = 1.0;
}

# 
# The frame listener will notify us when a frame's 
# about to be started, so we can update stuff
# 
void SimpleScenesApplication.frameStarted(const FrameEvent& evt,InputReader* mInputDevice)
{
    ## If we're looking at something then adjust the camera
    if (self._test && self._test.getLastNode())
    {
        if (_looking)
        {
            self.camera.lookAt(self._test.getLastNode().getPosition());

            if (_chasing)
            {
                ## Thanks to Ahmed!
                const Real followFactor = 0.1; 
                const Real camHeight = 5.0; 
                const Real camDistance = 7.0; 
                
                Quaternion q = self._test.getLastNode().getOrientation(); 
                Vector3 toCam = self._test.getLastNode().getPosition(); 

                toCam.y += camHeight; 
                toCam.z -= camDistance * q.zAxis().z; 
                toCam.x -= camDistance * q.zAxis().x; 
      
                self.camera.move( (toCam - self.camera.getPosition()) * followFactor ); 
            }
        }
    }

    ## Set the shadow distance according to how far we are from the plane that receives them
    sceneManager.setShadowFarDistance((fabs(self.camera.getPosition().y) + 1.0) * 3.0);

    ## If we're running a test, tell it that a frame's ended
    if ((self._test)&&(!_paused))
        self._test.frameStarted(evt.timeSinceLastFrame,mInputDevice);
}

# /*
# The frame listener will let us know when a frame's ended. So we
# can do stuff that we can't do in a frame started event
# e.g. delete things that we can't delete at the start of a frame,
# presumably because some processing has already been done, leaving
# things dangling, like particle systems.
# */
void SimpleScenesApplication.frameEnded(const FrameEvent& evt,InputReader* mInputDevice)
{
    Real time = evt.timeSinceLastFrame;

    ## If we're running a test, tell it that a frame's ended
    if ((self._test)&&(!_paused)) 
        self._test.frameEnded(time,mInputDevice);
    
    ## Step the world and then synchronise the scene nodes with it, 
    ## we could get this to do this automatically, but we 
    ## can't be sure of what order the framelisteners will fire in
    if (self._stepper.step(time))
        self._world.synchronise();

    self._delay += time;
    if (self._delay > 1.0)
    {
        bool changed = False;

        ## Switch the test we're displaying
        if (mInputDevice.isKeyDown(KC_F1))
        {
            delete self._test;
            self._test = new SimpleScenes_BoxStack();
            changed = True;
        }
        else if (mInputDevice.isKeyDown(KC_F2))
        {
            delete self._test;
            self._test = new SimpleScenes_Chain();
            changed = True;
        }
        else if (mInputDevice.isKeyDown(KC_F3))
        {
            delete self._test;
            self._test = new SimpleScenes_Buggy();
            changed = True;
        }
        else if (mInputDevice.isKeyDown(KC_F4))
        {
            delete self._test;
            self._test = new SimpleScenes_TriMesh();
            changed = True;
        }
        else if (mInputDevice.isKeyDown(KC_F5))
        {
            delete self._test;
            self._test = new SimpleScenes_Crash();
            changed = True;
        }
        else if (mInputDevice.isKeyDown(KC_F6))
        {
            delete self._test;
            self._test = new SimpleScenes_Joints();

            if (self.camera.getPosition().z < 10.0)
            {
                Vector3 pos = self.camera.getPosition();
                self.camera.setPosition(pos.x,pos.y,10.0);
                self.camera.lookAt(0,0,0);
            }
            changed = True;
        }
        else if (mInputDevice.isKeyDown(KC_F7))
        {
            delete self._test;
            self._test = new SimpleScenes_Zombie();

            changed = True;
        }

        ## If we changed the test...
        if ((self._test)&&(changed))
        {
            ## Register it with the stepper, so we can (for example) add forces before each step
            self._stepper.setStepListener(self._test);

            ## Set the UI to show the test's details
            OverlayManager.getSingleton().getOverlayElement("OgreOdeDemos/Name").setCaption("Name: " + self._test.getName());
            OverlayManager.getSingleton().getOverlayElement("OgreOdeDemos/Keys").setCaption("Keys: " + self._test.getKeys());

            self._delay = 0.0;
        }

        ## Switch shadows
        if (mInputDevice.isKeyDown(KC_SPACE))
        {
            static Ogre.uint shadowtype = 0;
            shadowtype += 1;
            if (shadowtype > 5)
                shadowtype = 0;
            switch (shadowtype)
            {
            case 0:
                sceneManager.setShadowTechnique (SHADOWTYPE_NONE); 
                break;
            case 1:
                sceneManager.setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
                sceneManager.setShadowColour(ColourValue(0.5, 0.5, 0.5));
                sceneManager.setShadowFarDistance(30);
                if (StringUtil.startsWith(self.root.getRenderSystem().getName(),"direct")) 
                    sceneManager.setShadowTextureSettings(1024, 2);
                else 
                    sceneManager.setShadowTextureSettings(512, 2);
                break;
            case 2:
                sceneManager.setShadowTechnique (SHADOWTYPE_STENCIL_ADDITIVE); 
                break;
            case 3:
                sceneManager.setShadowTechnique (SHADOWTYPE_STENCIL_MODULATIVE );  
                break;
            case 4:
                sceneManager.setShadowTechnique (SHADOWTYPE_TEXTURE_ADDITIVE );     
                sceneManager.setShadowColour(ColourValue(0.5, 0.5, 0.5));
                sceneManager.setShadowFarDistance(30);
                if (StringUtil.startsWith(self.root.getRenderSystem().getName(),"direct")) 
                    sceneManager.setShadowTextureSettings(1024, 2);
                else 
                    sceneManager.setShadowTextureSettings(512, 2);
                break;
            default:
                sceneManager.setShadowTechnique (SHADOWTYPE_NONE); 
                break;
            }
            self._delay = 0.0;
        }

        ## Look at the last object, chase it, or not
        if (mInputDevice.isKeyDown(KC_M))
        {
            if (_looking)
            {
                if (_chasing) 
                {
                    _looking = _chasing = False;
                }
                else
                {
                    _chasing = True;
                }
            }
            else 
            {
                _looking = True;
            }
            self._delay = 0.0;
        }

        ## Switch debugging objects on or off
        if (mInputDevice.isKeyDown(KC_E))
        {
            self._world.setShowDebugObjects(!self._world.getShowDebugObjects());
            self._delay = 0.0;
        }

        ## Pause or unpause the simulation
        if (mInputDevice.isKeyDown(KC_P))
        {
            _paused = !_paused;
            self._delay = 0.0;

            self._stepper.pause(_paused);

            const Real timeSet = (_paused)? 0.0 : 1.0;

            Ogre.SceneManager.MovableObjectIterator it = 
                sceneManager.getMovableObjectIterator(ParticleSystemFactory.FACTORY_TYPE_NAME);
            while(it.hasMoreElements())
            {
                ParticleSystem* p = static_cast<ParticleSystem*>(it.getNext());
                p.setSpeedFactor(timeSet);
            }
        }
    }
}





if __name__ == '__main__':
    try:
        application = SimpleScenesApplication()
        application.go()
    except ogre.Exception, e:
        print e
        print dir(e)
