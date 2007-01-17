# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  vehicle/sim/physics.py

"""
Wraps up the initialization and management of OgreNewt
"""

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
        #OgreNewt.Debugger.getSingleton().init(graphics_sys.scene_manager)
        
    def __del__(self):
        """
        You must delete this before the GraphicsSystem
        """
        #OgreNewt.Debugger.getSingleton().deInit()
        
        del self.bodies
        del self.world
        
    def update(self, time_since_last_update):
        """
        Called at a set interval update the physics and there graphical 
        counter parts.  This cannot be running at the same time as update for   
        the GraphicsSystem.
        
        A return of false from here shuts down the application
        """
        #OgreNewt.Debugger.getSingleton().showLines(self.world)
        
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
    
def gravityAndBouyancyCallback(me):
    mass, inertia = me.getMassMatrix()

    gravity = Ogre.Vector3(0, -9.8, 0) * mass
    me.addForce(gravity)

    # also don't forget buoyancy force.
    # just pass the acceleration due to gravity, not the force (accel * mass)! 
    me.addBouyancyForce(1000, 0.01, 0.01, Ogre.Vector3(0.0,-9.8,0.0), 
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
    
    # pos = Ogre.Vector3(0,0,0)
   
    return True              