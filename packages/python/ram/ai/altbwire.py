# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/ai/altbwire.py
  
# STD Imports
import math

# Project Imports
import ext.core as core
import ext.vision as vision
import ext.math

import ram.filter as filter
import ram.ai.state as state
import ram.motion as motion
import ram.motion.search
import ram.motion.seek
import ram.motion.duct
import ram.motion.pipe

COMPLETE = core.declareEventType('COMPLETE')

class StoreBarbedWireEvent(object):
    """
    Common subclass for states that have a BARBED_WIRE_FOUND transition,
    it stores the event is the ai.data.
    """
    def BARBED_WIRE_FOUND(self, event):
        self.ai.data['lastBarbedWireEvent'] = event

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

class RangeXYHold(FilteredState, state.State, StoreBarbedWireEvent):
    """
    Base state that holds the target centered in X, Y and depth without yawing.
    It will also throw off an event whenever it is within range and centering
    tolerances.
    """
    
    IN_RANGE = core.declareEventType('IN_RANGE')
    
    @staticmethod
    def transitions(myState = None, lostState = None, trans = None):
        if myState is None:
            myState = RangeXYHold
        if lostState is None:
            lostState = FindAttempt
        if trans is None:
            trans = {}
        trans.update({vision.EventType.BARBED_WIRE_FOUND : myState,
                      vision.EventType.BARBED_WIRE_LOST : lostState })
        return trans

    @staticmethod
    def getattr():
        return set(['yZero', 'rangeThreshold', 'frontThreshold', 'depthGain',
             'iDepthGain', 'dDepthGain', 'maxDepthDt', 'desiredRange',
             'maxRangeDiff', 'maxSpeed', 'rangeGain', 'translateGain',
             'iTranslateGain', 'dTranslateGain', 'filterSize'])
        
    def BARBED_WIRE_FOUND(self, event):
        """Update the state of the target, this moves the vehicle"""
        # Todo: consider filter removal
        #self._updateFilters(event)

        x = event.topX
        y = event.topY
        width = event.topWidth
        
        # Determine y value based on whether we are ignoring depth or not, 
        # also store this event
        StoreBarbedWireEvent.BARBED_WIRE_FOUND(self, event)
        if self._depthGain == 0:
            y = 0
        
        # Width == 1 when we are close, so inversion is needed to get range 
        range = 1 - width
        
        # Finally set the state (We ignore azimuth and elevation because we 
        # aren't using them)
        self._target.setState(0, 0, range = range, x = x, y = y,
                              timeStamp = event.timeStamp)
        
        # Only triggered the in range event if we are close and the target is
        # centered in the field of view
        rangeError = math.fabs(range - self._desiredRange)
        frontDistance = math.sqrt(x ** 2 + y ** 2)
        if (rangeError < self._rangeThreshold) and \
            (frontDistance < self._frontThreshold):
            self.publish(SeekingToRange.IN_RANGE, core.Event())
        
    def enter(self):
        FilteredState.enter(self)
        
        # Ensure vision system is on
        self.visionSystem.barbedWireDetectorOn()
        
        # Read in configuration settings
        self._yZero = self._config.get('yZero', 0.5)
        self._rangeThreshold = self._config.get('rangeThreshold', 0.05)
        self._frontThreshold = self._config.get('frontThreshold', 0.15)
        
        self._depthGain = self._config.get('depthGain', 1.5)
        iDepthGain = self._config.get('iDepthGain', 0.05)
        dDepthGain = self._config.get('dDepthGain', 0.3)
        maxDepthDt = self._config.get('maxDepthDt', 0.3)
        
        self._desiredRange = self._config.get('desiredRange', 0.5)
        maxRangeDiff = self._config.get('maxRangeDiff', 0.2)
        maxSpeed = self._config.get('maxSpeed', 0.75)
        rangeGain = self._config.get('rangeGain', 1)      
  
        translateGain = self._config.get('translateGain', 1)
        iTranslateGain = self._config.get('iTranslateGain', 0.125)
        dTranslateGain = self._config.get('dTranslateGain', 0.25)
        
        # Create tracking object
        # Initializing the range to the desired range so the vehicle won't
        # move forward or backward (aka. no jerking backwards)
        self._target = ram.motion.seek.PointTarget(azimuth = 0,
                                                   elevation = 0,
                                                   range = self._desiredRange,
                                                   x = 0,
                                                   y = 0,
                                                   timeStamp = None,
                                                   vehicle = self.vehicle)
        
        motion = ram.motion.seek.SeekPointToRange(target = self._target,
            desiredRange = self._desiredRange,
            maxRangeDiff = maxRangeDiff,
            maxSpeed = maxSpeed,
            rangeGain = rangeGain,
            depthGain = self._depthGain,
            iDepthGain = iDepthGain,
            dDepthGain = dDepthGain,
            maxDepthDt = maxDepthDt,
            translate = True,
            translateGain = translateGain,
            iTranslateGain = iTranslateGain,
            dTranslateGain = dTranslateGain)
        
        self.motionManager.setMotion(motion)
        
    def exit(self):
        self.motionManager.stopCurrentMotion()

class Start(state.State):
    """
    Gets the vehicle to the proper depth, so it can acquire the target.
    """
    
    @staticmethod
    def transitions():
        return { motion.basic.Motion.FINISHED : Searching }

    @staticmethod
    def getattr():
        return set(['diveSpeed'])
    
    def enter(self):
        # Set the initial direction
        orientation = self.vehicle.getOrientation()
        self.ai.data['barbedWireStartOrientation'] = \
            orientation.getYaw().valueDegrees()

        # Create the dive motion and start the vehicle
        diveMotion = motion.basic.RateChangeDepth(
            desiredDepth = self.ai.data['config'].get('bwireDepth', 8.3),
            speed = self._config.get('diveSpeed', 1.0/3.0))
        self.motionManager.setMotion(diveMotion)

        self.ai.data['firstSearching'] = True
        
    def exit(self):
        self.motionManager.stopCurrentMotion()

class FindAttempt(state.FindAttempt, StoreBarbedWireEvent):
    """
    Attempt to find the target, before starting search pattern.  This will
    catch the object if its front of the vehicle.
    """

    @staticmethod
    def transitions(foundState = None):
        if foundState is None:
            foundState = SeekingToRange
        return state.FindAttempt.transitions(vision.EventType.BARBED_WIRE_FOUND,
                                             foundState, Searching)
        
    def enter(self):
        state.FindAttempt.enter(self)
        self.visionSystem.barbedWireDetectorOn()

class Searching(state.State, StoreBarbedWireEvent):
    """
    Runs a zig-zag search pattern until it finds the duct
    """
    
    @staticmethod
    def transitions():
        return { vision.EventType.BARBED_WIRE_FOUND : SeekingToRange }

    @staticmethod
    def getattr():
        return set(['legTime', 'sweepAngle', 'speed'])

    def enter(self):
        # Make sure the detector is on the vision system
        self.visionSystem.barbedWireDetectorOn()

        orientation = self.vehicle.getOrientation()
        direction = self.ai.data.setdefault('barbedWireStartOrientation',
                                orientation.getYaw().valueDegrees())

        # Create the forward motion
        self._duration = self.ai.data['config'].get('BarbedWire', {}).get(
            'forwardDuration', 2)
        self._forwardSpeed = self.ai.data['config'].get('BarbedWire', {}).get(
            'forwardSpeed', 3)
        self._forwardMotion = motion.basic.TimedMoveDirection(
            desiredHeading = 0,
            speed = self._forwardSpeed,
            duration = self._duration,
            absolute = False)

        self._legTime = self._config.get('legTime', 15)
        self._sweepAngle = self._config.get('sweepAngle', 60)
        self._speed = self._config.get('speed', 2.5)

        # Create zig zag search to 
        self._zigZag = motion.search.ForwardZigZag(
            legTime = self._legTime,
            sweepAngle = self._sweepAngle,
            speed = self._speed,
            direction = direction)

        if self.ai.data.get('firstSearching', True) and self._duration > 0:
            self.motionManager.setQueuedMotions(self._forwardMotion,
                                                self._zigZag)
        else:
            self.motionManager.setMotion(self._zigZag)

        self.ai.data['firstSearching'] = False

    def exit(self):
        self.motionManager.stopCurrentMotion()
        
class SeekingToRange(RangeXYHold):
    """
    Heads toward the target until it reaches the desired range
    """
    
    @staticmethod
    def transitions():
        return RangeXYHold.transitions(myState = SeekingToRange,
            lostState = FindAttempt,
            trans = { RangeXYHold.IN_RANGE : SeekingToAligned })

    @staticmethod
    def getattr():
        return set(['maxAlignCheckWidth', 'maxOverlap']).union(
            RangeXYHold.getattr())

    def enter(self, maxAlignCheckWidth = 0.7, maxOverlap = 0.1):
        # Do the normal enter functions
        RangeXYHold.enter(self)
        
        # Load config options
        self._maxAlignCheckWidth = self._config.get('maxAlignCheckWidth',
                                                    maxAlignCheckWidth)
        self._maxOverlap = self._config.get('maxOverlap', maxOverlap)

# Not used
class SeekingToRange2(RangeXYHold):
    """
    Heads toward the target until it reaches the desired range
    """
    
    @staticmethod
    def transitions():
        return RangeXYHold.transitions(SeekingToRange2, {
            RangeXYHold.IN_RANGE : SeekingToAligned2 })
        
class TargetAlignState(FilteredState, StoreBarbedWireEvent):
    def BARBED_WIRE_FOUND(self, event):
        # Todo remove filters
        #self._updateFilters(event)
        """Store the event"""
        StoreBarbedWireEvent.BARBED_WIRE_FOUND(self, event)

        # Check if the bottom pipe is closer than the top pipe
        #if event.bottomWidth > event.topWidth:
        #    # If it is, use the bottom pipe instead
        #    x = event.bottomX
        #    y = event.bottomY
        #    width = event.bottomWidth
        #else:
        #    x = event.topX
        #    y = event.topY
        #    width = event.topWidth
        x = event.topX
        y = event.topY
        width = event.topWidth
        
        """Update the state of the light, this moves the vehicle"""
        azimuth = x * -107.0/2.0
        elevation = y * -80.0/2.0
        
        # Determine y value based on whether we are ignoring depth or not, 
        # also take into account any offset we wish to hold the object at.
        if self._depthGain == 0:
            y = 0
        
        range = 1 - width
        orientation = self.vehicle.getOrientation()
        alignment = orientation.getYaw().valueDegrees() - \
            self._desiredOrientation
        self._target.setState(azimuth, elevation, range = range,
                              x = x, y = y, 
                              alignment = alignment,
                              timeStamp = event.timeStamp)
        
    def enter(self):
        FilteredState.enter(self)
        
        # Ensure vision system is on
        self.visionSystem.barbedWireDetectorOn()
        
        # Read in configuration settings
        self._desiredOrientation = self.ai.data['config'].get(
            'BarbedWire', {}).get('orientation', 0)
        self._depthGain = self._config.get('depthGain', 1)
        maxDepthDt = self._config.get('maxDepthDt', 0.3)
        desiredRange = self._config.get('desiredRange', 0.5)
        maxRangeDiff = self._config.get('maxRangeDiff', 0.2)
        maxAlignDiff = self._config.get('maxAlignDiff', 20)
        alignGain = self._config.get('alignGain', 3.0/40.0)
        rangeGain = self._config.get('rangeGain', 1.0)
        maxSpeed = self._config.get('maxSpeed', 0.75)
        maxSidewaysSpeed = self._config.get('maxSidewaysSpeed', 1)
        yawGain = self._config.get('yawGain', 1.0)
        
        # Create tracking object
        # Initializing the range to the desired range so the vehicle won't
        # move forward or backward (aka. no jerking backwards)
        self._target = ram.motion.duct.Duct(azimuth = 0,
                                            elevation = 0,
                                            range = desiredRange,
                                            x = 0,
                                            y = 0,
                                            alignment = 0,
                                            vehicle = self.vehicle)

        motion = ram.motion.duct.DuctSeekAlign(target = self._target,
            desiredRange = desiredRange,
            maxRangeDiff = maxRangeDiff,
            maxAlignDiff = maxAlignDiff, 
            maxSpeed = maxSpeed,
            maxSidewaysSpeed = maxSidewaysSpeed,
            alignGain = alignGain,
            rangeGain = rangeGain,
            depthGain = self._depthGain,
            maxDepthDt = maxDepthDt,
            yawGain = yawGain)
        
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
        return { vision.EventType.BARBED_WIRE_FOUND : SeekingToAligned,
                 vision.EventType.BARBED_WIRE_LOST : AlignedFindAttempt,
                 SeekingToAligned.CHECK_DIRECTION : SeekingToAligned,
                 SeekingToAligned.ALIGNED : Aligning }

    @staticmethod
    def getattr():
        return set(['yZero', 'depthGain', 'maxDepthDt', 'desiredRange',
                    'maxRangeDiff', 'maxAlignDiff', 'alignGain', 'rangeGain',
                    'maxSpeed', 'maxSidewaysSpeed', 'yawGain', 'minAlignment',
                    'desiredRange', 'rangeThreshold'])

    def BARBED_WIRE_FOUND(self, event):
        # Publish aligned event if needed
        range = 1 - event.topWidth
        rangeError = math.fabs(range - self._desiredRange)

        inAlignment = (math.fabs(event.topX) < self._xThreshold) and \
            (math.fabs(event.topY) < self._yThreshold)
        orientation = self.vehicle.getOrientation()
        alignment = orientation.getYaw().valueDegrees() - \
            self._desiredOrientation
        inOrientation = alignment < self._orientationThreshold
        inRange = rangeError < self._rangeThreshold

        if inAlignment and inRange and inOrientation:
            self.publish(SeekingToAligned.ALIGNED, core.Event())
            
        # Update motion
        TargetAlignState.BARBED_WIRE_FOUND(self, event)
        
    def enter(self, minAlignment = 3, desiredRange = 0.5,
              rangeThreshold = 0.05):
        TargetAlignState.enter(self)
        
        # Record threshold
        self._xThreshold = self._config.get('xThreshold', 0.1)
        self._yThreshold = self._config.get('yThreshold', 0.1)
        self._orientationThreshold = self._config.get('orientationThreshold', 2)
        self._desiredRange = self._config.get('desiredRange', desiredRange)
        self._rangeThreshold = self._config.get('rangeThreshold', rangeThreshold)
        
class AlignedFindAttempt(FindAttempt):
    @staticmethod
    def transitions():
        return FindAttempt.transitions(SeekingToAligned)

class SeekingToAligned2(TargetAlignState, state.State, StoreBarbedWireEvent):
    """
    Holds the target at range and in the center of the field of view while 
    rotating around it.  If its rotating the wrong direction is will reverse
    that direction and keep going until its close enough to aligned.
    """
    ALIGNED = core.declareEventType('ALIGNED')
    CHECK_DIRECTION = core.declareEventType('CHECK_DIRECTION_')
    
    @staticmethod
    def transitions():
        return { vision.EventType.BARBED_WIRE_FOUND : SeekingToAligned2,
                 vision.EventType.BARBED_WIRE_LOST : FindAttempt,
                 SeekingToAligned.CHECK_DIRECTION : SeekingToAligned2,
                 SeekingToAligned.ALIGNED : Aligning }

    def BARBED_WIRE_FOUND(self, event):
        # Publish aligned event if needed
        alignment = math.fabs(event.topX) + math.fabs(event.bottomX)
        if alignment < self._minAlignment:
            self.publish(SeekingToAligned.ALIGNED, core.Event())
            
        # Update motion
        TargetAlignState.BARBED_WIRE_FOUND(self, event)
        
    def enter(self):
        TargetAlignState.enter(self)
        
        # Record threshold
        self._minAlignment = self._config.get('minAlignment', 0.1)
        
    
# The next two states are extra states, not sure if they are needed    
class Aligning(TargetAlignState, state.State):
    SETTLED = core.declareEventType('ALIGNED')
       
    @staticmethod
    def transitions():
        return { vision.EventType.BARBED_WIRE_FOUND : Aligning,
                 vision.EventType.BARBED_WIRE_LOST : AligningFindAttempt,
                 Aligning.SETTLED : Through }

    @staticmethod
    def getattr():
        return set(['yZero', 'depthGain', 'maxDepthDt', 'desiredRange',
                    'maxRangeDiff', 'maxAlignDiff', 'alignGain', 'maxSpeed',
                    'maxSidewaysSpeed', 'yawGain', 'threshold', 'settleTime'])

    def BARBED_WIRE_FOUND(self, event):
        TargetAlignState.BARBED_WIRE_FOUND(self, event)

        if event.bottomWidth != -1:
            if (math.fabs(event.topX) < self._threshold) and \
                    (math.fabs(event.bottomX) < self._threshold):
                if self.timer is not None:
                    self.timer.stop()
                self.publish(Aligning.SETTLED, core.Event())

    def enter(self):
        TargetAlignState.enter(self)
        
        self._threshold = self._config.get('threshold', 0.05)

        self.timer = self.timerManager.newTimer(
            Aligning.SETTLED, self._config.get('settleTime', 15))
        self.timer.start()
        
    def exit(self):
        TargetAlignState.exit(self)
        self.timer.stop()

class AligningFindAttempt(FindAttempt):
    @staticmethod
    def transitions():
        return FindAttempt.transitions(Aligning)
        
class Through(state.State, StoreBarbedWireEvent):
    @staticmethod
    def getattr():
        return set(['forwardTime'])
    
    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.QUEUED_MOTIONS_FINISHED : End }

    def enter(self):
        self.visionSystem.barbedWireDetectorOff()

        self._depth = self._config.get('diveDepth', 2)
        self._speed = self._config.get('speed', 3)
        self._duration = self._config.get('duration', 7)

        currentDepth = self.vehicle.getDepth()

        self._diveMotion = motion.basic.RateChangeDepth(
            desiredDepth = currentDepth + self._depth, speed = (1.0/3.0))
        self._forwardMotion = motion.basic.TimedMoveDirection(
            desiredHeading = 0, speed = self._speed,
            duration = self._duration, absolute = False)

        self.motionManager.setQueuedMotions(self._diveMotion,
                                            self._forwardMotion)

    def exit(self):
        self.motionManager.stopCurrentMotion()
        
class End(state.State):
    def enter(self):
        self.visionSystem.barbedWireDetectorOff()
        self.publish(COMPLETE, core.Event())
