# /*
#   OgreNewt library - connecting Ogre and Newton!

#   Demo03_CollisionCallbacks - the material system in Newton is extremely powerful.  by assigning
#   materials to bodies, you can define all kinds of specific behavior when objects collide.  you can use
#   material callbacks to create hot spots, or make sound effects, or spawn particles, etc.  this example
#   uses a conveyor belt as an example.  look at the conveyorMatCallback to see how it's implemented.
# */
import Ogre
import OgreNewt
import OIS
import SampleFramework as sf
import random
from BasicFrameListener import *     # a simple frame listener that updates physics as required..
from conveyorBelt import *
from conveyorMatCallback import *
BT_BASIC = 0
BT_CONVEYOR = 1
class OgreNewtonApplication (sf.Application):
    def __init__ ( self):
        sf.Application.__init__(self)
        self.World = OgreNewt.World()
        self.EntityCount = 0
        self.bodies=[]
        self.Belts=[]
        sf.Application.debugText = "Press Space Bar drop boxes.  ESC to end"


    def __del__ (self):
        ## delete the world when we're done.
        del self.bodies
        del self.Belts
        del self.World
    
        ## de-initialize the debugger.
    #   OgreNewt.Debugger.getSingleton().deInit()
        sf.Application.__del__(self)


    def _createScene ( self ):
        ## sky box.
        self.sceneManager.setSkyBox(True, "Examples/CloudyNoonSkyBox")
        
        ## shadows on!
        self.sceneManager.setShadowTechnique( Ogre.SHADOWTYPE_STENCIL_MODULATIVE )
    
        self.createMaterials()

        ## lets make a few conveyor belt objects.
        belt = conveyorBelt()
        belt.init( "ConveyorBelt1", self.sceneManager, 
                self.World, Ogre.Vector3(15,0.3,3), Ogre.Vector3(1,0,0), 2.0, 
                Ogre.Vector3(0,2,0), Ogre.Quaternion.IDENTITY, self.MatConveyor, BT_CONVEYOR )
        self.Belts.append( belt )
    
        belt = conveyorBelt()
        belt.init( "ConveyorBelt2", self.sceneManager, self.World, Ogre.Vector3(23,0.3,4), Ogre.Vector3(-1,0,0), 6.0, Ogre.Vector3(3,-1,0),
            Ogre.Quaternion.IDENTITY, self.MatConveyor, BT_CONVEYOR )
        self.Belts.append( belt )
    
        ## floor object!
        floor = self.sceneManager.createEntity("Floor", "simple_terrain.mesh" )
        floornode = self.sceneManager.getRootSceneNode().createChildSceneNode( "FloorNode" )
        floornode.attachObject( floor )
        floor.setMaterialName( "Simple/BeachStones" )
    
        floor.setCastShadows( False )
    
        ##Ogre.Vector3 siz(100.0, 10.0, 100.0)
        col = OgreNewt.TreeCollision( self.World, floornode, True )
        bod = OgreNewt.Body( self.World, col )
        
        ##floornode.setScale( siz )
        bod.attachToNode( floornode )
        bod.setPositionOrientation( Ogre.Vector3(0.0,-10.0,0.0), Ogre.Quaternion.IDENTITY )
        
        self.bodies.append ( bod )
    
        ## position camera
        self.msnCam = self.sceneManager.getRootSceneNode().createChildSceneNode()
        self.msnCam.attachObject( self.camera )
        self.camera.setPosition(0.0, 0.0, 0.0)
        self.msnCam.setPosition( 0.0, 2.0, 22.0)
    
        ##make a light
        light = self.sceneManager.createLight( "Light1" )
        light.setType( Ogre.Light.LT_POINT )
        light.setPosition( Ogre.Vector3(0.0, 100.0, 100.0) )
    
    
    def createMaterials(self) :
        self.MatDefault = self.World.getDefaultMaterialID()
        self.MatConveyor = OgreNewt.MaterialID( self.World )
    
        self.MatPairDefaultConveyor = OgreNewt.MaterialPair( self.World, self.MatDefault, self.MatConveyor )
        self.ConveyorCallback = conveyorMatCallback( BT_CONVEYOR )
        self.MatPairDefaultConveyor.setContactCallback( self.ConveyorCallback )
        self.MatPairDefaultConveyor.setDefaultFriction( 1.5, 1.4 )


    def _createFrameListener(self):
        ## this is a basic frame listener included with OgreNewt that does nothing but update the
        ## physics at a set framerate for you.  complex project will want more control, but this
        ## works for simple demos like this.  feel free to look at the source to see how it works.
        self.NewtonListener = BasicFrameListener( self.renderWindow, self.sceneManager, self.World, 120 )
        self.root.addFrameListener(self.NewtonListener)

        ## this is our custom frame listener for this app, that lets us shoot cylinders with the space bar, move
        ## the camera, etc.
        self.frameListener = OgreNewtonFrameListener( self.renderWindow, self.camera, self.sceneManager, 
                                                    self.World, self.msnCam, self.NewtonListener )
        self.root.addFrameListener(self.frameListener)


        
    
    
class OgreNewtonFrameListener(sf.FrameListener):
    def __init__(self, renderWindow, camera, Mgr, World, msnCam,  NewtonListener):

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
        self.msnCam.pitch( Ogre.Radian(ms.Y.rel * -0.2) )
        self.msnCam.yaw( Ogre.Radian(ms.X.rel * -0.2), Ogre.Node.TS_WORLD )

        ##and Keyboard
        if (self.Keyboard.isKeyDown(OIS.KC_UP)):
            self.msnCam.translate(trans * 1)
        if (self.Keyboard.isKeyDown(OIS.KC_DOWN)):
            self.msnCam.translate(trans * -1)
        if (self.Keyboard.isKeyDown(OIS.KC_LEFT)):
            self.msnCam.translate(strafe * -1)
        if (self.Keyboard.isKeyDown(OIS.KC_RIGHT)):
            self.msnCam.translate(strafe * 1)
        ## now "shoot" an object!
        if (self.Keyboard.isKeyDown(OIS.KC_SPACE)):
            if (self.timer <= 0.0):

                size = Ogre.Vector3 ( 0.2 + random.random() % 2, 0.2 + random.random() % 2, 0.2 + random.random() % 2 )
                mass = size.length()
                pos = self.msnCam.getWorldPosition()
                
                name = "Body "+str( self.count )
                self.count += 1
                
                ent = self.sceneManager.createEntity( name, "box.mesh" )
                node = self.sceneManager.getRootSceneNode().createChildSceneNode( name )
                node.attachObject( ent )
                
                node.setScale( size )
                
                ent.setMaterialName( "Simple/BumpyMetal" )
                ent.setNormaliseNormals(True)
                
                col =OgreNewt.Box( self.World, size )
                body = OgreNewt.Body( self.World, col )
                del col
                inertia = OgreNewt.CalcBoxSolid( mass, size )
                body.setMassMatrix( mass, inertia )
                body.attachToNode( node )
                body.setStandardForceCallback()
                body.setPositionOrientation( Ogre.Vector3(-5,8,0), Ogre.Quaternion.IDENTITY )
                self.bodies.append ( body )    
                 
                self.timer = 1.5
                
        self.timer -= frameEvent.timeSinceLastFrame
        if (self.Keyboard.isKeyDown(OIS.KC_F3)):
            if self.Debug:
                self.Debug = False
            else:
                self.Debug = True
            self.basicframelistener.debug ( self.Debug )
        if (self.Keyboard.isKeyDown(OIS.KC_ESCAPE)):
            return False
        #sf.FrameListener.frameStarted(self, frameEvent)
        return True        

        


if __name__ == '__main__':
    try:
        application = OgreNewtonApplication()
        application.go()
    except Ogre.Exception, e:
        print e
        print dir(e)
    
     