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
        core.Subsystem.__init__(self, config.get('name', 'MotionManager'), deps)
        self._motion = None
        
        self._controller = core.Subsystem.getSubsystemOfType(control.IController, 
                                                             deps, 
                                                             nonNone = True)
        
        self._vehicle = core.Subsystem.getSubsystemOfType(vehicle.IVehicle, 
                                                          deps, 
                                                          nonNone = True)
        
        self._qeventHub = core.Subsystem.getSubsystemOfType(core.QueuedEventHub, 
                                                           deps,
                                                           nonNone = True)
        
        self._eventHub = core.Subsystem.getSubsystemOfExactType(
            core.EventHub, deps, nonNone = True)
            
    def setMotion(self, motion):
        self.stopCurrentMotion()
            
        eventPublisher = core.EventPublisher(self._eventHub)
        self._motion = motion
        self._motion.start(self._controller, self._vehicle, self._qeventHub,
                           eventPublisher)

    def stopCurrentMotion(self):
        if self._motion is not None:
            self._motion.stop()        
        
    def background(self):
        pass
        
    def backgrounded(self):
        return True
        
    def unbackground(self, join = True):
        pass
        
    def update(self, timeSinceLastUpdate):
        pass
    
core.SubsystemMaker.registerSubsystem('MotionManager', MotionManager)
        
class Motion(object):
    FINISHED = core.declareEventType('FINISHED')
    
    def __init__(self):
        self._eventPublisher = None
        self._controller = None
        self._vehicle = None
        self._eventHub = None
    
    def start(self, controller, vehicle, eventHub, eventPublisher):
        """
        Called by the motion manager to state main state variables
        
        DO NOT OVERRIED this method directly.  Overrive _start instead.
        
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
        self._eventPublisher = eventPublisher
        
        # Set up the publish method to be seemless and easy
        self.publish = self._eventPublisher.publish
        
        self._start()
        
    def _start(self):
        raise Exception("You must implement this method")
    
    def _finish(self):
        """
        Raises, motion finished event.
        """
        self.publish(Motion.FINISHED, core.Event())
        
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
        Motion._finish(self)
        self._conn.disconnect()

    def stop(self):
        self._conn.disconnect()
        
class ChangeHeading(Motion):
    def __init__(self, desiredHeading, steps):
        """
        @type  desiredHeading: float
        @param desiredHeading: Heading you wish to sub to be had
        
        @type  steps: int
        @param steps: Number of increments you wish to change heading in    
        """
        Motion.__init__(self)
        
        self._steps = steps
        self._desiredHeading = desiredHeading
        self._conn = None
        
    def _start(self):
        # Register to recieve AT_ORIENTATION events
        self._conn = self._eventHub.subscribe(control.IController.AT_ORIENTATION,
                                              self._controller, 
                                              self._atOrientation)

        # Start changing heading
        self._nextHeading(self._vehicle.getOrientation().getYaw(True).valueDegrees())
        
    def _nextHeading(self, currentVehicleHeading):
        """
        Changes the heading by one 'step' of the remaing heading change
        """
        headingDifference = self._desiredHeading - currentVehicleHeading 
        headingChange = headingDifference/self._steps
        self._controller.yawVehicle(headingChange)
        
    def _atOrientation(self, event):
        """
        AT_ORIENTATION event handler
        
        Called when the vehicle reaches the command orientation
        """
        self._steps -= 1
        currentVehicleHeading = event.orientation.getYaw(True).valueDegrees()
        if not (self._steps == 0):
            self._nextHeading(currentVehicleHeading)
        else:
            self._finish()
    
    def _finish(self):
        """
        Finishes off the motion, disconnects events, and putlishes finish event
        """
        Motion._finish(self)
        self._conn.disconnect()
