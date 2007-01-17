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
import OIS
import logging

import event
from vehicle import VehicleFactory, IVehicle, DeviceFactory
from vehicle.sim import core
from vehicle.sim.gui import GUISystem
from vehicle.sim.input import InputSystem, KeyStateObserver
from vehicle.sim.physics import PhysicsSystem, buoyancyCallback
from vehicle.sim.graphics import GraphicsSystem, CameraController
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
        devices = config['Devices']
        for device_name in devices:
            device_cfg = devices[device_name]
            device = DeviceFactory.create(device_cfg['type'], device_name, 
                                          device_cfg, self)
            setattr(self, device_name, device)
            
        # Create Simple Keyboard Mouse controller
        self.kmcontroller = VehicleKeyMouseController(self)

        
    def __del__(self):
        del self.kmcontroller
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
            
        self.kmcontroller.update()
        self.attitude = self.hull_node.orientation
        self.position = self.hull_node.position
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
        self.hull_body = OgreNewt.Body(world, col)
        self.hull_body.attachToNode(self.hull_node)
        inertia = OgreNewt.CalcBoxSolid(mass, size)
        self.hull_body.setMassMatrix(mass, inertia)
        self.hull_body.setCustomForceAndTorqueCallback(
            thruster_force_callback, "")
        self.hull_body.setPositionOrientation(position, orientation)
        self.hull_body.setAutoFreeze(False)
        
# Register Simuldated Vehicle with Factory
VehicleFactory.register('Simulated', Vehicle)

def thruster_force_callback(body):
    mass, inertia = body.getMassMatrix()
    gravity = Vector((0, -9.8, 0))
    
    body.addForce(gravity * mass)
    body.addBouyancyForce(1000, 0.01, 0.01, gravity, buoyancyCallback, "")
    
    # Apply our thrust from the thrusters
    Thruster.apply_thruster_force(body)
    
class VehicleKeyMouseController(object):
    """
    This listens to key events and will move the vehicle accordingly
    """
    def __init__(self, vehicle):
        self.vehicle = vehicle
        
        self.handler_map = {
            'KEY_PRESSED': self._key_pressed,
            'KEY_RELEASED': self._key_released }
        
        event.register_handlers(self.handler_map)
        
        # This sets up automatic setting of the key down properties
        watched_keys = [OIS.KC_I, OIS.KC_K, OIS.KC_U, OIS.KC_J,
                        OIS.KC_Y, OIS.KC_H, OIS.KC_O, OIS.KC_L]
        self.key_observer = KeyStateObserver(self, watched_keys)
    
    def __del__(self):
        # Make sure to remove event handlers so they are called after the 
        # object is gone
        event.remove_handlers(self.handler_map)
    
    def update(self):
        #if self.up_i:
        #    self.vehicle.
        #if self.down_m:
        #    self.camera_node.translate(trans * -1.0)
        if self.i_key:
            self.vehicle.PortThruster.power += 0.005
            #print self.vehicle.PortThruster.power
        if self.k_key:
            self.vehicle.PortThruster.power -= 0.005
            #print self.vehicle.PortThruster.power
        if self.u_key:
            self.vehicle.StarboardThruster.power += 0.005
            #print self.vehicle.StarboardThruster.power
        if self.j_key:
            self.vehicle.StarboardThruster.power -= 0.005
            #print self.vehicle.StarboardThruster.power
        if self.y_key:
            self.vehicle.ForeThruster.power += 0.005
            #print self.vehicle.PortThruster.power
        if self.h_key:
            self.vehicle.ForeThruster.power -= 0.005
            #print self.vehicle.PortThruster.power
        if self.o_key:
            self.vehicle.AftThruster.power += 0.005
            #print self.vehicle.StarboardThruster.power
        if self.l_key:
            self.vehicle.AftThruster.power -= 0.005
            #print self.vehicle.StarboardThruster.power
            
        #print 'P: %3d S: %3d F: %3d  A: %3d' % \
        #    (self.vehicle.PortThruster.power * 100,
        #     self.vehicle.StarboardThruster.power * 100,
        #     self.vehicle.ForeThruster.power * 100,
        #     self.vehicle.AftThruster.power * 100)

    def _key_pressed(self, key_event):
        # Update the state of *_key properties  
        self.key_observer.key_pressed(key_event)

    def _key_released( self, key_event ):
        # Update the state of *_key properties
        self.key_observer.key_released(key_event)