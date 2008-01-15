# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/motion.py

# Project Imports
import ext.core as core
import ext.control as control
import ext.vehicle as vehicle
import ext.math as math

class MotionManager(core.Subsystem):
    def __init__(self, config, deps):
        core.Subsystem.__init__(self, config.get('name', 'MotionManager'))
        self._motion = None
        
        self._controller = core.Subsystem.getSubsystemOfType(control.IController, deps)
        self._vehicle = core.Subsystem.getSubsystemOfType(vehicle.IVehicle, deps)
        self._eventHub = core.Subsystem.getSubsystemOfType(core.QueuedEventHub, deps)
            
    def setMotion(self, motion):
        if self._motion is not None:
            self._motion.stop()
            
        self._motion = motion
        self._motion.start(self._controller, self._vehicle, self._eventHub)    
        
    def background(self):
        pass
        
    def backgrounded(self):
        return True
        
    def unbackground(self, join = True):
        pass
        
    def update(self, timeSinceLastUpdate):
        pass
    
core.SubsystemMaker.registerSubsystem('MotionManager', MotionManager)
        
class Motion(core.EventPublisher):
    FINISHED = core.declareEventType('FINISHED')
    
    def __init__(self):
        core.EventPublisher.__init__(self)
        self._controller = None
        self._vehicle = None
        self._eventHub = None
    
    def start(self, controller, vehicle, eventHub):
        """
        Called by the motion manager to state main state variables
        
        Do not override this method directly.  Overrive _start instead.
        
        @type  controller: ext.control.IController 
        @param controller: The current controller of the vehicle
        
        @type  vehicle: ext.vehicle.IVehicle
        @param vehicle: The vehicle we are controlling
        
        @type  eventHub: ext.core.QueuedEventHub
        @param eventHub: The event hub to subscribe to events through 
        """
        self._controller = controller
        self._vehicle = vehicle
        self._eventHub =  eventHub
        
        self._start()
        
    def _start(self):
        raise Exception("You must implement this method")
        
    def stop(self):
        pass
    
class ChangeDepth(Motion):
    def __init__(self, desiredDepth, steps):
        """
        @type  desiredDepth: float
        @param desiredDepth: Depth you wish the sub to be at
        
        @type  steps: int
        @param steps: Number of increments you wish to change depth in    
        """
        Motion.__init__(self)
        
        self._steps = steps
        self._desiredDepth = desiredDepth
        self._conn = None
        
    def _start(self):
        # Register to recieve AT_DEPTH events
        self._conn = self._eventHub.subscribe(control.IController.AT_DEPTH,
                                              self._controller, self._atDepth)

        # Start changing depth
        self._nextDepth(self._vehicle.getDepth())
        
    def _nextDepth(self,currentVehicleDepth):
        """
        Descreses depth by one 'step' of the remaing depth change
        """
        depthDifference = self._desiredDepth - currentVehicleDepth 
        depthChange = depthDifference/self._steps
        self._controller.setDepth(currentVehicleDepth + depthChange)
        
    def _atDepth(self, event):
        """
        AT_DEPTH event handler, called when the vehicle reaches the command depth
        """
        self._steps -= 1
        currentVehicleDepth = event.number
        if not (self._steps == 0):
            self._nextDepth(currentVehicleDepth)
        else:
            self._finish()
    
    def _finish(self):
        """
        Finishes off the motion, disconnects events, and putlishes finish event
        """
        self.publish(Motion.FINISHED, core.Event())
        self._conn.disconnect()
        

