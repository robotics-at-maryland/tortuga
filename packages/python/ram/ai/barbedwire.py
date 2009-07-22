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
    def transitions(myState = None, trans = None):
        if myState is None:
            myState = RangeXYHold
        if trans is None:
            trans = {}
        trans.update({vision.EventType.BARBED_WIRE_FOUND : myState,
                      vision.EventType.BARBED_WIRE_LOST : FindAttempt })
        return trans
        
    def BARBED_WIRE_FOUND(self, event):
        """Update the state of the target, this moves the vehicle"""
        # Todo: consider filter removal
        #self._updateFilters(event)
        
        # Determine y value based on whether we are ignoring depth or not, 
        # also take into account any offset we wish to hold the object at.
        StoreBarbedWireEvent.BARBED_WIRE_FOUND(self, event)
        y = 0
        if self._depthGain != 0:
            y = event.topY - self._yZero
        
        # Width == 1 when we are close, so inversion is needed to get range 
        range = 1 - event.topWidth
        
        # Finally set the state (We ignore azimuth and elevation because we 
        # aren't using them)
        self._target.setState(0, 0, range = range, x = event.topX, y = y)
        
        # Only triggered the in range event if we are close and the target is
        # centered in the field of view
        rangeError = math.fabs(range - self._desiredRange)
        #print "RE",rangeError,"AR",rge,"DR",self._desiredRange
        frontDistance = math.sqrt(event.topX ** 2 + y ** 2)
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
        return { motion.basic.Motion.FINISHED : FindAttempt }
    
    def enter(self):
        # Set the initial direction
        orientation = self.vehicle.getOrientation()
        self.ai.data['barbedWireStartOrientation'] = \
            orientation.getYaw().valueDegrees()

        # Create the dive motion and start the vehicle
        diveMotion = motion.basic.RateChangeDepth(
            desiredDepth = self.ai.data['config'].get('bwireDepth', 12),
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
    def transitions():
        return state.FindAttempt.transitions(vision.EventType.BARBED_WIRE_FOUND,
                                             FarSeekingToRange, Searching)
        
    def enter(self):
        state.FindAttempt.enter(self)
        self.visionSystem.barbedWireDetectorOn()

class Searching(state.State, StoreBarbedWireEvent):
    """
    Runs a zig-zag search pattern until it finds the duct
    """
    
    @staticmethod
    def transitions():
        return { vision.EventType.BARBED_WIRE_FOUND : FarSeekingToRange }

    def enter(self):
        # Make sure the detector is on the vision system
        self.visionSystem.barbedWireDetectorOn()

        orientation = self.vehicle.getOrientation()
        direction = self.ai.data.setdefault('barbedWireStartOrientation',
                                orientation.getYaw().valueDegrees())

        # Create the forward motion
        self._duration = self._config.get('duration', 2)
        self._forwardSpeed = self._config.get('forwardSpeed', 3)
        self._forwardMotion = motion.basic.TimedMoveDirection(
            desiredHeading = 0,
            speed = self._forwardSpeed,
            duration = self._duration,
            absolute = False)

        # Create zig zag search to 
        self._zigZag = motion.search.ForwardZigZag(
            legTime = 15,
            sweepAngle = 60,
            speed = 2.5,
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
    def transitions(myState, inRangeState):
        return RangeXYHold.transitions(myState, {
            RangeXYHold.IN_RANGE : inRangeState })

    def BARBED_WIRE_FOUND(self, event):
        """
        This attempts to find side by side pipes, and if it does, it forces
        """
        StoreBarbedWireEvent.BARBED_WIRE_FOUND(self, event)
        
        # Determine if we aren't well aligned (ie. both pipes are beside each 
        # other and there is no overlap)
        wellAligned = True
        
        # Only run the check if the total width is low enough
        haveBottomPipe = event.bottomWidth != -1
        totalWidth = event.topWidth + event.bottomWidth
        smallEnoughWidth = totalWidth < self._maxAlignCheckWidth
        if haveBottomPipe and smallEnoughWidth:
            # Determine the start end and of the pipes on screen
            leftSt = event.topX - event.topWidth/2.0 
            leftEnd = event.topX + event.topWidth/2.0
            rightSt = event.bottomX - event.bottomWidth/2.0 
            rightEnd = event.bottomX + event.bottomWidth/2.0
        
            if event.topX > event.bottomX:
                # We assumed line direction wrong, swap it
                leftSt, rightSt = rightSt, leftSt 
                leftEnd, rightEnd = rightEnd, leftEnd
               
            # Find the amount of overlap
            overlap = (leftEnd - rightSt) / totalWidth
            
            # Determine if there is so little overlap we really have side by
            # side pipes
            if overlap < self._maxOverlap:
                wellAligned = False
        
        # If we aren't well aligned go the alignment phase, other wise continue
        # as normal
        if wellAligned:
            RangeXYHold.BARBED_WIRE_FOUND(self, event)
        else:
            self.publish(SeekingToRange.IN_RANGE, core.Event())    

    def enter(self, maxAlignCheckWidth = 0.7, maxOverlap = 0.1):
        # Do the normal enter functions
        RangeXYHold.enter(self)
        
        # Load config options
        self._maxAlignCheckWidth = self._config.get('maxAlignCheckWidth',
                                                    maxAlignCheckWidth)
        self._maxOverlap = self._config.get('maxOverlap', maxOverlap)
        
class FarSeekingToRange(SeekingToRange):
    @staticmethod
    def transitions():
        return SeekingToRange.transitions(myState = FarSeekingToRange,
                                          inRangeState = FarSeekingToAligned)
    
    def enter(self):
        SeekingToRange.enter(self, maxAlignCheckWidth = 0.7, maxOverlap = 0.1)
        
class CloseSeekingToRange(SeekingToRange):
    @staticmethod
    def transitions():
        return SeekingToRange.transitions(myState = CloseSeekingToRange,
                                          inRangeState = CloseSeekingToAligned)
        
    def enter(self):
        SeekingToRange.enter(self, maxAlignCheckWidth = 0.7, maxOverlap = 0.1)

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
        
        """Update the state of the light, this moves the vehicle"""
        azimuth = event.topX * -107.0/2.0
        elevation = event.topY * -80.0/2.0
        
        # Determine y value based on whether we are ignoring depth or not, 
        # also take into account any offset we wish to hold the object at.
        y = 0
        if self._depthGain != 0:
            y = event.topY - self._yZero
        
        range = 1 - event.topWidth
        alignment = event.bottomX - event.topX
        self._target.setState(azimuth, elevation, range = range,
                              x = event.topX, y = y, 
                              alignment = alignment)
        
    def enter(self):
        FilteredState.enter(self)
        
        # Ensure vision system is on
        self.visionSystem.barbedWireDetectorOn()
        
        # Read in configuration settings
        self._yZero = self._config.get('yZero', 0.5)
        self._depthGain = self._config.get('depthGain', 1)
        maxDepthDt = self._config.get('maxDepthDt', 0.3)
        desiredRange = self._config.get('desiredRange', 0.5)
        maxRangeDiff = self._config.get('maxRangeDiff', 0.2)
        maxAlignDiff = self._config.get('maxAlignDiff', 0.5)
        alignGain = self._config.get('alignGain', 1.0)
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
    def transitions(myState, alignedState):
        return { vision.EventType.BARBED_WIRE_FOUND : myState,
                 vision.EventType.BARBED_WIRE_LOST : FindAttempt,
                 SeekingToAligned.CHECK_DIRECTION : myState,
                 SeekingToAligned.ALIGNED : alignedState }

    def BARBED_WIRE_FOUND(self, event):
        # Publish aligned event if needed
        alignment = math.fabs(event.topX) + math.fabs(event.bottomX)
        range = 1 - event.topWidth        
        rangeError = math.fabs(range - self._desiredRange)

        inAlignment = alignment < self._minAlignment
        inRange = rangeError < self._rangeThreshold
        haveBottomPipe = event.bottomWidth != -1

        if haveBottomPipe and inAlignment and inRange:
            self.publish(SeekingToAligned.ALIGNED, core.Event())
            
        # Update motion
        TargetAlignState.BARBED_WIRE_FOUND(self, event)
        
    def enter(self, minAlignment = 0.1, desiredRange = 0.5,
              rangeThreshold = 0.05):
        TargetAlignState.enter(self)
        
        # Record threshold
        self._minAlignment = self._config.get('minAlignment', minAlignment)
        self._desiredRange = self._config.get('desiredRange', desiredRange)
        self._rangeThreshold = self._config.get('rangeThreshold', rangeThreshold)
        
class FarSeekingToAligned(SeekingToAligned):
    @staticmethod
    def transitions():
        return SeekingToAligned.transitions(FarSeekingToAligned,
                                            CloseSeekingToRange)
        
    def enter(self):
        SeekingToAligned.enter(self, minAlignment = 0.1, desiredRange = 0.5,
                               rangeThreshold = 0.05)
        
class CloseSeekingToAligned(SeekingToAligned):
    @staticmethod
    def transitions():
        return SeekingToAligned.transitions(CloseSeekingToAligned, Aligning)
    
    def enter(self):
        SeekingToAligned.enter(self, minAlignment = 0.1, desiredRange = 0.25,
                               rangeThreshold = 0.05)

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
                 vision.EventType.BARBED_WIRE_LOST : FindAttempt,
                 Aligning.SETTLED : Under }

    def enter(self):
        TargetAlignState.enter(self)
        
        self.timer = self.timerManager.newTimer(
            Aligning.SETTLED, self._config.get('settleTime', 15))
        self.timer.start()
        
    def exit(self):
        TargetAlignState.exit(self)
        self.timer.stop()

class Under(FilteredState, state.State, StoreBarbedWireEvent):
    @staticmethod
    def transitions():
        return { vision.EventType.BARBED_WIRE_LOST : Through,
                 vision.EventType.BARBED_WIRE_FOUND : Under }

    def BARBED_WIRE_FOUND(self, event):
        # Do not change the angle
        angle = ext.math.Degree(0)
        if event.topY <= self._maxy:
            self._bwire.setState(event.topX, event.topY, angle)
        else:
            if event.bottomWidth > 0 and event.bottomY <= self._maxy:
                self._bwire.setState(event.bottomX, event.bottomY, angle)

    def enter(self):
        # Use the same motion from AlongPipe
        self._bwire = ram.motion.pipe.Pipe(0, 0, 0)

        maxSpeed = self._config.get('forwardSpeed', 5)
        self._speedGain = self._config.get('speedGain', 5)
        self._maxSidewaysSpeed = self._config.get('maxSidewaysSpeed', 3)
        self._sidewaysSpeedGain = self._config.get('sidewaysSpeedGain', 1)
        self._yawGain = self._config.get('yawGain', 0)
        self._iSpeedGain = self._config.get('iSpeedGain', 0)
        self._dSpeedGain = self._config.get('dSpeedGain', 0)
        self._iSidewaysSpeedGain = self._config.get('iSidewaysSpeedGain', 0)
        self._dSidewaysSpeedGain = self._config.get('dSidewaysSpeedGain', 0)
        self._maxy = self._config.get('maxY', 0.8)

        motion = ram.motion.pipe.Follow(pipe = self._bwire,
                                        speedGain = self._speedGain,
                                        maxSpeed = maxSpeed,
                                        iSpeedGain = self._iSpeedGain,
                                        dSpeedGain = self._dSpeedGain,
                                        maxSidewaysSpeed = self._maxSidewaysSpeed,
                                        sidewaysSpeedGain = self._sidewaysSpeedGain,
                                        iSidewaysSpeedGain = self._iSidewaysSpeedGain,
                                        dSidewaysSpeedGain = self._dSidewaysSpeedGain,
                                        yawGain = self._yawGain)
        self.motionManager.setMotion(motion)
        
class Through(state.State):
    FORWARD_DONE = core.declareEventType('FORWARD_DONE')
    
    @staticmethod
    def transitions():
        return {Through.FORWARD_DONE : End}

    def enter(self):
        self.visionSystem.barbedWireDetectorOff()

        # Timer goes off in 3 seconds then sends off FORWARD_DONE
        self.timer = self.timerManager.newTimer(
            Through.FORWARD_DONE, self._config.get('forwardTime', 8))
        self.timer.start()
        self.controller.setSidewaysSpeed(0)
        self.controller.setSpeed(3)
    
    def exit(self):
        self.timer.stop()
        self.controller.setSpeed(0)
        
class End(state.State):
    def enter(self):
        self.publish(COMPLETE, core.Event())
