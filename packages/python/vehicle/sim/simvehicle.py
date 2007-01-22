# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  vehicle/sim/simvehicle.py

"""
    Contains the vehicle class, main object for the vehicle simulation, it
encapsulates all aspects of the GUI, and simulation elements away from the 
control code below.
"""

# Stdlib imports
import logging

# Library Imports
import OgreNewt
import OIS

# Project imports
import event
import control
import logloader

from vehicle import VehicleFactory, IVehicle, DeviceFactory
from vehicle.sim import core
from vehicle.sim.gui import GUISystem
from vehicle.sim.input import InputSystem
from vehicle.sim.physics import PhysicsSystem, buoyancyCallback
from vehicle.sim.graphics import GraphicsSystem, CameraController
from vehicle.sim.core import Vector, Quat
from vehicle.sim.device import Thruster

# Events
event.add_event_types('SIM_SHUTDOWN') # Called to shutdown the simulation

class Vehicle(IVehicle):          
    def __init__(self, config):
        self.scene = None
        self.gui_sys = None
        self.input_sys = None
        self.physics_sys = None
        self.graphics_sys = None
        
        self._setup_logging(config.get('Logging', {'name' : 'SimVehicle',
                                                   'level': 'INFO'}))
        self.logger.info('* * * Beginning initialization')
        
        self.components = self._create_components(config)
        
        # load the scene
        self.scene = core.load_scene(config['Scenes'], self.graphics_sys, 
                                     self.physics_sys)
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

        event.register_handlers('SIM_SHUTDOWN', self._shutdown)
        self.run = True
        
    def _shutdown(self):
        self.run = False
        
    def __del__(self):
        self.logger.info('* * * Beginning shutdown')
        
        del self.components
        del self.scene
        del self.gui_sys
        del self.input_sys
        del self.physics_sys
        del self.graphics_sys

        self.logger.info('* * * Shutdown complete')
        
    def update(self, time_since_last_update):
        if self.run:
            # Update all components, drop out if one returns false
            for component in self.components:
                component.update(time_since_last_update)
                
            self.attitude = self.hull_node.orientation
            self.position = self.hull_node.position
   
        return self.run
    
    def _setup_logging(self, config):
        self.logger = logloader.setup_logger(config, config)
    
    def _create_components(self, config):
        self.graphics_sys = GraphicsSystem(config.get('Graphics',{}))
        self.physics_sys = PhysicsSystem(config.get('Physics',{}), 
                                         self.graphics_sys)
        self.input_sys = InputSystem(self.graphics_sys, config.get('Input',{}))
        self.gui_sys = GUISystem(config['GUI'], self.graphics_sys, 
                                 self.input_sys)
        
        return [self.input_sys, self.physics_sys, self.graphics_sys]
        
    def _create_vehicle(self, config):
        scene_mgr = self.graphics_sys.scene_manager
        world = self.physics_sys.world
        
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
        
        self.physics_sys.bodies.append(hull_body)

# Register Simuldated Vehicle with Factory
VehicleFactory.register('Simulated', Vehicle)

def thruster_force_callback(body):
    mass, inertia = body.getMassMatrix()
    gravity = Vector((0, -9.8, 0))
    
    body.addForce(gravity * mass)
    body.addBouyancyForce(1000, 0.01, 0.01, gravity, buoyancyCallback, "")
    
    # Apply our thrust from the thrusters
    Thruster.apply_thruster_force(body)