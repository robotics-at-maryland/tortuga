# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  vehicle/sim/vehicle.py

# Python Imports
import warnings

# Project Imports
import event
from devices import IDevice
from core import Component, implements
from module import IModule, Module, Component, ModuleManager

warnings.simplefilter('ignore', RuntimeWarning)
from ext.vehicle import Vehicle as _Vehicle # Import C++ Vehicle
from ext.core import ConfigNode as _ConfigNode
warnings.simplefilter('default', RuntimeWarning)

class Vehicle(_Vehicle, Component):
    implements(IModule)
    
    def __init__(self, config):
        # Create C++ super class
        _Vehicle.__init__(self, _ConfigNode.fromString(str(config)))
        
        # Module variabls
        self.name = config['name']
        self.type = type(self)
        self._depends = []
        self._running = False
        self._devices = {}
        
        self._config = config
        
        # Create the devices
        print 'Creating devices'
        self._create_devices()
        print 'Done'
        
        # Create module base class, send off events
        print 'Initing'    
        event.send('MODULE_CREATED', self)
        ModuleManager.get().register(self)
        print 'Done'
        
    def start(self):
        self.background(self._config['update_interval'])
        event.send('MODULE_START', self)    
        
    def pause(self):
        self.unbackground()
        self._running = False
        event.send('MODULE_PAUSE', self)
        
    def shutdown(self):
        self.pause()
        # Put more shutdown stuff here

        # Make sure to get rid of reference loops
        del self._devices
        
        event.send('MODULE_SHUTDOWN',self)

    def getDevice(self, name):
        return self._devices[name]

    def _addDevice(self, device):
        self._devices[device.getName()] = device
        self._cpp_addDevice(device)
        
    def _create_devices(self):
        device_nodes = self._config.get('Devices', None)

        # Iterate over nodes and create devices
        if device_nodes is not None:
            for name, config_node in device_nodes.iteritems():
                class_name = config_node['type']
                config_node['name'] = name
                
                # Grab our device class then create an instance
                device_class = Component.get_class(IDevice, class_name)

                # If we are in the 'ext' module we are a C++ class and need the
                # special config node wrapper
                if class_name.startswith('ext.'):
                    config_node = _ConfigNode.fromString(str(config_node))
                    device_class = device_class.construct
                    
                device = device_class(self, config_node)

                self._addDevice(device)
                # Add the device to the C++ side of things
#                self._cpp_addDevice(device)
#                # Store the device for python
 #               self._devices[name] = device
                
    

#import devices
#'''
#This is the vehicle abstraction. A vehicle has devices, things like thrusters and sensors.
#All access to the devices occurs through the vehicle. Functions that access devices are written 
#as accessors in the vehicle class (no direct access to devices from state machine)
#'''
#
#class main_vehicle:
#    def __init__(self,vc,mc,sc):
#        print "Creating a vehicle"
#        #Set the vision Communicator
#        self.vision_comm = vc
#        self.motor_comm = mc
#        self.sensor_comm = sc
#        
#        # Initialize the thruster objects
#        t1 = devices.Thruster(0.0,1)
#        t2 = devices.Thruster(0.0,2)
#        t3 = devices.Thruster(0.0,3)
#        t4 = devices.Thruster(0.0,4)
#        self.thrusters = [t1,t2,t3,t4]
#        
#        #Create a depth sensor object
#        self.depth_sensor = devices.DepthSensor(self.sensor_comm)
#    
#    #sets thruster values, as well as updating the motor controller with the command  
#    #thruster is an actual thruster object, not just an address number
#    def set_thruster_power(self,thruster,power):
#        thruster.power = power
#        self.motor_comm.set_power(thruster.address,thruster.power)
#    
#    #updates the motor communicator with the power setting for the thruster device.
#    #if for some reason the "code" thruster got a power setting that the motor communicator
#    #didn't send to the motor controllers, this function does the job
#    def update_thruster(self,thruster):
#	self.motor_comm.set_power(thruster.address,thruster.power)
#
#    #prints out the thruster powers in a readable format
#    def print_thruster_powers(self):
#        for thruster in self.thruster:
#            print "Thruster " + str(thruster.address) + ": " + str(thruster.power)
#            
#    #gets the depth from the depth sensor
#    def depth(self):
#        return self.depth_sensor.depth()
#        
#    #starts the vision system running along on a thread
#    def start_vision_system(self):
#        self.vision_comm.start_vision_thread()
#        return self.vision_comm.error
#    
#    #gets the data from the vision system
#    def get_vision_structure(self):
#        return self.vision_comm.get_data()
#    
#    #this is the vehicle shutdown routine. Put code in here that stops robot operation cleanly
#    def shutdown(self):
#        self.motor_comm.close(self.thrusters)
#    
