import Ogre
import OgreNewt

class PhysicsSystem(object):
    """
    This handles everything need with the Physics system.
    """
    def __init__(self, config, graphics_sys):
        self.graphics_sys = graphics_sys
        
        # The main OgreNewt object
        self.world = OgreNewt.World()
        # Here so we can clean up after outselves
        self.bodies = []
        
        self.update_interval = (1.0 / config['update_rate'])
        self.elapsed = 0.0;
        
        # Start up the debugger so that we can show debugging lines
        OgreNewt.Debugger.getSingleton().init(graphics_sys.scene_manager)
        
    def __del__(self):
        """
        You must delete this before the GraphicsSystem
        """
        OgreNewt.Debugger.getSingleton().deInit()
        
        del self.bodies
        del self.world
        
    def update(self, time_since_last_update):
        """
        Called at a set interval update the physics and there graphical 
        counter parts.  This cannot be running at the same time as update for   
        the GraphicsSystem.
        
        A return of false from here shuts down the application
        """
        self.elapsed += time_since_last_update
  
        if ((self.elapsed > self.update_interval) and (self.elapsed < (1.0)) ):
            while (self.elapsed > self.update_interval):
                self.world.update(self.update_interval)
                self.elapsed -= self.update_interval
        else:
            if (self.elapsed < self.update_interval):
                # not enough time has passed this loop, so ignore for now.
                pass
            else:
                self.world.update(self.elapsed)
                # reset the elapsed time so we don't become "eternally behind"
                self.elapsed = 0.0
                
        return True
    
    def makeSimpleBox(self, name, size, pos, orient):
        """
        Convience function, creates a box of the give size where the user wants
        it
        """
        scene_mgr = self.graphics_sys.scene_manager
        
        ## base mass on the size of the object.
        mass = size.x * size.y * size.z * 0.5
            
        ## calculate the inertia based on box formula and mass
        inertia = OgreNewt.CalcBoxSolid(mass, size)
                    
        box1 = scene_mgr.createEntity( name, "box.mesh" )
        box1node = scene_mgr.getRootSceneNode().createChildSceneNode()
        box1node.attachObject(box1)
        box1node.setScale(size)
        box1.setNormaliseNormals(True)
        
        col = OgreNewt.Box( self.world, size )
        bod = OgreNewt.Body( self.world, col )
        #del col
                    
        bod.attachToNode( box1node )
        
        bod.setMassMatrix( mass, inertia )
        bod.setCustomForceAndTorqueCallback( gravityAndBouyancyCallback, "")
        
        box1.setMaterialName("Simple/BumpyMetal")
    
        bod.setPositionOrientation(pos, orient)
    
        return bod
    
def gravityAndBouyancyCallback(me):
    mass, inertia = me.getMassMatrix()

    gravity = Ogre.Vector3(0,-9.8,0) * mass
    me.addForce(gravity)

    # also don't forget buoyancy force.
    # just pass the acceleration due to gravity, not the force (accel * mass)! 
    me.addBouyancyForce(0.7, 0.01, 0.01, Ogre.Vector3(0.0,-9.8,0.0), 
                        buoyancyCallback, "")
    
def buoyancyCallback(colID, me, orient, pos, plane):
    """
    Here we need to create an Ogre::Plane object representing the surface of 
    the liquid.  In our case, we're just assuming a completely flat plane of 
    liquid, however you could use this function to retrieve the plane
    equation for an animated sea, etc.
    """
    plane1 = Ogre.Plane( Ogre.Vector3(0,1,0), Ogre.Vector3(0,0,0) )
    
    # we need to copy the normals and 'd' to the plane we were passed...
    plane.normal = plane1.normal
    plane.d = plane1.d
   
    return True
              