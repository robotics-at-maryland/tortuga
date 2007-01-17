# /*
#   OgreNewt library - connecting Ogre and Newton!

#   Demo02_Joints - basic demo that shows how to connect rigid bodies via joints.
# */

import Ogre
import OgreNewt
import OIS
import SampleFramework as sf
from BasicFrameListener import *     # a simple frame listener that updates physics as required..
from MyCustomBallSocket import *


class OgreNewtonApplication (sf.Application):
    def __init__ ( self):
        sf.Application.__init__(self)
        self.World = OgreNewt.World()
        self.EntityCount = 0
        self.bodies=[]
        sf.Application.debugText = "Press Space Bar to fire bouncing balls.  ESC to end"

    def __del__ (self):
        ## delete the world when we're done.
        del self.bodies
        del self.World
    
        ## de-initialize the debugger.
    #   OgreNewt.Debugger.getSingleton().deInit();
        sf.Application.__del__(self)
        
    def makeSimpleBox( self, size, pos,  orient ):
        ## base mass on the size of the object.
        mass = size.x * size.y * size.z * 2.5
            
        ## calculate the inertia based on box formula and mass
        inertia = OgreNewt.CalcBoxSolid( mass, size )
    
        box1 = self.sceneManager.createEntity( "Entity"+str(self.EntityCount), "box.mesh" )
        self.EntityCount += 1
        box1node = self.sceneManager.getRootSceneNode().createChildSceneNode()
        box1node.attachObject( box1 )
        box1node.setScale( size )
        box1.setNormaliseNormals(True)
    
        col = OgreNewt.Box( self.World, size )
        bod = OgreNewt.Body( self.World, col )
        del col
                    
        bod.attachToNode( box1node )
        bod.setMassMatrix( mass, inertia )
        bod.setStandardForceCallback()
    
        box1.setMaterialName( "Simple/BumpyMetal" )
    
        bod.setPositionOrientation( pos, orient )
    
        return bod

    def _createScene ( self ):
        ## sky box.
        self.sceneManager.setSkyBox(True, "Examples/CloudyNoonSkyBox")
        
        ## shadows on!
        self.sceneManager.setShadowTechnique( Ogre.SHADOWTYPE_STENCIL_ADDITIVE )
       
        ## floor object!
        floor = self.sceneManager.createEntity("Floor", "simple_terrain.mesh" )
        floornode = self.sceneManager.getRootSceneNode().createChildSceneNode( "FloorNode" )
        floornode.attachObject( floor )
        floor.setMaterialName( "Simple/BeachStones" )
        floor.setCastShadows( False )
    
        ##-------------------------------------------------------------
        ## add some other objects.
        floor2 = self.sceneManager.createEntity("Floor2", "simple_terrain.mesh" )
        floornode2 = floornode.createChildSceneNode( "FloorNode2" )
        floornode2.attachObject( floor2 )
        floor2.setMaterialName( "Simple/BeachStones" )
        floor2.setCastShadows( False )
        floornode2.setPosition( Ogre.Vector3(80.0, 0.0, 0.0) )
    
        floor3 = self.sceneManager.createEntity("Floor3", "simple_terrain.mesh" )
        floornode3 = floornode.createChildSceneNode( "FloorNode3" )
        floornode3.attachObject( floor3 )
        floor3.setMaterialName( "Simple/BeachStones" )
        floor3.setCastShadows( False )
        floornode3.setPosition( Ogre.Vector3(-80.0, -5.0, 0.0) )
        floornode3.setOrientation( Ogre.Quaternion( Ogre.Degree(d=15.0), Ogre.Vector3.UNIT_Z ) )
        ##-------------------------------------------------------------
    
        ## using the new "SceneParser" TreeCollision primitive.  this will automatically parse an entire tree of
        ## SceneNodes (parsing all children), and add collision for all meshes in the tree.
        stat_col = OgreNewt.TreeCollisionSceneParser( self.World )
        stat_col.parseScene( floornode, True )
        bod = OgreNewt.Body( self.World, stat_col )
        del stat_col
        
        bod.attachToNode( floornode )
        bod.setPositionOrientation( Ogre.Vector3(0.0,-20.0,0.0), Ogre.Quaternion.IDENTITY )
        self.bodies.append( bod )
        
        ## make a simple rope.
        size = Ogre.Vector3(3,1.5,1.5)
        pos = Ogre.Vector3(0,3,0)
        orient = Ogre.Quaternion.IDENTITY
    
        ## loop through, making bodies and connecting them.
        parent = None
        child = None
    
        for x in range (5):
            ## make the next box.
            child = self.makeSimpleBox(size, pos, orient)
            self.bodies.append(child)
            
            ## make the joint right between the bodies...
            if (parent):
                joint = OgreNewt.BallAndSocket( self.World, child, parent, pos-Ogre.Vector3(size.x/2,0,0) )
                
            else:
                ## no parent, this is the first joint, so just pass NULL as the parent, to stick it to the "world"
                joint = OgreNewt.BallAndSocket( self.World, child, None, pos-Ogre.Vector3(size.x/2,0,0) )
                
            ## offset pos a little more.
            pos += Ogre.Vector3(size.x,0,0)
    
            ## save the last body for the next loop!
            parent = child
            
            ## NOW - we also have to kepe copies of the joints, otherwise they get deleted !!!
            self.bodies.append ( joint) 
            
        ## position camera
        self.msnCam = self.sceneManager.getRootSceneNode().createChildSceneNode()
        self.msnCam.attachObject( self.camera )
        self.camera.setPosition(0.0, 0.0, 0.0)
        self.msnCam.setPosition( 0.0, -3.0, 20.0)
    
        ##make a light
        light = self.sceneManager.createLight( "Light1" )
        light.setType( Ogre.Light.LT_POINT )
        light.setPosition( Ogre.Vector3(0.0, 100.0, 100.0) )
        
    def _createFrameListener(self):
        ## this is our custom frame listener for this app, that lets us shoot cylinders with the space bar, move
        ## the camera, etc.
        self.frameListener = OgreNewtonFrameListener( self.renderWindow, self.camera, self.sceneManager, self.World, self.msnCam )
        self.root.addFrameListener(self.frameListener)

        ## this is a basic frame listener included with OgreNewt that does nothing but update the
        ## physics at a set framerate for you.  complex project will want more control, but this
        ## works for simple demos like this.  feel free to look at the source to see how it works.
        self.NewtonListener = BasicFrameListener( self.renderWindow, self.sceneManager, self.World, 120 )
        self.root.addFrameListener(self.NewtonListener)    
    
class OgreNewtonFrameListener(sf.FrameListener):
    def __init__(self, renderWindow, camera, Mgr, World, msnCam):

        sf.FrameListener.__init__(self, renderWindow, camera)
        self.World = World
        self.msnCam = msnCam
        self.camera= camera
        self.sceneManager = Mgr
        self.timer=0
        self.count=0
        self.bodies=[]
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
                   
                ent = self.sceneManager.createEntity( name, "ellipsoid.mesh" )
                node = self.sceneManager.getRootSceneNode().createChildSceneNode( name )
                node.attachObject( ent )
                
                ent.setMaterialName( "Simple/dirt01" )
                ent.setNormaliseNormals(True)

                  
                ## again, make the collision shape.
                ##col = OgreNewt.CollisionPrimitives.Cylinder(self.World, 1, 1)
                col = OgreNewt.Ellipsoid(self.World, Ogre.Vector3(1,1,1))
                
                ## then make the rigid body.
                body = OgreNewt.Body( self.World, col )
    
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
                self.bodies.append(body)
                self.timer = 0.2
                
        self.timer -= frameEvent.timeSinceLastFrame
        if (self.Keyboard.isKeyDown(OIS.KC_ESCAPE)):
            return False
        if (self.Keyboard.isKeyDown(OIS.KC_F3)):
            if self.Debug:
                self.Debug = False
            else:
                self.Debug = True
            self.basicframelistener.debug ( self.Debug )
    
        sf.FrameListener.frameStarted(self, frameEvent)
        return True        

        


if __name__ == '__main__':
    try:
        application = OgreNewtonApplication()
        application.go()
    except Ogre.Exception, e:
        print e
        print dir(e)
    
                
