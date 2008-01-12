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

def getSubsystemOfType(_type, deps):
    for d in deps:
        # Simple check first
        if isinstance(d, _type):
            return d
        
        # Hacky cast check second
        result = _type.castTo(d)
        if result is not None:
            return result

class MotionManager(core.Subsystem):
    def __init__(self, config, deps):
        core.Subsystem.__init__(self, config.get('name', 'MotionManager'))
        self._motion = None
        
        self._controller = getSubsystemOfType(control.IController, deps)
        self._vehicle = getSubsystemOfType(vehicle.IVehicle, deps)
            
    def setMotion(self, motion):
        if self._motion is not None:
            self._motion.stop()
            
        self._motion = motion
        self._motion.start(self._controller, self._vehicle)    
        
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
    FINISHED = 'FINISHED'
    
    def __init__(self):
        core.EventPublisher.__init__(self)
        self._controller = None
        self._vehicle = None
    
    def start(self, controller, vehicle):
        self._controller = controller
        self._vehicle = vehicle
        
    def stop(self):
        pass
    
class ChangeDepth(Motion):
    def __init__(self, desiredDepth, steps):
        Motion.__init__(self)
        
        self._steps = steps
        self._desiredDepth = desiredDepth
        self._conn = None
        
    def start(self, controller, vehicle):
        Motion.start(self, controller, vehicle)
        
        # Register to recieve AT_DEPTH events
        self._conn = controller.subscribe(control.IController.AT_DEPTH, 
                                         self._atDepth)

        # Start changing depth
        self._nextDepth(vehicle.getDepth())
        
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
        

