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
import OgreNewt
import logging

from vehicle import VehicleFactory, IVehicle, DeviceFactory
from vehicle.sim import core
from vehicle.sim.gui import GUISystem
from vehicle.sim.input import InputSystem
from vehicle.sim.physics import PhysicsSystem, buoyancyCallback
from vehicle.sim.graphics import GraphicsSystem
from vehicle.sim.core import Vector, Quat
from vehicle.sim.device import Thruster

class Vehicle(IVehicle):          
    def __init__(self, config):
        self.scene = None
        self.gui_sys = None
        self.input_sys = None
        self.physics_sys = None
        self.graphics_sys = None
        self.hull_node = None
        self.hull_body = None
        
        self.components = self._create_components(config)
        
        # load the scene
        self.scene = core.load_scene(config['Scenes'], self.graphics_sys, 
                                     self.physics_sys)
        # Create vehicle and devices
        self._create_vehicle(config)
        #for device in config['Devices']:
        #    DeviceFactory.create(device, [config[device], self])

        
    def __del__(self):
        del self.hull_node
        del self.hull_body
        del self.scene
        del self.gui_sys
        del self.input_sys
        del self.physics_sys
        del self.graphics_sys    
        
    def update(self, time_since_last_update):
        # Update all components, drop out if one returns false
        for component in self.components:
            if not component.update(time_since_last_update):
                return False
        return True
    
    def _create_components(self, config):
        self.graphics_sys = GraphicsSystem(config['Graphics'])
        self.physics_sys = PhysicsSystem(config['Physics'], self.graphics_sys)
        self.input_sys = InputSystem(self.graphics_sys)
        self.gui_sys = GUISystem(config['GUI'], self.graphics_sys, 
                                 self.input_sys)
        
        return [self.input_sys, self.physics_sys, self.graphics_sys]
        
    def _create_vehicle(self, config):
        scene_mgr = self.graphics_sys.scene_manager
        world = self.physics_sys.world
        
        # calculate the inertia based on box formula and mass
        size = Vector(config['size'])
        mass = config['mass']
        inertia = OgreNewt.CalcBoxSolid(mass, size)
                    
        hull = scene_mgr.createEntity('Hull', 'box.mesh')
        hull.setMaterialName("Simple/BumpyMetal")
        hull.setNormaliseNormals(True)
        self.hull_node = scene_mgr.getRootSceneNode().createChildSceneNode()
        self.hull_node.attachObject(hull)
        self.hull_node.setScale(size)
        
        
        col = OgreNewt.Box(world, size)
        self.hull_body = OgreNewt.Body(world, col)
        self.hull_body.attachToNode(self.hull_node)
        self.hull_body.setMassMatrix(mass, inertia)
        
        self.hull_body.setCustomForceAndTorqueCallback(thruster_force_callback, 
                                                       "")
        orientation = Quat(config['orientation'], axis_angle = True)
        self.hull_body.setPositionOrientation(Vector(config['position']), 
                                              orientation)
        

# Register Simuldated Vehicle with Factory
VehicleFactory.register('Simulated', Vehicle)

def thruster_force_callback(body):
    gravity = Vector((0, -9.8, 0))
    body.addForce(gravity)
    body.addBouyancyForce(100, 0.01, 0.01, gravity, buoyancyCallback, "")
    
    # Apply our thrust from the thrusters
    #Thruster.apply_thruster_force(body)