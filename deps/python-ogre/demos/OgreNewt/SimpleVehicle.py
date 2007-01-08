import Ogre
import OgreNewt
EntityCount = 0
class SimpleVehicle (OgreNewt.Vehicle):

    ## SimpleVehicle constructor .  this creates and sets up the entire vehicle!
    def __init__( self, mgr,  world,  position,  orient) : 
        OgreNewt.Vehicle.__init__(self)
        ## save the scene manager
        self.sceneManager = mgr
        self.World = world
        self.Torque=0
        self.Steering =Ogre.Degree(0)
        ## first thing we need to do is create the rigid body for the main chassis.
        size = Ogre.Vector3 (5,1.2,2)
        self.bodies=[]
        self.tires=[]
        self.chassis = self.makeSimpleBox(size, position, orient)
        ##now that we have defined the chassis, we can call the "init()" function.  this is a helper function that
        ## simply sets up some internal wiring of the vehicle class that makes eveything work :)  it also calls the virtual
        ## function "setup" to finish building the vehicle.
        ## you pass this function the body to be used as the main chassis, and the up direction of the world (for suspension purposes).
        self.init( self.chassis, Ogre.Vector3(0,1,0) )
        ## the above function calls our "setup" function, which takes care of the rest of the vehicle setup.
        
    
    def makeSimpleBox( self, size, pos,  orient ):
        global EntityCount
        ## base mass on the size of the object.
        mass = size.x * size.y * size.z * 100.0
            
        ## calculate the inertia based on box formula and mass
        inertia = OgreNewt.CalcBoxSolid( mass, size )
    
        box1 = self.sceneManager.createEntity( "Entity"+str(EntityCount), "box.mesh" )
        EntityCount += 1
        box1node = self.sceneManager.getRootSceneNode().createChildSceneNode()
        box1node.attachObject( box1 )
        box1node.setScale( size )
    
        col = OgreNewt.Box( self.World, size )
        bod = OgreNewt.Body( self.World, col )
        del col
                    
        bod.attachToNode( box1node )
        bod.setMassMatrix( mass, inertia )
        bod.setStandardForceCallback()
    
        box1.setMaterialName( "Simple/BumpyMetal" )
        box1.setNormaliseNormals(True)
    
        bod.setPositionOrientation( pos, orient )
    
        return bod


    ## virtual function - setup().  this actually builds the tires, etc.
    def setup(self):
        global EntityCount
        ## okay, we have the main chassis all setup.  let's do a few things to it:
        self.chassis.setStandardForceCallback()
              
        ## we don't want the vehicle to freeze, because we'll be unable to control it.
        self.chassis.setAutoFreeze(0)
        
        ## okay, let's add tires!
        ## all offsets here are in local space of the vehicle.
        offset = Ogre.Vector3(1.8,-1.6,0.87)
        tireNum = 0
        for x in range (-1, 2, 2):
            for z in range (-1, 2, 2):
                 ## okay, let's create the tire itself.  we'll use the OgreNewt.Vehicle.Tire class for this.  most of the
                ## parameters are self-explanatory... try changing some of them to see what happens.
                tireorient = Ogre.Quaternion(Ogre.Degree(0), Ogre.Vector3.UNIT_Y)
                tirepos = offset * Ogre.Vector3(x,0.5,z)
                pin= Ogre.Vector3(0,0,x)
                mass = 15.0
                width = 0.3
                radius = 0.5
                susShock = 30.0
                susSpring = 200.0
                susLength = 1.2
                steering = True
    
                ## first, load the visual mesh that represents the tire.
                ent = self.sceneManager.createEntity("Tire"+Ogre.StringConverter.toString(EntityCount),"wheel.mesh") # "ellipsoid.mesh") #"wheel.mesh")
                EntityCount+=1
                ## make a scene node for the tire.
                node = self.sceneManager.getRootSceneNode().createChildSceneNode()
                node.attachObject( ent )
                node.setScale( Ogre.Vector3(radius, radius, width) )
                if (x > 0):
                    steering = True
                else:
                    steering = False
    
                ## create the actual tire!
                tire = self.SimpleTire(self, tireorient, tirepos, pin, mass, width, radius,
                    susShock, susSpring, susLength, 0, steering)
                    
                ## attach the tire to the node.
                tire.attachToNode( node )
                self.tires.append(tire)
                
         
    def setTorqueSteering( self, torque,  steering):
	    self.Torque = torque
	    self.Steering = steering
    
    def __del__(self):
        print "IN __DEL__ SIMPLEVEC"
        return
        ## delete tire objects.
        for tire in self.tires:
            tire.__del__()
            del tire
        ## finally, destroy entity and node from chassis.
#         ent = self.chassis.getOgreNode().getAttachedObject(0) ## causes no RTTI ???
#         self.chassis.getOgreNode().detachAllObjects()
#         self.chassis.getOgreNode().getCreator().destroyEntity( ent )
#         self.chassis.getOgreNode().getParentSceneNode().removeAndDestroyChild( self.chassis.getOgreNode().getName() )
        print "DONE __DEL__ SIMPLEVEC"
	    
    ## This is the important callback, which is the meat of controlling the vehicle.
    def userCallback(self):
        ## foop through wheels, adding torque and steering, and updating their positions.
        ## problem here as the original source casts the tire returned as a simpletire
        ## however getFirstTire returns and OgreNewt tire so we lose the 'steering'
        for tire in self.tires:
            if (tire.SteeringTire):
                tire.setSteeringAngle( self.Steering )
            else:
                tire.setTorque( self.Torque )
            ## finally, this command updates the location of the visual mesh.
            tire.updateNode()
            
            
    class SimpleTire (OgreNewt.Vehicle.Tire):
            def __init__(self, vehicle, localorient,  localpos,pin, 
                                mass,  width,  radius, susShock,  susSpring,  susLength, ColID,  steer ):
                OgreNewt.Vehicle.Tire.__init__(self, vehicle, localorient, localpos, pin, 
                        mass, width, radius, susShock, susSpring, susLength, ColID )
                self.SteeringTire = steer;

            def __del__(self):
                ## destroy entity, and scene node.

                print "IN __DEL__ for simpletire"
                return
                print "DONE __DEL__ SIMPLETIRE"
#                 ent = self.node.getAttachedObject(0)
#                 self.node.detachAllObjects()
#                 self.node.getCreator().destroyEntity( ent )
#                 self.node.getParentSceneNode().removeAndDestroyChild( self.node.getName() )
 