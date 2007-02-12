# 
#   OgreNewt library - connecting Ogre and Newton!
#   Demo01_TheBasics - basic demo that shows a simple OgreNewt world, and how
#   to setup basic rigid bodies.
# 

import Ogre
import OgreNewt
import OIS
import SampleFramework as sf
from BasicFrameListener import *     # a simple frame listener that updates physics as required..

class OgreNewtonApplication (sf.Application):
    def __init__ ( self):
        sf.Application.__init__(self)
        self.World = OgreNewt.World()
        self.EntityCount = 0
        self.bodies=[]
        sf.Application.debugText = "Press Space Bar to fire.  ESC to end"

    def __del__ (self):
        ## delete the world when we're done.
        #OgreNewt.Debugger.getSingleton().deInit()
        del self.bodies
        del self.World;
    
        ## de-initialize the debugger.
#        OgreNewt.Debugger.getSingleton().deInit()
        sf.Application.__del__(self)


    def _createScene ( self ):
        ## sky box.
        self.sceneManager.setSkyBox(True, "Examples/CloudyNoonSkyBox")
    
        ## this will be a static object that we can throw objects at.  we'll use a simple cylinder primitive.
        ## first I load the visual mesh that represents it.  I have some simple primitive shaped .mesh files in
        ## the "primitives" directory to make this simple... all of them have a basic size of "1" so that they
        ## can easily be scaled to fit any size primitive.
        floor = self.sceneManager.createEntity("Floor", "cylinder.mesh" )
        floor.setNormaliseNormals(True)
        floornode = self.sceneManager.getRootSceneNode().createChildSceneNode( "FloorNode" )
        floornode.attachObject( floor )
        floor.setMaterialName( "Simple/BeachStones" )
        floor.setCastShadows( False )
        
        ## okay, the basic mesh is loaded.  now let's decide the size of the object, and scale the node.
        siz = Ogre.Vector3(5,2.5,2.5)
        floornode.setScale( siz )

             
        ## here's where we make a collision shape for the physics.  note that we use the same size as
        ## above.
        col = OgreNewt.Cylinder(self.World, 2.5, 5)
        #col = OgreNewt.CollisionPrimitives.Cylinder(self.World, 2.5, 5)
    
        ## now we make a new rigid body based on this collision shape.
        body = OgreNewt.Body( self.World, col )
    
        ## we`re done with the collision shape, we can delete it now.
        del col
    
        ## now we "attach" the rigid body to the scene node that holds the visual object, and set it's
        ## original position and orientation.  all rigid bodies default to mass=0 (static, immobile), so
        ## that's all we'll need to do for this object.  dynamic objects have a few more steps, so look
        ## at the code in the FrameListener for more.
        body.attachToNode( floornode )
        body.setPositionOrientation( Ogre.Vector3(0.0,-10.0,0.0), Ogre.Quaternion.IDENTITY )
        
        self.bodies.append(body)
        ## position camera
        self.msnCam = self.sceneManager.getRootSceneNode().createChildSceneNode()
        self.msnCam.attachObject( self.camera )
        self.camera.setPosition(0.0, 0.0, 0.0)
        self.msnCam.setPosition( 0.0, -10.0, 20.0)
    
        ##make a light
        light = self.sceneManager.createLight( "Light1" )
        light.setType( Ogre.Light.LT_POINT )
        light.setPosition( Ogre.Vector3(0.0, 100.0, 100.0) )

    def _createFrameListener(self):
 
        ## this is a basic frame listener included with OgreNewt that does nothing but update the
        ## physics at a set framerate for you.  complex project will want more control, but this
        ## works for simple demos like this.  feel free to look at the source to see how it works.
        self.NewtonListener = BasicFrameListener( self.renderWindow, self.sceneManager, self.World, 60 )
        self.root.addFrameListener(self.NewtonListener)
        
        ## this is our custom frame listener for this app, that lets us shoot cylinders with the space bar, move
        ## the camera, etc.
        self.frameListener = OgreNewtonFrameListener( self.renderWindow, self.camera, self.sceneManager, self.World, self.msnCam, self.NewtonListener )
        self.root.addFrameListener(self.frameListener)
       
       
class OgreNewtonFrameListener(sf.FrameListener):
    def __init__(self, renderWindow, camera, Mgr, World, msnCam, NewtonListener):

        sf.FrameListener.__init__(self, renderWindow, camera)
        self.World = World
        self.msnCam = msnCam
        self.camera= camera
        self.sceneManager = Mgr
        self.timer=0
        self.count=0
        self.bodies=[]
        self.basicframelistener = NewtonListener
        self.Debug = False
 

    
    def frameStarted(self, frameEvent):
        ## in this frame listener we control the camera movement, and allow the user to "shoot" cylinders
        ## by pressing the space bar.  first the camera movement...
        quat = self.msnCam.getOrientation()
    
        vec = Ogre.Vector3(0.0,0.0,-0.5)
        trans = quat * vec
    
        vec = Ogre.Vector3(0.5,0.0,0.0)
        strafe = quat * vec
        
        ##Need to capture/update each device - this will also trigger any listeners
        ## OIS specific !!!!
        self.Keyboard.capture()    
        self.Mouse.capture()
        
        ## now lets handle mouse input
        ms = self.Mouse.getMouseState()
        self.msnCam.pitch( Ogre.Radian(ms.Y.rel * -0.5) )
        self.msnCam.yaw( Ogre.Radian(ms.X.rel * -0.5), Ogre.Node.TS_WORLD )

        ##and Keyboard
        if (self.Keyboard.isKeyDown(OIS.KC_UP)):
            self.msnCam.translate(trans);
        if (self.Keyboard.isKeyDown(OIS.KC_DOWN)):
            self.msnCam.translate(trans * -1.0);
        if (self.Keyboard.isKeyDown(OIS.KC_LEFT)):
            self.msnCam.translate(strafe * -1.0);
        if (self.Keyboard.isKeyDown(OIS.KC_RIGHT)):
            self.msnCam.translate(strafe);
        ## now "shoot" an object!
        if (self.Keyboard.isKeyDown(OIS.KC_SPACE)):
            if (self.timer <= 0.0):

                ## we get the position and direction from the camera...
                camorient = self.msnCam.getWorldOrientation()
                vec = Ogre.Vector3(0,0,-1)
                direct = camorient * vec
    
                ## then make the visual object (again a cylinder)
                #pos = Ogre.Vector3(self.msnCam.getWorldPosition())
                pos = self.msnCam.getWorldPosition()
    
                name = "Body"+str( self.count )
                self.count += 1
    
                ent = self.sceneManager.createEntity( name, "cylinder.mesh" )
                node = self.sceneManager.getRootSceneNode().createChildSceneNode( name + "Node" )
                node.attachObject( ent )
                
                node.setPosition(0.0, 0.0, 0.0)
                
                ent.setMaterialName( "Simple/dirt01" )
                ent.setNormaliseNormals(True)
    
                ## again, make the collision shape.
                ##col = OgreNewt.CollisionPrimitives.Cylinder(self.World, 1, 1)
                col = OgreNewt.Cylinder(self.World, 1, 1)
                
                ## then make the rigid body.    ## need to keep it around see below.......
                body = OgreNewt.Body( self.World, col)
    
                ##no longer need the collision shape object
                del col
    
                ## something new: moment of inertia for the body.  this describes how much the body "resists"
                ## rotation on each axis.  realistic values here make for MUCH more realistic results.  luckily
                ## OgreNewt has some helper functions for calculating these values for many primitive shapes!
                inertia = OgreNewt.CalcSphereSolid( 10.0, 1.0 )
                body.setMassMatrix( 10.0, inertia )
    
                ## attach to the scene node.
                body.attachToNode( node )
    
                ## this is a standard callback that simply add a gravitational force (-9.8*mass) to the body.
                body.setStandardForceCallback()
    
                ## set the initial orientation and velocity!
                body.setPositionOrientation( pos, camorient )
                body.setVelocity( (direct * 50.0) )
                
                ## note that we have to keep the bodies around :)
                self.bodies.append(body)
                
                self.timer = 0.2
        self.timer -= frameEvent.timeSinceLastFrame
        if (self.Keyboard.isKeyDown(OIS.KC_F3)):
            if self.Debug:
                self.Debug = False
            else:
                self.Debug = True
            self.basicframelistener.debug ( self.Debug )
            
        if (self.Keyboard.isKeyDown(OIS.KC_ESCAPE)):
            ##OgreNewt.Debugger.getSingleton().deInit()
            return False
        return True        
    

if __name__ == '__main__':
    try:
        application = OgreNewtonApplication()
        application.go()
    except Ogre.Exception, e:
        print e
        print dir(e)
    
    