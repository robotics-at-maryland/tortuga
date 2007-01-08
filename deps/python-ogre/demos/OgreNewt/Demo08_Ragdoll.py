# /*
#   OgreNewt library - connecting Ogre and Newton!

#   Demo08 Ragdoll
# */

### DEMO NOT YET WORKING......

import Ogre
import OgreNewt
import OIS
import SampleFramework as sf
from BasicFrameListener import *     # a simple frame listener that updates physics as required..
from ragdoll import *

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
    
    	##Ogre.Vector3 siz(100.0, 10.0, 100.0)
    	col = OgreNewt.TreeCollision( self.World, floornode, True )
    	bod = OgreNewt.Body( self.World, col )
    	del col
	       
        bod.attachToNode( floornode )
        bod.setPositionOrientation( Ogre.Vector3(0.0,-4.0,0.0), Ogre.Quaternion.IDENTITY )
        self.bodies.append( bod )
        
        ## position camera
        self.msnCam = self.sceneManager.getRootSceneNode().createChildSceneNode()
        self.msnCam.attachObject( self.camera )
        self.camera.setPosition(0.0, 0.0, 0.0)
        self.msnCam.setPosition( 0.0, -2.0, 10.0)
    	self.camera.setNearClipDistance( 0.01 )

        ##make a light
        light = self.sceneManager.createLight( "Light1" )
        light.setType( Ogre.Light.LT_POINT )
        light.setPosition( Ogre.Vector3(0.0, 100.0, 100.0) )


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
        
        ##############################
    	## setup the ragdoll model here.
    	self.RagEntity = self.sceneManager.createEntity( "RAGDOLL_ENTITY", "zombie.mesh" )
    	self.RagNode = self.sceneManager.getRootSceneNode().createChildSceneNode()
    	self.RagNode.attachObject( self.RagEntity )
    	self.RagEntity.getAnimationState( "LOOP" ).setLoop( True )
    	self.RagEntity.getAnimationState( "LOOP" ).setEnabled( True )
    	self.RagNode.setPosition( 0.0, -4.0, 0.0 )
    
    	self.Ragdoll = None
    
    	self.K1 = self.K2 = False


    
    def frameStarted(self, frameEvent):
        
        ## in this frame listener we control the camera movement, and allow the user to "shoot" cylinders
        ## by pressing the space bar.  first the camera movement...
        quat = self.msnCam.getOrientation()
    
    	vec = Ogre.Vector3(0.0,0.0,-3 * frameEvent.timeSinceLastFrame)
    	trans = quat * vec

    	vec = Ogre.Vector3(3*frameEvent.timeSinceLastFrame,0.0,0.0)
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
        
    	######################################################
    	## RAGDOLL CODE
    	if (self.Ragdoll):
    		## ragdoll exists, so pressing "R" Key will remove ragdoll, and reset the entity.
    		if ((self.Keyboard.isKeyDown(OIS.KC_1) and ( not self.K1)) or
    		             (self.Keyboard.isKeyDown( OIS.KC_2 ) and (not self.K2) )):
    			self.K1 = True
    			self.K2 = True
    
    			## remove the ragdoll.
    			del self.Ragdoll
    			self.Ragdoll = None
    
    			self.RagNode.setPosition( Ogre.Vector3(0.0, -4.0, 0.0) )
    			self.RagNode.setOrientation( Ogre.Quaternion.IDENTITY )
    
    			self.RagEntity.getAnimationState( "LOOP" ).setEnabled( True )
    			self.RagEntity.getAnimationState( "LOOP" ).setTimePosition( 0.0 )
    	else:
    		## no ragdoll, so "R" key spawns the ragdoll. otherwise, we add time to the animation.
    		self.RagEntity.getAnimationState( "LOOP" ).addTime( frameEvent.timeSinceLastFrame )
    
    		if (self.Keyboard.isKeyDown(OIS.KC_1 ) and ( not self.K1)):
    			self.K1 = True
    
    			## spawn the ragdoll.  this is the version that is made up of simple primitives.
    			self.Ragdoll = RagDoll( "../media/models/zombie_rag_primitives.xml", self.World, self.RagNode )
    			self.RagEntity.getAnimationState( "LOOP" ).setEnabled( False )
    		elif (self.Keyboard.isKeyDown(OIS.KC_2 ) and ( not self.K2)):
    			self.K2 = True
    
    			## spawn the ragdoll.  this is the version that uses auto-hull generation for very accurate collision shapes.
    			self.Ragdoll = RagDoll( "../media/models/zombie_rag_hull.xml", self.World, self.RagNode )
    			self.RagEntity.getAnimationState( "LOOP" ).setEnabled( False )

    	if ( not self.Keyboard.isKeyDown(OIS.KC_1 )): 
    	    self.K1 = False
    	if ( not self.Keyboard.isKeyDown(OIS.KC_2 )):
    	    self.K2 = False
    	if (self.Keyboard.isKeyDown(OIS.KC_F3 )): 
    		self.RagEntity.setVisible( False )
    	else: 
    		self.RagEntity.setVisible( True ) 
        
        
        if (self.Keyboard.isKeyDown(OIS.KC_ESCAPE)):
            return False
        sf.FrameListener.frameStarted(self, frameEvent)
        return True        

        


if __name__ == '__main__':
    try:
        application = OgreNewtonApplication()
        application.go()
    except Ogre.Exception, e:
        print e
        print dir(e)
    
                