# Copyright (C) 2010 Maryland Robotics Club
# Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
# All rights reserved.
#
# Author: Jonathan Sternberg <jsternbe@umd.edu>
# File:  packages/python/ram/ai/hedge.py
  
"""
A state machine for moving through the U-shaped hedge.

 - Find the hedge
 - Align to it
 - Move straight through
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

class StoreHedgeEvent(object):
    """
    Common subclass for states that have a LIGHT_FOUND transition, it stores 
    the event in ai.data.
    """
    def HEDGE_FOUND(self, event):
        self.ai.data['lastHedgeEvent'] = event

class FilteredState(object):
    """
    Provides filter on the inputs from the vision system
    """
    @staticmethod
    def getattr():
        return set(['filterSize'])

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
        self._alignFilter.append(event.squareNess - 2)
        
        # Get new result
        self._filterdX = self._xFilter.getAverage()
        self._filterdY = self._yFilter.getAverage()
        self._filterdRange = self._rangeFilter.getAverage()
        self._filterdAlign = self._alignFilter.getAverage()

class RangeXYHold(FilteredState, state.State, StoreHedgeEvent):
    """
    Base state that holds the hedge centered in X, Y and depth without yawing.
    It will also throw off an event whenever it is within range and centering
    tolerances.
    """
    
    IN_RANGE = core.declareEventType('IN_RANGE')
    
    @staticmethod   
    def transitions(myState = None, trans = None, lostState = None):
        if myState is None:
            myState = RangeXYHold
        if lostState is None:
            lostState = FindAttempt
        if trans is None:
            trans = {}
        trans.update({vision.EventType.HEDGE_FOUND : myState,
                      vision.EventType.HEDGE_LOST : lostState })
        return trans

    @staticmethod
    def getattr():
        return set(['rangeThreshold', 'frontThreshold', 'alignmentThreshold',
                    'depthGain', 'iDepthGain', 'dDepthGain', 'maxDepthDt',
                    'desiredRange', 'maxRangeDiff', 'maxSpeed',
                    'translateGain']).union(FilteredState.getattr())

    def HEDGE_FOUND(self, event):
        """Update the state of the hedge, this moves the vehicle"""
        StoreHedgeEvent.HEDGE_FOUND(self, event)
        self._updateFilters(event)
        
        y = self._filterdY
        if 0 == self._depthGain:
            y = 0
        
        # We ignore azimuth and elevation because we aren't using them
        self._hedge.setState(0, 0, self._filterdRange, self._filterdX, self._filterdY, event.timeStamp)
        
        # Only triggered the in range event if we are close and the hedge is
        # centered in the field of view
        rangeError = math.fabs(self._filterdRange - self._desiredRange)
        frontDistance = math.sqrt(self._filterdX ** 2 + y ** 2)
        if (rangeError < self._rangeThreshold) and \
            (frontDistance < self._frontThreshold):
            self.publish(SeekingToRange.IN_RANGE, core.Event())
        
    def enter(self, defaultDepthGain = 1.5):
        FilteredState.enter(self)
        
        # Ensure vision system is on
        self.visionSystem.hedgeDetectorOn()
        
        # Create tracking object
        self._hedge = ram.motion.seek.PointTarget(0, 0, 0, 0, 0,
                                                   timeStamp = None,
                                                   vehicle = self.vehicle)
        
        # Read in configuration settings
        self._rangeThreshold = self._config.get('rangeThreshold', 0.05)
        self._frontThreshold = self._config.get('frontThreshold', 0.15)
        alignmentThreshold = self._config.get('alignmentThreshold', 0.1)
        self._depthGain = self._config.get('depthGain', defaultDepthGain)
        iDepthGain = self._config.get('iDepthGain', 0)
        dDepthGain = self._config.get('dDepthGain', 0.75)
        maxDepthDt = self._config.get('maxDepthDt', 0.3)
        self._desiredRange = self._config.get('desiredRange', 0.5)
        maxRangeDiff = self._config.get('maxRangeDiff', 0.1)
        maxSpeed = self._config.get('maxSpeed', 1)
        translateGain = self._config.get('translateGain', 2)
        
        motion = ram.motion.seek.SeekPointToRange(target = self._hedge,
            alignmentThreshold = alignmentThreshold,
            desiredRange = self._desiredRange,
            maxRangeDiff = maxRangeDiff,
            maxSpeed = maxSpeed,
            depthGain = self._depthGain,
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
    Gets the vehicle to the proper depth, so it can acquire the hedge.
    """
    
    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Searching }

    @staticmethod
    def getattr():
        return set(['diveSpeed'])
    
    def enter(self):
        # Store the initial orientation
        orientation = self.vehicle.getOrientation()
        self.ai.data['hedgeStartOrientation'] = \
            orientation.getYaw().valueDegrees()

        # Set the dive motion
        diveMotion = motion.basic.RateChangeDepth(
            desiredDepth = self.ai.data['config'].get('hedgeDepth', 5),
            speed = self._config.get('diveSpeed', 1.0/3.0))
        self.motionManager.setMotion(diveMotion)

        self.ai.data['firstSearching'] = True
        
    def exit(self):
        self.motionManager.stopCurrentMotion()

class FindAttempt(state.FindAttempt, StoreHedgeEvent):
    """
    Attempt to find the hedge, before starting search pattern.  This will
    catch the object if its front of the vehicle.
    """

    @staticmethod
    def transitions(myState = None):
        if myState is None:
            myState = SeekingToCentered
        return state.FindAttempt.transitions(vision.EventType.HEDGE_FOUND,
                                       myState, Recover)
        
    def enter(self):
        state.FindAttempt.enter(self)
        self.visionSystem.hedgeDetectorOn()

class FindAttemptRange(FindAttempt):
    @staticmethod
    def transitions():
        return FindAttempt.transitions(myState = SeekingToRange)

class FindAttemptAligned(FindAttempt):
    @staticmethod
    def transitions():
        return FindAttempt.transitions(myState = SeekingToAligned)

class Recover(state.FindAttempt, StoreHedgeEvent):
    """
    TODO
    """
    @staticmethod
    def transitions():
        return state.FindAttempt.transitions(vision.EventType.HEDGE_FOUND,
                                             SeekingToCentered, Searching)

    @staticmethod
    def getattr():
        return state.FindAttempt.getattr()

    def enter(self, timeout = 3):
        state.FindAttempt.enter(self)
        self.visionSystem.hedgeDetectorOn()

        event = self.ai.data.get('lastHedgeEvent', None)
        
        self._recoverMethod = "Default"
        
        if event is None:
            # If there was no event, do this
            self._recoverMethod = "None"
            self.motionManager.stopCurrentMotion()
        elif False:
            # Fill in other conditions
            pass
        else:
            # Otherwise, wait for a symbol before continuing
            self.motionManager.stopCurrentMotion()

class Searching(state.State, StoreHedgeEvent):
    """
    Runs a zig-zag search pattern until it finds the duct
    """
    
    @staticmethod
    def transitions():
        return { vision.EventType.HEDGE_FOUND : SeekingToCentered }

    @staticmethod
    def getattr():
        return set(['legTime', 'sweepAngle', 'speed'])

    def enter(self):
        # Make sure the detector is on the vision system
        self.visionSystem.hedgeDetectorOn()

        # Set the start orientation if it isn't already set
        orientation = self.vehicle.getOrientation()
        direction = self.ai.data.setdefault('hedgeStartOrientation',
                                orientation.getYaw().valueDegrees())

        # Create the forward motion
        self._duration = self.ai.data['config'].get('Hedge', {}).get(
            'forwardDuration', 2)
        self._forwardSpeed = self.ai.data['config'].get('Hedge', {}).get(
            'forwardSpeed', 3)
        self._forwardMotion = motion.basic.TimedMoveDirection(
            desiredHeading = 0,
            speed = self._forwardSpeed,
            duration = self._duration,
            absolute = False)

        # Create zig zag search to
        legTime = self._config.get('legTime', 5)
        sweepAngle = self._config.get('sweepAngle', 30)
        speed = self._config.get('speed', 2.5)
 
        self._zigZag = motion.search.ForwardZigZag(
            legTime = legTime,
            sweepAngle = sweepAngle,
            speed = speed,
            direction = direction)

        if self.ai.data.get('firstSearching', True) and self._duration > 0:
            self.motionManager.setMotion(self._forwardMotion,
                                         self._zigZag)
        else:
            self.motionManager.setMotion(self._zigZag)
        self.ai.data['firstSearching'] = False

    def exit(self):
        self.motionManager.stopCurrentMotion()

class SeekingToCentered(RangeXYHold):
    """
    Changes the depth and centers the hedge
    """

    _offset = 0.0
    _firstRun = True

    @staticmethod
    def transitions():
        return RangeXYHold.transitions(SeekingToCentered, {
            ram.motion.seek.SeekPoint.POINT_ALIGNED : SeekingToRange})
        
    def POINT_ALIGNED(self, event):
        """When aligned we are at a good depth so hold it"""
        if self._depthGain != 0:
            self.controller.holdCurrentDepth()
        
    def HEDGE_FOUND(self, event):
        """Update the state of the hedge, this moves the vehicle"""
        self._updateFilters(event)

        # To keep the range the same, the vehicle saves its first range
        # value and keeps the vehicle at this range
        if (self._firstRun):
            self._offset = self._filterdRange
            self._firstRun = False
        # TODO: take a close look at range seeking here
        # We ignore azimuth and elevation because we aren't using them
        self._hedge.setState(0, 0, self._filterdRange, #- self._offset +
                              #self._desiredRange, 
                              self._filterdX,
                              self._filterdY,
                              event.timeStamp)
        
class SeekingToRange(RangeXYHold):
    """
    Heads toward the hedge until it reaches the desired range
    """
    
    @staticmethod
    def transitions():
        return RangeXYHold.transitions(SeekingToRange, {
            RangeXYHold.IN_RANGE : SeekingToAligned },
                                       lostState = FindAttemptRange)

    def enter(self):
        self._config.setdefault('desiredRange', 5)
        RangeXYHold.enter(self)
        
class HedgeAlignState(FilteredState, StoreHedgeEvent):
    def HEDGE_FOUND(self, event):
        StoreHedgeEvent.HEDGE_FOUND(self, event)
        self._updateFilters(event)
        """Update the state of the light, this moves the vehicle"""
        azimuth = self._filterdX * -107.0/2.0
        elevation = self._filterdY * -80.0/2.0
        self._hedge.setState(azimuth, elevation, self._filterdRange, 
                              self._filterdX, self._filterdY, 
                              self._filterdAlign * self._alignSign,
                              event.timeStamp)

    @staticmethod
    def getattr():
        attr = set(['depthGain', 'iDepthGain', 'dDepthGain', 'maxDepthDt',
                    'desiredRange', 'maxRangeDiff', 'maxAlignDiff',
                    'alignGain', 'maxSpeed', 'maxSidewaysSpeed', 'yawGain'])
        return attr.union(FilteredState.getattr())
        
    def enter(self):
        FilteredState.enter(self)
        
        # Ensure vision system is on
        self.visionSystem.hedgeDetectorOn()
        
        # Create tracking object
        self._hedge = ram.motion.duct.Duct(0, 0, 0, 0, 0, 0,
                                           vehicle = self.vehicle)
        self._alignSign = 1
        
        # Read in configuration settings
        depthGain = self._config.get('depthGain', 1.5)
        iDepthGain = self._config.get('iDepthGain', 0)
        dDepthGain = self._config.get('dDepthGain', 0.75)
        maxDepthDt = self._config.get('maxDepthDt', 0.3)
        
        desiredRange = self._config.get('desiredRange', 5)
        maxRangeDiff = self._config.get('maxRangeDiff', 0.2)
        maxAlignDiff = self._config.get('maxAlignDiff', 0.5)
        alignGain = self._config.get('alignGain', 1.0)
        maxSpeed = self._config.get('maxSpeed', 0.75)
        maxSidewaysSpeed = self._config.get('maxSidewaysSpeed', 2)
        yawGain = self._config.get('yawGain', 1.0)

        motion = ram.motion.duct.DuctSeekAlign(target = self._hedge,
            desiredRange = desiredRange,
            maxRangeDiff = maxRangeDiff,
            maxAlignDiff = maxAlignDiff, 
            maxSpeed = maxSpeed,
            maxSidewaysSpeed = maxSidewaysSpeed,
            alignGain = alignGain,
            depthGain = depthGain,
            iDepthGain = iDepthGain,
            dDepthGain = dDepthGain,
            maxDepthDt = maxDepthDt,
            yawGain = yawGain)
        
        self.motionManager.setMotion(motion)
        
    def exit(self):
        self.motionManager.stopCurrentMotion()   
        
class SeekingToAligned(HedgeAlignState, state.State):
    """
    Holds the hedge at range and in the center of the field of view while 
    rotating around it.  If its rotating the wrong direction is will reverse
    that direction and keep going until its close enough to aligned.
    """
    ALIGNED = core.declareEventType('ALIGNED')
    CHECK_DIRECTION = core.declareEventType('CHECK_DIRECTION_')
    
    @staticmethod
    def transitions():
        return { vision.EventType.HEDGE_FOUND : SeekingToAligned,
                 vision.EventType.HEDGE_LOST : FindAttemptAligned,
                 SeekingToAligned.CHECK_DIRECTION : SeekingToAligned,
                 SeekingToAligned.ALIGNED : Aligning }

    @staticmethod
    def getattr():
        return set(['minSquareNess', 'checkDelay']).union(
            HedgeAlignState.getattr())

    def HEDGE_FOUND(self, event):
        # Update motion
        HedgeAlignState.HEDGE_FOUND(self, event)

        # Record first squareNess and every squareness
        squareNess = self._filterdAlign + 2
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
        HedgeAlignState.enter(self)

        self._firstEvent = True
        self._startSquareNess = None
        self._currentSquareNess = None
        
        # Start timer
        self._minSquareNess = self._config.get('minSquareNess', 1.9)
        self._delay = self._config.get('checkDelay', 3)
        self._timer = self.timerManager.newTimer(
            SeekingToAligned.CHECK_DIRECTION, self._delay)
        self._timer.start()
        
    def exit(self):
        HedgeAlignState.exit(self)
        self._timer.stop()
    
class Aligning(HedgeAlignState, state.State):
    SETTLED = core.declareEventType('ALIGNED')
       
    @staticmethod
    def transitions():
        return { vision.EventType.HEDGE_FOUND : Aligning,
                 vision.EventType.HEDGE_LOST : FindAttempt,
                 Aligning.SETTLED : Through }

    @staticmethod
    def getattr():
        return set(['settleTime']).union(HedgeAlignState.getattr())

    def enter(self):
        HedgeAlignState.enter(self)
        
        self.timer = self.timerManager.newTimer(
            Aligning.SETTLED, self._config.get('settleTime', 15))
        self.timer.start()
        
    def exit(self):
        HedgeAlignState.exit(self)
        self.timer.stop()
        
class Through(state.State):
    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : End }

    @staticmethod
    def getattr():
        return set(['duration', 'speed', 'distance'])

    def enter(self):
        self.visionSystem.hedgeDetectorOff()

        speed = self._config.get('speed', 3)
        duration = self._config.get('duration', 15)
        distance = self._config.get('distance', 5)
        heading = self.vehicle.getOrientation().getYaw(True).valueDegrees()
        
        # Assumes that the DVL is named 'DVL'
        if 'DVL' in self.vehicle.getDeviceNames():
            # Set the DVL motion
            m = motion.basic.MoveDistance(desiredHeading = heading,
                                          distance = distance,
                                          speed = speed)
        else:
            # No DVL, use a timed motion
            m = motion.basic.TimedMoveDirection(desiredHeading = heading,
                                                speed = speed,
                                                duration = duration)

        self.motionManager.setMotion(m)
        
    def exit(self):
        self.motionManager.stopCurrentMotion()
        
class End(state.State):
    def enter(self):
        self.publish(COMPLETE, core.Event())
