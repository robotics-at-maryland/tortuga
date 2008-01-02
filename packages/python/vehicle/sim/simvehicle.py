# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  vehicle/sim/simvehicle.py

"""
    Contains the vehicle class, this implements the vehicle interface using the
sim module.
"""

# Makes everything much easier, all imports must work from the root
#from __future__ import absolute_import

# Stdlib imports
import logging

# Library Imports
import ogre.physics.OgreNewt as OgreNewt
import ogre.io.OIS as OIS

# Project imports
import event
import logloader

from ram.sim.util import Vector, Quat
from sim.simulation import Simulation
from sim.physics import buoyancyCallback
from vehicle.vehicle import VehicleFactory, IVehicle
from vehicle.device import DeviceFactory
from vehicle.sim.device import Thruster

event.add_event_types('CAM_CHANGE')

class Vehicle(IVehicle):
    """
    self.position is and Ogre.Vector3 that gives the position of the vehicle
    self.attitude is and Ogre.Quaternion that gives the orientation of the 
                            vehicle
    """
              
    def __init__(self, config):
        self._setup_logging(config.get('Logging', {'name' : 'SimVehicle',
                                                   'level': 'INFO'}))
        self.logger.info('* * * Beginning initialization')
        
        # Create simulation singleton
        Simulation(config['Simulation'])
        
        # Create vehicle and devices
        self._create_vehicle(config)
        devices = config['Devices']
        for device_name in devices:
            device_cfg = devices[device_name]
            device = DeviceFactory.create(device_cfg['type'], device_name, 
                                          device_cfg, self)
            setattr(self, device_name, device)
            
        self._setup_controller(config)
        self.logger.info('* * * Initialized')
        
        event.register_handlers('CAM_CHANGE', self._cam_follow)
        Simulation.get().input_sys.map_key_events('CAM_CHANGE', OIS.KC_F)
        self.following = False
        
    def __del__(self):
        self.logger.info('* * * Beginning shutdown')
        Simulation.delete()
        self.logger.info('* * * Shutdown complete')
        
    def _cam_follow(self):
        if self.following:
            event.send('CAM_INDEPENDENT')
            self.following = False
        else:
            event.send('CAM_FOLLOW', self.hull_node)
            self.following = True
        
    def update(self, time_since_last_update):
        
        # Update our public attributes
        self.attitude = self.hull_node.orientation
        self.position = self.hull_node.position
        return Simulation.get().update(time_since_last_update)
    
    def _setup_logging(self, config):
        self.logger = logloader.setup_logger(config, config)
        
    def _setup_controller(self, config):
        # Hook up input system to thrusters
        thruster_events = {
         'PORT_THRUST_UP' : OIS.KC_U,
         'PORT_THRUST_DOWN' : OIS.KC_J,
         'STARBOARD_THRUST_UP' : OIS.KC_I,
         'STARBOARD_THRUST_DOWN' : OIS.KC_K,
         'FORE_THRUST_UP' : OIS.KC_Y,
         'FORE_THRUST_DOWN' : OIS.KC_H,
         'AFT_THRUST_UP' : OIS.KC_O,
         'AFT_THRUST_DOWN' : OIS.KC_L}
        
        Simulation.get().input_sys.map_key_events(thruster_events)
        
        # Create Simple Keyboard Mouse controller
        #self.kmcontroller = control.DirectVehicleController(self)
        
    def _create_vehicle(self, config):
        scene_mgr = Simulation.get().graphics_sys.scene_manager
        world = Simulation.get().physics_sys.world
        
        # Load values from the config file
        size = Vector(config['size'])
        mass = config['mass']
        orientation = Quat(config['orientation'], axis_angle = True)
        position = Vector(config['position'])
        
                    
        hull = scene_mgr.createEntity('Hull', 'box.mesh')
        hull.setMaterialName("Simple/BumpyMetal")
        hull.setNormaliseNormals(True)
        self.hull_node = scene_mgr.getRootSceneNode().createChildSceneNode()
        self.hull_node.attachObject(hull)
        self.hull_node.setScale(size)
        self.hull_node.setPosition(position)
        
        col = OgreNewt.Box(world, size)
        hull_body = OgreNewt.Body(world, col)
        hull_body.attachToNode(self.hull_node)
        inertia = OgreNewt.CalcBoxSolid(mass, size)
        hull_body.setMassMatrix(mass, inertia)
        hull_body.setCustomForceAndTorqueCallback(
            thruster_force_callback, "")
        hull_body.setPositionOrientation(position, orientation)
        hull_body.setAutoFreeze(False)
        
        Simulation.get().physics_sys.bodies.append(hull_body)

# Register Simuldated Vehicle with Factory
VehicleFactory.register('Simulated', Vehicle)

def thruster_force_callback(body):
    mass, inertia = body.getMassMatrix()
    gravity = Vector((0, -9.8, 0))
    
    body.addForce(gravity * mass)
    body.addBouyancyForce(1000, 0.01, 0.01, gravity, buoyancyCallback, "")
    
    # Apply our thrust from the thrusters
    Thruster.apply_thruster_force(body)