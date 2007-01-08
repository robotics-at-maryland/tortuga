# /*
#   OgreNewt library - connecting Ogre and Newton!

#   Demo02_Joints - basic demo that shows how to connect rigid bodies via joints.
# */
import exceptions
import Ogre
import OgreNewt
import OIS
import SampleFramework as sf
from BasicFrameListener import *     # a simple frame listener that updates physics as required..
import SimpleVehicle

class OgreNewtonApplication (sf.Application):
    def __init__ ( self):
        sf.Application.__init__(self)
        self.World = OgreNewt.World()
        self.EntityCount = 0
        self.bodies=[]
        self.Car = None
        sf.Application.debugText = "I, J, K, L move/steer the car. ESC to exit"


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
        
    	## floor object! this time we'll scale it slightly to make it more vehicle-friendly :P
    	size=Ogre.Vector3(2.0,0.5,2.0)
    	floor = self.sceneManager.createEntity("Floor", "simple_terrain.mesh" )
    	floornode = self.sceneManager.getRootSceneNode().createChildSceneNode( "FloorNode" )
    	floornode.attachObject( floor )
    	floor.setMaterialName( "Simple/BeachStones" )
    	floornode.setScale(size)
    
    	floor.setCastShadows( False )
    
    	##Ogre.Vector3 siz(100.0, 10.0, 100.0);
    	col = OgreNewt.TreeCollision( self.World, floornode, False )
    	bod = OgreNewt.Body( self.World, col )
    	del col
    	
    	bod.attachToNode( floornode )
    	bod.setPositionOrientation( Ogre.Vector3(0.0,-2.0,0.0), Ogre.Quaternion.IDENTITY )
        
    	self.bodies.append(bod)
    
    
    	## here's where we make the simple vehicle.  everything is taken care of in the constuctor.
    	self.Car = SimpleVehicle.SimpleVehicle( self.sceneManager, self.World, 
    	                        Ogre.Vector3(0,-0.5,0), Ogre.Quaternion.IDENTITY )

        ## position camera
        self.msnCam = self.sceneManager.getRootSceneNode().createChildSceneNode()
        self.msnCam.attachObject( self.camera )
        self.camera.setPosition(0.0, 0.0, 0.0)
        self.msnCam.setPosition( 0.0, 1.0, 20.0)
    
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
        self.frameListener = OgreNewtonFrameListener( self.renderWindow, self.camera, 
                                    self.sceneManager, self.World, self.msnCam, self.Car )
        self.root.addFrameListener(self.frameListener)

        ## this is a basic frame listener included with OgreNewt that does nothing but update the
        ## physics at a set framerate for you.  complex project will want more control, but this
        ## works for simple demos like this.  feel free to look at the source to see how it works.
        self.NewtonListener = BasicFrameListener( self.renderWindow, self.sceneManager, self.World, 120 )
        self.root.addFrameListener(self.NewtonListener)

        
    
    
class OgreNewtonFrameListener(sf.FrameListener):
    def __init__(self, renderWindow, camera, Mgr, World, msnCam, car):

        sf.FrameListener.__init__(self, renderWindow, camera)
        self.World = World
        self.msnCam = msnCam
        self.camera= camera
        self.sceneManager = Mgr
        self.timer=0
        self.count=0
        self.bodies=[]
        self.Car=car
    
    def frameStarted(self, frameEvent):
        sf.FrameListener.frameStarted(self, frameEvent)

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
                pos = self.camera.getWorldPosition()
    
                name = "Body"+str( self.count )
                print "created ", name
                self.count += 1
                   
                ent = self.sceneManager.createEntity( name, "ellipsoid.mesh" )
                node = self.sceneManager.getRootSceneNode().createChildSceneNode( name )
                node.attachObject( ent )
                
                node.setPosition(0.0, 0.0, 0.0)
                
                ent.setMaterialName( "Simple/dirt01" )
                #ent.setNormaliseNormals(True)
                  
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
     	## ---------------------------------------------------------
    	## -- VEHICLE CONTORLS
    	## ---------------------------------------------------------
    	torque = 0.0
    	steering = Ogre.Degree(0.0)
    
    	if (self.Keyboard.isKeyDown(OIS.KC_I)):
    		torque += 800.0
    
    	if (self.Keyboard.isKeyDown(OIS.KC_K)):
    		torque -= 800.0
    
    	if (self.Keyboard.isKeyDown(OIS.KC_J)):
    		steering = Ogre.Degree(d=30)
    
    	if (self.Keyboard.isKeyDown(OIS.KC_L)):
    		steering = Ogre.Degree(d=-30)
    
    	##update the vehicle!
     	self.Car.setTorqueSteering( torque, steering )
    
    	if ((self.Keyboard.isKeyDown(OIS.KC_R)) and  ( not self.R)):
    		self.R = True
    		## rebuild the vehicle
    		if (self.Car):
    			del self.Car
    			self.Car = SimpleVehicle.SimpleVehicle( self.sceneManager, self.World, 
    			                                Ogre.Vector3(0,Ogre.Math.UnitRandom() * 10.0,0), 
    			                                Ogre.Quaternion.IDENTITY )
    	if ( not self.Keyboard.isKeyDown( OIS.KC_R )):
    	    self.R = False
        
        if (self.Keyboard.isKeyDown(OIS.KC_ESCAPE)):
            if self.Car:
                del self.Car
            return False
        return True        

if __name__ == '__main__':
    try:
        application = OgreNewtonApplication()
        application.go()
    except Ogre.Exception, e:
        print e
        print dir(e)
    except exceptions.RuntimeError, e:
        print "Runtime error:", e
    except exceptions.TypeError, e:
        print "Type error:", e
    except exceptions.AttributeError, e:
        print "Attribute error:", e
    except exceptions.NameError, e:
        print "Name error:", e
    except Exception,inst:
        print "EException"
        print type(inst)     # the exception instance
        print inst.args      # arguments stored in .args
        print inst
    except exceptions.ValueError,e:
        print "ValueError",e
    except :
        print "Unexpected error:", sys.exc_info()[0]
   
                