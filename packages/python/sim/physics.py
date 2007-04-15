# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  sim/physics.py

"""
Wraps up the initialization and management of OgreNewt
"""

# Makes everything much easier, all imports must work from the root
#from __future__ import absolute_import

# Libraries Imports
import ogre.renderer.OGRE as Ogre
import ogre.physics.OgreNewt as OgreNewt

# Project Imports
import logloader
from core import FixedUpdater

class PhysicsSystem(FixedUpdater):
    """
    This handles everything need with the Physics system.
    """
    def __init__(self, config):
        
        self._setup_logging(config.get('Logging', {'name' : 'Physics',
                                                  'level': 'INFO'}))
        self.logger.info('* * * Beginning initialization')
        
        # The main OgreNewt object
        self.world = OgreNewt.World()
        # Here so we can clean up after outselves
        self.bodies = []
        
        # This super class runs the update at a fixed rate
        FixedUpdater.__init__(self, 1.0 / config.get('update_rate',60), 1.0)
   
        
        # Start up the debugger so that we can show debugging lines
        #OgreNewt.Debugger.getSingleton().init(graphics_sys.scene_manager)
        self.logger.info('* * * Initialized')
        
    def __del__(self):
        """
        You must delete this before the GraphicsSystem
        """
        self.logger.info('* * * Beginning shutdown')
        #OgreNewt.Debugger.getSingleton().deInit()
        
        del self.bodies
        del self.world
        self.logger.info('* * * Shutdown complete')
        
    def _update(self, time_since_last_update):        
        """
        Called at a set interval update the physics and there graphical 
        counter parts.  This cannot be running at the same time as update for   
        the GraphicsSystem.
        
        A return of false from here shuts down the application
        """
        self.world.update(self.update_interval)
        
    def _always_updated(self, time_since_last_update):
        #OgreNewt.Debugger.getSingleton().showLines(self.world)
        pass
        
    def _setup_logging(self, config):
        self.logger = logloader.setup_logger(config, config)
        
    
def gravityAndBouyancyCallback(me):
    mass, inertia = me.getMassMatrix()

    gravity = Ogre.Vector3(0, -9.8, 0) * mass
    me.addForce(gravity)

    # also don't forget buoyancy force.
    # just pass the acceleration due to gravity, not the force (accel * mass)! 
    me.addBouyancyForce(1000, 0.03, 0.03, Ogre.Vector3(0.0,-9.8,0.0), 
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