# 
#   OgreNewt library - connecting Ogre and Newton!
#   Demo07_CustomJoints - basic demo that shows a simple OgreNewt world, and how
#   to setup basic rigid bodies.
# 
## NOT YET WORKING

import Ogre
import OgreNewt
import OIS
import SampleFramework as sf
from MyCustomBallSocket import *
from BasicFrameListener import *     # a simple frame listener that updates physics as required..

class OgreNewtonApplication (sf.Application):
    def __init__ ( self):
        sf.Application.__init__(self)
        self.World = OgreNewt.World()
        self.EntityCount = 0
        self.bodies=[]

    def __del__ (self):
        ## delete the world when we're done.
        del self.World;
    
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
 
        floor = self.sceneManager.createEntity("Floor", "simple_terrain.mesh" )
        floornode = self.sceneManager.getRootSceneNode().createChildSceneNode( "FloorNode" )
        floornode.attachObject( floor )
        floor.setMaterialName( "Simple/BeachStones" )
        floor.setCastShadows( False )

        ## here's where we make a collision shape for the physics.  note that we use the same size as
        ## above.
        col = OgreNewt.TreeCollision(self.World,floornode, True)
        ## now we make a new rigid body based on this collision shape.
        body = OgreNewt.Body( self.World, col )
        ## we`re done with the collision shape, we can delete it now.
        del col
        
        body.attachToNode( floornode )
        body.setPositionOrientation( Ogre.Vector3(0.0,-20.0,0.0), Ogre.Quaternion.IDENTITY )
        
        ## make a simple rope.
        size= Ogre.Vector3(3,1.5,1.5)
        pos = Ogre.Vector3(0,3,0)
        orient = Ogre.Quaternion.IDENTITY
        parent=None
        if False: #### NOTE AJM -- DEMO NOT YET FINISHED !!!!
            for x in range(5):
                ## make the next box.
                child = self.makeSimpleBox(size, pos, orient)
            
                ## make the joint right between the bodies...
                if (parent):  # OgreNewt.BallAndSocket
                    joint = MyCustomBallSocket(child, parent, 
                                        pos-Ogre.Vector3(size.x/2,0,0), Ogre.Vector3.UNIT_X )
                else:
                    ## no parent, this is the first joint, so just pass None as the parent, to stick it to the "world"
                    joint = MyCustomBallSocket(child, None, 
                                        pos-Ogre.Vector3(size.x/2,0,0), Ogre.Vector3.UNIT_X )
                ## rememeber to make a copy
                self.bodies.append ( joint )
                ## offset pos a little more.
                pos += Ogre.Vector3(size.x, 0, 0)
            
                ## save the last body for the next loop!
                parent = child
        
        self.bodies.append(body)
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

        self.NewtonListener = BasicFrameListener( self.renderWindow, self.sceneManager, self.World, 60 )
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
    
    def frameStarted(self, frameEvent):
        ##sf.FrameListener.frameStarted(self, frameEvent)

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
                pos = self.msnCam.getWorldPosition()
                name = "Body"+str( self.count )
                self.count += 1
    
                ent = self.sceneManager.createEntity( name, "ellipsoid.mesh" )
                node = self.sceneManager.getRootSceneNode().createChildSceneNode( name + "Node" )
                node.attachObject( ent )
                
                node.setPosition(0.0, 0.0, 0.0)
                
                ent.setMaterialName( "Simple/dirt01" )
                ent.setNormaliseNormals(True)
    
                ## again, make the collision shape.
                col = OgreNewt.Ellipsoid(self.World, Ogre.Vector3(1, 1,1))
                ## then make the rigid body.    ## need to keep it arround
                body = OgreNewt.Body( self.World, col)
    
                ##no longer need the collision shape object
                del col

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
        return True        
    

if __name__ == '__main__':
    try:
        application = OgreNewtonApplication()
        application.go()
    except Ogre.Exception, e:
        print e
        print dir(e)
    
    