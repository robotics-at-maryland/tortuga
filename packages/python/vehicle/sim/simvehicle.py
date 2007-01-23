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
import OgreNewt
import OIS

# Project imports
import event
import control
import logloader

from sim.util import Vector, Quat
from sim.simulation import Simulation
from sim.physics import buoyancyCallback
from vehicle.vehicle import VehicleFactory, IVehicle
from vehicle.device import DeviceFactory
from vehicle.sim.device import Thruster

class Vehicle(IVehicle):          
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
            
        # Create Simple Keyboard Mouse controller
        self.kmcontroller = control.DirectVehicleController(self)
        self.logger.info('* * * Initialized')
        
    def __del__(self):
        self.logger.info('* * * Beginning shutdown')
        Simulation.delete()
        self.logger.info('* * * Shutdown complete')
        
    def update(self, time_since_last_update):
        
        # Update our public attributes
        self.attitude = self.hull_node.orientation
        self.position = self.hull_node.position
        return Simulation.get().update(time_since_last_update)
    
    def _setup_logging(self, config):
        self.logger = logloader.setup_logger(config, config)
        
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