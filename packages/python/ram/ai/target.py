# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/ai/duct.py
  
"""
A state machine for finding and hitting the red light
 - Search for duct
 - Seeks till its in range of duct
 - Aligns to the front of the duct while holding range
 - Does a timed run through the duct
 - Halts vehicle
 
Requires the following subsystems:
 - timerManager - ram.timer.TimerManager
 - motionManager - ram.motion.MotionManager
 - controller - ext.control.IController
"""

# STD Imports
import math

# Project Imports
import ext.core as core
import ext.vision as vision

import ram.filter as filter
import ram.ai.state as state
import ram.motion as motion
import ram.motion.search
import ram.motion.seek
import ram.motion.duct

COMPLETE = core.declareEventType('COMPLETE')

class FilteredState(object):
    """
    Provides filter on the inputs from the vision system
    """
    def enter(self):
        # Read in filter size
        filterSize = self._config.get('filterSize', 1)
        
        # Create filters
        self._xFilter = filter.MovingAverageFilter(filterSize)
        self._yFilter = filter.MovingAverageFilter(filterSize)
        self._rangeFilter = filter.MovingAverageFilter(filterSize)
        self._alignFilter = filter.MovingAverageFilter(filterSize)
        
        # Zero values
        self._filterdX = self._xFilter.getAverage()
        self._filterdY = self._yFilter.getAverage()
        self._filterdRange = self._rangeFilter.getAverage()
        self._filterdAlign = self._alignFilter.getAverage()
        
    def _updateFilters(self, event):
        # Add to filters
        self._xFilter.append(event.x)
        self._yFilter.append(event.y)
        self._rangeFilter.append(event.range)
        self._alignFilter.append(event.squareNess - 1)
        
        # Get new result
        self._filterdX = self._xFilter.getAverage()
        self._filterdY = self._yFilter.getAverage()
        self._filterdRange = self._rangeFilter.getAverage()
        self._filterdAlign = self._alignFilter.getAverage()

class RangeXYHold(FilteredState, state.State):
    """
    Base state that holds the target centered in X, Y and depth without yawing.
    It will also throw off an event whenever it is within range and centering
    tolerances.
    """
    
    IN_RANGE = core.declareEventType('IN_RANGE')
    
    @staticmethod   
    def transitions(myState = None, trans = None):
        if myState is None:
            myState = RangeXYHold
        if trans is None:
            trans = {}
        trans.update({vision.EventType.TARGET_FOUND : myState,
                      vision.EventType.TARGET_LOST : FindAttempt })
        return trans
        
    def TARGET_FOUND(self, event):
        """Update the state of the target, this moves the vehicle"""
        self._updateFilters(event)
        
        # We ignore azimuth and elevation because we aren't using them
        self._target.setState(0, 0, self._filterdRange, self._filterdX, 
                              self._filterdY)
        
        # Only triggered the in range event if we are close and the target is
        # centered in the field of view
        rangeError = math.fabs(self._filterdRange - self._desiredRange)
        frontDistance = math.sqrt(self._filterdX ** 2 + self._filterdY ** 2)
        if (rangeError < self._rangeThreshold) and \
            (frontDistance < self._frontThreshold):
            self.publish(SeekingToRange.IN_RANGE, core.Event())
        
    def enter(self, defaultDepthGain = 1.5):
        FilteredState.enter(self)
        
        # Ensure vision system is on
        self.visionSystem.targetDetectorOn()
        
        # Create tracking object
        self._target = ram.motion.seek.PointTarget(0, 0, 0, 0, 0,
                                                   vehicle = self.vehicle)
        
        # Read in configuration settings
        self._rangeThreshold = self._config.get('rangeThreshold', 0.05)
        self._frontThreshold = self._config.get('frontThreshold', 0.15)
        alignmentThreshold = self._config.get('alignmentThreshold', 0.1)
        depthGain = self._config.get('depthGain', defaultDepthGain)
        iDepthGain = self._config.get('iDepthGain', 0.05)
        dDepthGain = self._config.get('dDepthGain', 0.75)
        maxDepthDt = self._config.get('maxDepthDt', 0.3)
        self._desiredRange = self._config.get('desiredRange', 0.5)
        maxRangeDiff = self._config.get('maxRangeDiff', 0.1)
        maxSpeed = self._config.get('maxSpeed', 1)
        translateGain = self._config.get('translateGain', 2)
        
        motion = ram.motion.seek.SeekPointToRange(target = self._target,
            alignmentThreshold = alignmentThreshold,
            desiredRange = self._desiredRange,
            maxRangeDiff = maxRangeDiff,
            maxSpeed = maxSpeed,
            depthGain = depthGain,
            iDepthGain = iDepthGain,
            dDepthGain = dDepthGain,
            maxDepthDt = maxDepthDt,
            translate = True,
            translateGain = translateGain)
        
        self.motionManager.setMotion(motion)
        
    def exit(self):
        self.motionManager.stopCurrentMotion()

class Start(state.State):
    """
    Gets the vehicle to the proper depth, so it can acquire the target.
    """
    
    @staticmethod
    def transitions():
        return { motion.basic.Motion.FINISHED : FindAttempt }
    
    def enter(self):
        diveMotion = motion.basic.RateChangeDepth(
            desiredDepth = self._config.get('diveDepth', 12),
            speed = self._config.get('diveSpeed', 1.0/3.0))
        self.motionManager.setMotion(diveMotion)
        
    def exit(self):
        self.motionManager.stopCurrentMotion()

class FindAttempt(state.State):
    """
    Attempt to find the target, before starting search pattern.  This will
    catch the object if its front of the vehicle.
    """
    TIMEOUT = core.declareEventType('TIMEOUT')

    @staticmethod
    def transitions():
        return { FindAttempt.TIMEOUT : Searching,
                 vision.EventType.TARGET_FOUND : SeekingToCentered }
    
    def enter(self):
        # Turn all off all motions, hold the current heading
        self.motionManager.stopCurrentMotion()
        self.controller.holdCurrentHeading()
        # TODO: Hold deth        
        
        # Turn on the detector
        self.visionSystem.targetDetectorOn()

        # Start up the timer 
        delay = self._config.get('delay', 2)
        self.timer = self.timerManager.newTimer(FindAttempt.TIMEOUT, delay)
        self.timer.start()

    def exit(self):
        self.timer.stop()

class Searching(state.State):
    """
    Runs a zig-zag search pattern until it finds the duct
    """
    
    @staticmethod
    def transitions():
        return { vision.EventType.TARGET_FOUND : SeekingToCentered }

    def enter(self):
        # Make sure the detector is on the vision system
        self.visionSystem.targetDetectorOn()

        # Create zig zag search to 
        zigZag = motion.search.ForwardZigZag(
            legTime = 15,
            sweepAngle = 60,
            speed = 2.5)
        self.motionManager.setMotion(zigZag)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class SeekingToCentered(RangeXYHold):
    """
    Changes the depth and centers the target
    """

    _offset = 0.0
    _firstRun = True

    @staticmethod
    def transitions():
        return RangeXYHold.transitions(SeekingToCentered, {
            ram.motion.seek.SeekPoint.POINT_ALIGNED : SeekingToRange})
        
    def POINT_ALIGNED(self, event):
        """When aligned we are at a good depth so hold it"""
        self.controller.holdCurrentDepth()
        
    def TARGET_FOUND(self, event):
        """Update the state of the target, this moves the vehicle"""
        self._updateFilters(event)

        # To keep the range the same, the vehicle saves its first range
        # value and keeps the vehicle at this range
        if (self._firstRun):
            self._offset = self._filterdRange
            self._firstRun = False
        # TODO: take a close look at range seeking here
        # We ignore azimuth and elevation because we aren't using them
        self._target.setState(0, 0, self._filterdRange, #- self._offset +
                              #self._desiredRange, 
                              self._filterdX,
                              self._filterdY)
        
class SeekingToRange(RangeXYHold):
    """
    Heads toward the target until it reaches the desired range
    """
    
    @staticmethod
    def transitions():
        return RangeXYHold.transitions(SeekingToRange, {
            RangeXYHold.IN_RANGE : SeekingToAligned })
        
class FireTorpedos(RangeXYHold):
    """
    Fires the two torpedos at the target, with 1 second in between each, but
    only if the target is within the desired bounds.
    """

    NUMBER_TORPEDOS = 2
    ARM_TORPEDOS = core.declareEventType('ARM_TORPEDOS_')
    MOVE_ON = core.declareEventType('MOVE_ON')

    @staticmethod
    def transitions():
        return RangeXYHold.transitions(FireTorpedos, {
            RangeXYHold.IN_RANGE : FireTorpedos,
            FireTorpedos.ARM_TORPEDOS: FireTorpedos,
            FireTorpedos.MOVE_ON : End})

    def IN_RANGE(self, event):
        """
        Fires the torpedos only when armed, disarms the torpedos afterward so
        that next will be fired only after the delay period.
        """
        if self._armed:
            self.vehicle.fireTorpedo()

            # Diarm torpeos, must be armed again, after a wait for the current
            # torpedo to get through
            self._armed = False

            # Increment number of torpedos fired
            fireCount = self.ai.data.get('torpedosFired', 0)
            self.ai.data['torpedosFired'] = fireCount + 1

            # Reset the countdown if we still have more to fire
            self._resetFireTimer()

    def ARM_TORPEDOS_(self, event):
        """
        Arms the torpedos for firing when we are in range.
        """
        self._armed = True

    def enter(self):
        RangeXYHold.enter(self)

        self._timer = None
        self._delay = self._config.get('fireDelay', 2)
        self._armed = False

        # Start the countdown till arming
        self._resetFireTimer()

    def exit(self):
        if self._timer is not None:
            self._timer.stop()

    @property
    def armed(self):
        return self._armed
    
    def _resetFireTimer(self):
        """
        Initiates a timer which arms the firing of a torpedo after the desired
        delay
        """
        if self.ai.data.get('torpedosFired', 0) < FireTorpedos.NUMBER_TORPEDOS:
            self._timer = self.timerManager.newTimer(FireTorpedos.ARM_TORPEDOS,
                                                     self._delay)
            self._timer.start()
        else:
            # All torpedos fired, lets get out of this state
            self.publish(FireTorpedos.MOVE_ON, core.Event())
        
class TargetAlignState(FilteredState):
    def TARGET_FOUND(self, event):
        self._updateFilters(event)
        """Update the state of the light, this moves the vehicle"""
        azimuth = self._filterdX * -107.0/2.0
        elevation = self._filterdY * -80.0/2.0
        self._target.setState(azimuth, elevation, self._filterdRange, 
                              self._filterdX, self._filterdY, 
                              self._filterdAlign * self._alignSign)
        
    def enter(self):
        FilteredState.enter(self)
        
        # Ensure vision system is on
        self.visionSystem.targetDetectorOn()
        
        # Create tracking object
        self._target = ram.motion.duct.Duct(0, 0, 0, 0, 0, 0,
                                            vehicle = self.vehicle)
        self._alignSign = 1
        
        # Read in configuration settings
        depthGain = self._config.get('depthGain', 1.5)
        iDepthGain = self._config.get('iDepthGain', 0.05)
        dDepthGain = self._config.get('dDepthGain', 0.75)
        maxDepthDt = self._config.get('maxDepthDt', 0.3)
        
        desiredRange = self._config.get('desiredRange', 0.5)
        maxRangeDiff = self._config.get('maxRangeDiff', 0.1)
        maxAlignDiff = self._config.get('maxAlignDiff', 0.5)
        alignGain = self._config.get('alignGain', 1.0)
        maxSpeed = self._config.get('maxSpeed', 0.75)
        maxSidewaysSpeed = self._config.get('maxSidewaysSpeed', 2)

        motion = ram.motion.duct.DuctSeekAlign(target = self._target,
            desiredRange = desiredRange,
            maxRangeDiff = maxRangeDiff,
            maxAlignDiff = maxAlignDiff, 
            maxSpeed = maxSpeed,
            maxSidewaysSpeed = maxSidewaysSpeed,
            alignGain = alignGain,
            depthGain = depthGain,
            iDepthGain = iDepthGain,
            dDepthGain = dDepthGain,
            maxDepthDt = maxDepthDt)
        
        self.motionManager.setMotion(motion)
        
    def exit(self):
        self.motionManager.stopCurrentMotion()   
        
class SeekingToAligned(TargetAlignState, state.State):
    """
    Holds the target at range and in the center of the field of view while 
    rotating around it.  If its rotating the wrong direction is will reverse
    that direction and keep going until its close enough to aligned.
    """
    ALIGNED = core.declareEventType('ALIGNED')
    CHECK_DIRECTION = core.declareEventType('CHECK_DIRECTION_')
    
    @staticmethod
    def transitions():
        return { vision.EventType.TARGET_FOUND : SeekingToAligned,
                 vision.EventType.TARGET_LOST : FindAttempt,
                 SeekingToAligned.CHECK_DIRECTION : SeekingToAligned,
                 SeekingToAligned.ALIGNED : FireTorpedos }

    def TARGET_FOUND(self, event):
        # Update motion
        TargetAlignState.TARGET_FOUND(self, event)

        # Record first squareNess and every squareness
        squareNess = self._filterdAlign + 1
        if self._firstEvent:
            self._startSquareNess = squareNess
            self._firstEvent = False
        self._currentSquareNess = squareNess
            
        # Publish aligned event if needed
        if squareNess > self._minSquareNess:
            self.publish(SeekingToAligned.ALIGNED, core.Event())

    def CHECK_DIRECTION_(self, event):
        if self._currentSquareNess < self._startSquareNess:
            self._alignSign *= -1
            
    def enter(self):
        TargetAlignState.enter(self)

        self._firstEvent = True
        self._startSquareNess = None
        self._currentSquareNess = None
        
        # Start timer
        self._minSquareNess = self._config.get('minSquareNess', 0.9)
        self._delay = self._config.get('checkDelay', 4)
        self._timer = self.timerManager.newTimer(
            SeekingToAligned.CHECK_DIRECTION, self._delay)
        self._timer.start()
        
    def exit(self):
        TargetAlignState.exit(self)
        self._timer.stop()
    
# The next two states are extra states, not sure if they are needed    
class Aligning(TargetAlignState, state.State):
    SETTLED = core.declareEventType('ALIGNED')
       
    @staticmethod
    def transitions():
        return { vision.EventType.TARGET_FOUND : Aligning,
                 vision.EventType.TARGET_LOST : FindAttempt,
                 Aligning.SETTLED : Through }

    def enter(self):
        TargetAlignState.enter(self)
        
        self.timer = self.timerManager.newTimer(
            Aligning.SETTLED, self._config.get('settleTime', 15))
        self.timer.start()
        
    def exit(self):
        TargetAlignState.exit(self)
        self.timer.stop()
        
class Through(state.State):
    FORWARD_DONE = core.declareEventType('FORWARD_DONE')
    
    @staticmethod
    def transitions():
        return {Through.FORWARD_DONE : End}

    def enter(self):
        self.visionSystem.targetDetectorOff()

        # Timer goes off in 3 seconds then sends off FORWARD_DONE
        self.timer = self.timerManager.newTimer(
            Through.FORWARD_DONE, self._config.get('forwardTime', 15))
        self.timer.start()
        self.controller.setSidewaysSpeed(0)
        self.controller.setSpeed(3)
    
    def exit(self):
        self.timer.stop()
        self.controller.setSpeed(0)
        
class End(state.State):
    def enter(self):
        self.publish(COMPLETE, core.Event())
