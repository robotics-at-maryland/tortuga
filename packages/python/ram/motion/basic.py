# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/motion.py

# Python Imports
import math as pmath

# Project Imports
import ext.core as core
import ext.control as control
import ext.vehicle as vehicle
import ext.estimation as estimation
import ext.math as math

import ram.timer as timer
from ram.logloader import resolve

import trajectories


class MotionManager(core.Subsystem):
    """
    Starts and stops the AI's currently active ram.motion.basic.Motion
    
    Requires the following subsystems:
     - ext.control.IController
     - ext.vehicle.IVehicle
     - ext.core.EventHub
     - ext.core.QueuedEventHub
    """

    FINISHED = core.declareEventType('FINISHED')
    
    def __init__(self, config, deps):
        """
        Create a MotionManager object
        """
        core.Subsystem.__init__(self, config.get('name', 'MotionManager'), deps)
        self._inPlaneMotion = None
        self._depthMotion = None
        self._orientationMotion = None
        
        self._controller = core.Subsystem.getSubsystemOfType(control.IController, 
                                                             deps, 
                                                             nonNone = True)
        
        self._vehicle = core.Subsystem.getSubsystemOfType(vehicle.IVehicle, 
                                                          deps, 
                                                          nonNone = True)

        self._estimator = core.Subsystem.getSubsystemOfType(estimation.IStateEstimator,
                                                            deps,
                                                            nonNone = True)
        
        self._qeventHub = core.Subsystem.getSubsystemOfType(core.QueuedEventHub, 
                                                           deps,
                                                           nonNone = True)
        
        self._eventHub = core.Subsystem.getSubsystemOfExactType(
            core.EventHub, deps, nonNone = True)
        
        self._eventHub.subscribeToType(Motion.FINISHED, self._motionFinished)

        self._queuedMotions = []
        self._queueActive = False
            
    def _setMotion(self, motion):
        """
        Stops the current motion and starts this one
        """
        eventPublisher = core.EventPublisher(self._eventHub)
        started = False
        
        if motion.type & Motion.IN_PLANE:
            if self._inPlaneMotion is not None:
                self._stopMotion(self._inPlaneMotion)
            self._inPlaneMotion = motion
            
            self._inPlaneMotion.start(self._controller, self._vehicle,
                                      self._estimator, self._qeventHub,
                                      eventPublisher)
            started = True
            
        if motion.type & Motion.DEPTH:
            if self._depthMotion is not None:
                self._stopMotion(self._depthMotion)
            self._depthMotion = motion
            
            if not started:
                self._depthMotion.start(self._controller, self._vehicle,
                                        self._estimator, self._qeventHub,
                                        eventPublisher)
                started = True
            
        if motion.type & Motion.ORIENTATION:
            if self._orientationMotion is not None:
                self._stopMotion(self._orientationMotion)
            self._orientationMotion = motion
            
            if not started:
                self._orientationMotion.start(self._controller, self._vehicle,
                                              self._estimator, self._qeventHub,
                                              eventPublisher)

    def setMotion(self, motion, *motions):
        self._queuedMotions = []
        # Queue the motions before setting the first motion
        # Reason: If the first motion finishes in its _start function,
        # it will not queue the remaining motions correctly
        self._queueMotions(*motions)
        self._setMotion(motion)

    def _queueMotions(self, *motions):
        for m in motions:
            self._queuedMotions.append(m)

    def stopCurrentMotion(self):
        """
        Calls stop() on the current motion if it exists.
        """
        if self._inPlaneMotion is not None:
            self._stopMotion(self._inPlaneMotion)
        if self._depthMotion is not None:
            self._stopMotion(self._depthMotion)
        if self._orientationMotion is not None:
            self._stopMotion(self._orientationMotion)
        self._queuedMotions = []
        
    def stopMotionOfType(self, _type):
        if _type & Motion.IN_PLANE:
            assert not (self._inPlaneMotion is None)
            self._stopMotion(self._inPlaneMotion)
        elif _type & Motion.DEPTH:
            assert not (self._depthMotion is None)
            self._stopMotion(self._depthMotion)
        elif _type & Motion.ORIENTATION:
            assert not (self._orientationMotion is None)
            self._stopMotion(self._orientationMotion)
        
    @property
    def currentMotion(self):
        motionCount = 0
        motion = None
        if not (self._inPlaneMotion is None):
            motionCount += 1
            motion = self._inPlaneMotion
        if not (self._depthMotion is None) and \
          not (motion == self._depthMotion):
            motionCount += 1
            motion = self._depthMotion
        if not (self._orientationMotion is None) and \
          not (motion == self._orientationMotion):
            motionCount += 1
            motion = self._orientationMotion
            
        if motionCount <= 1:
            return motion
        return (self._inPlaneMotion, self._depthMotion, 
                self._orientationMotion)

    @property
    def currentMotionList(self):
        currentMotion = self.currentMotion

        if currentMotion is None:
            return None

        return [currentMotion] + self._queuedMotions

    def _removeMotion(self, motion):
        if motion.type & Motion.IN_PLANE:
            assert not (self._inPlaneMotion is None)
            assert motion == self._inPlaneMotion
            self._inPlaneMotion = None
        if motion.type & Motion.DEPTH:
            assert not (self._depthMotion is None)
            assert motion == self._depthMotion
            self._depthMotion = None
        if motion.type & Motion.ORIENTATION:
            assert not (self._orientationMotion is None)
            assert motion == self._orientationMotion
            self._orientationMotion = None
    
    def _stopMotion(self, motion):
        motion.stop()
        self._removeMotion(motion)
            
    def _motionFinished(self, event):
        self._removeMotion(event.motion)
        # Start any remaining motions
        self._startQueuedMotion()

    def _startQueuedMotion(self):
        # Check if there are any queued motions
        if len(self._queuedMotions) == 0:
            # Publish the queued event finish if there's no queued motions
            self.publish(MotionManager.FINISHED, core.Event())
        else:
            # Start the queued motion    
            motion = self._queuedMotions.pop(0)
            self._setMotion(motion)
        
    def background(self):
        pass
        
    def backgrounded(self):
        return True
        
    def unbackground(self, join = True):
        pass
        
    def update(self, timeSinceLastUpdate):
        pass

    @staticmethod
    def generateMotion(dottedName, complete = False, **kwargs):
        class_ = resolve(dottedName)
        if not issubclass(class_, Motion):
            raise Exception('%s is not of type %s' % (class_, Motion))
        if complete and not class_.isComplete():
            raise Exception('%s is an incomplete motion. A complete'
                            'motion is required' % class_)

        # Generate an instance of the motion using kwargs
        try:
            return class_(**kwargs)
        except TypeError, e:
            raise TypeError('Not enough arguments for motion %s' % dottedName)

    @staticmethod
    def generateMotionList(mList, strict = False):
        """
        Generates a list of motions from the given dictionary/config section.

        The format of the config file should have ONLY motion entries.
        Each entry should be labeled with its position in the list.
        For example, to generate 2 motions, you would use this:

        '1':
            type: 'ram.motion.basic.TimedMoveDirection'
            ...
        '2':
            type: 'ram.motion.basic.RateChangeDepth'
            ...

        The entries for a motion should contain the full dotted name of the
        motion and any named parameters for the motion. This does not error
        check if the parameters you sent are correct, and will just merely
        error out when it tries to create the motion.

        The motion list must contain only complete motions. A complete motion
        is any motion that will finish on its own without being explicitly
        stopped. The exception is the last motion. If strict is set to
        False, then the last motion can be incomplete. If strict is set to
        True, then the last motion must complete.

        @param mList config for motions to generate
        @param strict Whether to be strict with the last motion, see above
        """
        # Number of total motions
        max = len(mList)
        motionList = [0]*max

        for num, info in mList.iteritems():
            type_ = info.pop('type')
            if int(num) == max:
                m = MotionManager.generateMotion(
                    type_, complete = strict, **info)
            else:
                m = MotionManager.generateMotion(
                    type_, complete = True, **info)
            motionList[int(num)-1] = m
	    # Fix the config file
	    info['type'] = type_
        return motionList
            
core.SubsystemMaker.registerSubsystem('MotionManager', MotionManager)
        
class Motion(object):
    """
    A class which encapsulates some sort of motion of the vehicle.
    """    
    
    IN_PLANE = 1
    DEPTH = 2
    ORIENTATION = 4
    NORMAL = IN_PLANE | DEPTH | ORIENTATION
    
    FINISHED = core.declareEventType('FINISHED')
    
    def __init__(self, _type = NORMAL):
        self._eventPublisher = None
        self._controller = None
        self._vehicle = None
        self._estimator = None
        self._eventHub = None
        self._type = _type
    
    def start(self, controller, vehicle, estimator, eventHub, eventPublisher):
        """
        Called by the motion manager to state main state variables
        
        DO NOT OVERRIDE this method directly.  Override _start instead.
        
        @type  controller: ext.control.IController 
        @param controller: The current controller of the vehicle
        
        @type  vehicle: ext.vehicle.IVehicle
        @param vehicle: The vehicle we are controlling
        
        @type  eventHub: ext.core.QueuedEventHub
        @param eventHub: The event hub to subscribe to events through 
        """
        self._controller = controller
        self._vehicle = vehicle
        self._estimator = estimator
        self._eventHub = eventHub
        self._eventPublisher = eventPublisher
        
        # Set up the publish method to be seemless and easy
        self.publish = self._eventPublisher.publish
        
        self._start()
        
    @property
    def type(self):
        return self._type
        
    def _start(self):
        raise Exception("You must implement this method")
    
    def _finish(self):
        """
        Raises, motion finished event.
        """
        event = core.Event()
        event.motion = self
        self.publish(Motion.FINISHED, event)
        
    def stop(self):
        pass

    @staticmethod
    def isComplete(self):
        """
        Returns true if the motion eventually terminates and calls _finish()
      
        All subclasses need to implement this feature, if they don't
        an exception will be thrown.  Abstract base classes, added to 
        python in 2.6, provide this same functionality but we only
        support python 2.5.
        """
        raise Exception("Not implemented")
    
class ChangeDepth(Motion):
    DEPTH_TRAJECTORY_UPDATE  = core.declareEventType('DEPTH_TRAJECTORY_UPDATE')

    def __init__(self, trajectory, updateRate = 1/25):
        """
        @type  Trajectory
        @param trajecotry
        must evaluate with respect to absolute robot time

        @type  updateRate: int
        @param updateRate: rate to evaluate the trajectory   
        """
        Motion.__init__(self, _type = Motion.DEPTH)
        self._trajectory = trajectory
        self._interval = updateRate
        self._conn = None
        self._timer = None
        
    def _start(self):
        self._timer  = timer.Timer(self, ChangeDepth.DEPTH_TRAJECTORY_UPDATE,
                                   self._interval, repeat = True)

        self._conn = self._eventHub.subscribeToType(ChangeDepth.DEPTH_TRAJECTORY_UPDATE,
                                                    self._update)

        # evaluate the initial state of the trajectory
        initialTime = self._trajectory.getInitialTime()
        initialDepth = self._trajectory.computeValue(initialTime)
        initialRate = self._trajectory.computeDerivative(initialTime,1)
        
        # send the initial values to the controller
        self._controller.changeDepth(initialDepth, initialRate)

        self._timer.start()
        
    def _update(self, event):
        currentTime = timer.time()

        # evaluate the trajectory value and 1st derivative
        newDepth = self._trajectory.computeValue(currentTime)
        newDepthRate = self._trajectory.computeDerivative(currentTime,1)

        # send the new values to the controller
        self._controller.changeDepth(newDepth, newDepthRate)

        if (currentTime >= self._trajectory.getFinalTime() and
           self._controller.atDepth()):
            self._finish()
        
    def _finish(self):
        """
        Finishes off the motion, disconnects events, and publishes finish event
        """
        self._conn.disconnect()
        self._timer.stop()
        Motion._finish(self)
        


    def stop(self):
        pass

    @staticmethod
    def isComplete():
        return timer.time() > self._trajectory.getFinalTime()

class ChangeOrientation(Motion):
    ORIENTATION_TRAJECTORY_UPDATE = core.declareEventType('ORIENTATION_TRAJECTORY_UPDATE')

    def __init__(self, trajectory, updateRate = 25):
        """
        @type  trajectory: Trajctory
        @param trajectory: Heading you wish to sub to be at
        
        @type  updateRate: int
        @param updateRate: rate to evaluate the trajectory    
        """
        Motion.__init__(self, _type = Motion.ORIENTATION)

        self._trajectory = trajectory
        self._interval = updateRate
        self._conn = None
        self._timer = None

    def _start(self):

        self._timer  = timer.Timer(
            self, ChangeOrientation.ORIENTATION_TRAJECTORY_UPDATE,
            self._interval, repeat = True)

        self._conn = self._eventHub.subscribeToType(
            ChangeOrientation.ORIENTATION_TRAJECTORY_UPDATE,
            self._update)

        # evaluate the initial state of the trajectory
        initialTime = self._trajectory.getInitialTime()
        initialOrientation = self._trajectory.computeValue(initialTime)
        initialRate = self._trajectory.computeDerivative(initialTime,1)
        
        # send the initial values to the controller
        self._controller.rotate(initialOrientation, initialRate)
        

    def _update():
        currentTime = timer.time()

        # evaluate the trajectory value and 1st derivative
        newOrientation = self._trajectory.computeValue(currentTime)
        newAngularRate = self._trajectory.computeDerivative(currentTime,1)

        # send the new values to the controller
        self._controller.rotate(newOrientation, newAngularRate)

        if (currentTime >= self._trajectory.getFinalTime() and 
            self._controller.atOrientation()):
            self._finish()
        

    def _finish(self):
        """
        Finishes off the motion, disconnects events, and publishes finish event
        """        
        self._conn.disconnect()
        self._timer.stop()
        Motion._finish(self)


    def stop(self):
        pass

    @staticmethod
    def isComplete():
        return True

class MoveInPlane(Motion):
    INPLANE_TRAJECTORY_UPDATE = core.declareEventType('INPLANE_TRAJECTORY_UPDATE')

    def __init_(self, trajectory, updateRate = 25):
        """
        Initializes the motion to execute a trajectory at the specified rate
        @type  trajectory: Trajctory
        @param trajectory: Heading you wish to sub to be at
        
        @type  updateRate: int
        @param updateRate: rate to evaluate the trajectory  
        """
        Motion.__init__(self, _type = Motion.IN_PLANE)

        self._trajectory = trajectory
        self._interval = updateRate
        self._conn = None
        self._timer = None

    def _start(self):

        self._timer  = timer.Timer(
            self, MoveInPlane.INPLANE_TRAJECTORY_UPDATE,
            self._interval, repeat = True)

        self._conn = self._eventHub.subscribeToType(
            MoveInPlane.INPLANE_TRAJECTORY_UPDATE,
            self._update)

        # evaluate the initial state of the trajectory
        initialTime = self._trajectory.getInitialTime()
        initialPosition = self._trajectory.computeValue(initialTime)
        initialVelocity = self._trajectory.computeDerivative(initialTime,1)
        
        # send the initial values to the controller
        self._controller.rotate(initialPosition, initialVelocity)

    def _update(self):
        currentTime = timer.time()

        # evaluate the trajectory value and 1st derivative
        newPosition = self._trajectory.computeValue(currentTime)
        newVelocity = self._trajectory.computeDerivative(currentTime,1)

        # send the new values to the controller
        self._controller.rotate(newPosition, newVelocity)

        if (currentTime >= self._trajectory.getFinalTime() and
           self._controller.atPosition() and self._controller.atVelocity()):
            self._finish()

    def _finish(self):
        """
        Finishes off the motion, disconnects events, and publishes finish event
        """
        Motion._finish(self)
        self._conn.disconnect()
        self._timer.stop()
        
    def stop(self):
        pass

    @staticmethod
    def isComplete():
        return True
