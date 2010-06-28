# Copyright (C) 2010 Maryland Robotics Club
# Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
# All rights reserved.
#
# Author: Jonathan Sternberg <jsternbe@umd.edu>
# File:  packages/python/ram/ai/window.py

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

class WindowTrackingState(state.State):
    """
    Common subclass for states that have a WINDOW_FOUND transition, it stores 
    the event in ai.data.
    """
    @staticmethod
    def transitions(foundState, lostState):
        return { vision.EventType.WINDOW_FOUND : foundState,
                 vision.EventType.WINDOW_LOST : lostState }

    @staticmethod
    def getattr():
        return set(['eventDelay'])

    def enter(self):
        self.ai.data.setdefault('windowData', {})

        # Load the current target color
        colorList = self.ai.data['config'].get('targetWindows',['red', 'green'])
        torpedosFired = self.ai.data.get('torpedosFired', 0)

        # Go to end state if there are no target windows
        colorString = colorList[self.ai.data.get('torpedosFired', 0)]
        
        # Convert the string into the appropriate attribute
        self._desiredColor = getattr(vision.Color, colorString.upper())
        self._eventDelay = self._config.get('eventDelay', 1000)

    def mergeWindows(self, event):
        windowData = self.ai.data['windowData']
        def recent_event(a):
            return (event.timeStamp - \
                        a.timeStamp) < self._eventDelay
        eventList = __builtins__['filter'](recent_event,
                                           windowData.values())
        
        eventList = map(lambda a: event if a.color == event.color else a,
                        eventList)
        
        # Add all variables
        x, y, range, squareNess = 0, 0, 0, 0
        for e in eventList:
            x += e.x
            y += e.y
            range += e.range
            squareNess += e.squareNess

        # Average the variables
        x /= len(eventList)
        y /= len(eventList)
        range /= len(eventList)
        squareNess /= len(eventList)

        return (x, y, range, squareNess)

    def WINDOW_FOUND(self, event):
        windowData = self.ai.data['windowData']
        windowData[event.color] = event

        # Veto the event if the color doesn't match the desired color
        if self._desiredColor != event.color:
            return False

    def WINDOW_LOST(self, event):
        if self._desiredColor != event.color:
            return False

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
        
    def _updateFilters(self, x, y, range, squareNess):
        # Add to filters
        self._xFilter.append(x)
        self._yFilter.append(y)
        self._rangeFilter.append(range)
        self._alignFilter.append(squareNess - 1)
        
        # Get new result
        self._filterdX = self._xFilter.getAverage()
        self._filterdY = self._yFilter.getAverage()
        self._filterdRange = self._rangeFilter.getAverage()
        self._filterdAlign = self._alignFilter.getAverage()

class RangeXYHold(FilteredState, WindowTrackingState):
    """
    Base state that holds the target centered in X, Y and depth without yawing.
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
        trans.update(WindowTrackingState.transitions(myState, lostState))
        return trans

    @staticmethod
    def getattr():
        attr = WindowTrackingState.getattr()
        attr.update(
            set(['rangeThreshold', 'frontThreshold', 'alignmentThreshold',
                 'depthGain', 'iDepthGain', 'dDepthGain', 'maxDepthDt',
                 'desiredRange', 'maxRangeDiff', 'maxSpeed',
                 'translateGain', 'filterSize']))
        return attr

    def WINDOW_FOUND(self, event):
        """Update the state of the target, this moves the vehicle"""
        # Check if this event should be vetoed, if it should, pass it on
        ret = WindowTrackingState.WINDOW_FOUND(self, event)
        if ret is False and not self._average:
            return ret

        # Average the found windows if specified
        if self._average:
            x, y, range, squareNess = self.mergeWindows(event)
        else:
            x, y, range, squareNess = \
                event.x, event.y, event.range, event.squareNess

        self._updateFilters(x, y, range, squareNess)
        
        y = self._filterdY
        if 0 == self._depthGain:
             y = 0
        
        # We ignore azimuth and elevation because we aren't using them
        self._target.setState(0, 0, self._filterdRange, self._filterdX, self._filterdY, event.timeStamp)
        
        # Only triggered the in range event if we are close and the target is
        # centered in the field of view
        rangeError = math.fabs(self._filterdRange - self._desiredRange)
        frontDistance = math.sqrt(self._filterdX ** 2 + y ** 2)
        if (rangeError < self._rangeThreshold) and \
            (frontDistance < self._frontThreshold):
            self.publish(SeekingToRange.IN_RANGE, core.Event())

        return ret
        
    def enter(self, defaultDepthGain = 1.5, average = False):
        FilteredState.enter(self)
        WindowTrackingState.enter(self)
        
        # Ensure vision system is on
        self.visionSystem.windowDetectorOn()
        self._average = average
        
        # Create tracking object
        self._target = ram.motion.seek.PointTarget(0, 0, 0, 0, 0,
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
        
        motion = ram.motion.seek.SeekPointToRange(target = self._target,
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
    Gets the vehicle to the proper depth, so it can acquire the target.
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
        self.ai.data['windowStartOrientation'] = \
            orientation.getYaw().valueDegrees()

        # Send a default value to windowOffset
        self.ai.data['windowOffset'] = \
            self.ai.data['config'].get('windowOffset', 0.3)

        # Set the dive motion
        diveMotion = motion.basic.RateChangeDepth(
            desiredDepth = self.ai.data['config'].get('windowDepth', 12),
            speed = self._config.get('diveSpeed', 1.0/3.0))
        self.motionManager.setMotion(diveMotion)

        self.ai.data['firstSearching'] = True
        
    def exit(self):
        self.motionManager.stopCurrentMotion()

class FindAttempt(state.FindAttempt, WindowTrackingState):
    """
    Attempt to find the target, before starting search pattern.  This will
    catch the object if its front of the vehicle.
    """

    @staticmethod
    def transitions(myState = None):
        if myState is None:
            myState = SeekingToCentered
        return state.FindAttempt.transitions(vision.EventType.WINDOW_FOUND,
                                       myState, Recover)
        
    def enter(self):
        state.FindAttempt.enter(self)
        WindowTrackingState.enter(self)
        self.visionSystem.windowDetectorOn()

class FindAttemptRange(FindAttempt):
    @staticmethod
    def transitions():
        return FindAttempt.transitions(myState = SeekingToRange)

class FindAttemptAligned(FindAttempt):
    @staticmethod
    def transitions():
        return FindAttempt.transitions(myState = SeekingToAligned)

class FindAttemptFireTorpedos(FindAttempt):
    @staticmethod
    def transitions():
        return FindAttempt.transitions(myState = FireTorpedos)

class Recover(state.FindAttempt, WindowTrackingState):
    """
    Move backwards if we were too close
    """
    @staticmethod
    def transitions():
        return state.FindAttempt.transitions(vision.EventType.WINDOW_FOUND,
                                             SeekingToCentered, Searching)

    @staticmethod
    def getattr():
        return set(['reverseSpeed', 'closeRangeThreshold']).union(
            state.FindAttempt.getattr())

    def enter(self, timeout = 3):
        state.FindAttempt.enter(self)
        WindowTrackingState.enter(self)
        self.visionSystem.windowDetectorOn()

        event = self.ai.data.get('lastTargetEvent', None)
        
        self._recoverMethod = "Default"
        
        # Load the thresholds for searching
        self._reverseSpeed = self._config.get('reverseSpeed', 3)
        self._closeRangeThreshold = self._config.get('closeRangeThreshold', 0.5)
        
        if event is None:
            # If there was no event, do nothing
            self._recoverMethod = "None"
            self.motionManager.stopCurrentMotion()
        elif event.range < self._closeRangeThreshold:
            # If the range is very close, backup and change depth
            # Find the backwards direction and create the motion
            self._recoverMethod = "Close Range"
            self._recoverMotion = \
                motion.basic.MoveDirection(-180, self._reverseSpeed,
                                            absolute = False)
            
            # Set the backwards motion
            self.motionManager.setMotion(self._recoverMotion)
        else:
            # Otherwise, wait for a symbol before continuing
            self.motionManager.stopCurrentMotion()

class Searching(WindowTrackingState):
    """
    Runs a zig-zag search pattern until it finds the correct colored window
    """
    @staticmethod
    def transitions():
        return { vision.EventType.WINDOW_FOUND : Approach }

    @staticmethod
    def getattr():
        return set(['legTime', 'sweepAngle', 'speed'])

    def WINDOW_FOUND(self, event):
        WindowTrackingState.WINDOW_FOUND(self, event)
        # Check the color of the target
        if event.color != self._desiredColor:
            # Veto the event (since it's not the color we want)
            return False

        # TODO: Advanced checking if the vehicle is too close to the window

    def enter(self):
        WindowTrackingState.enter(self)

        # Make sure the detector is on the vision system
        self.visionSystem.windowDetectorOn()

        # Set the start orientation if it isn't already set
        orientation = self.vehicle.getOrientation()
        direction = self.ai.data.setdefault('windowStartOrientation',
                                orientation.getYaw().valueDegrees())

        # Create the forward motion
        self._duration = self.ai.data['config'].get('Window', {}).get(
            'forwardDuration', 2)
        self._forwardSpeed = self.ai.data['config'].get('Window', {}).get(
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

class Approach(RangeXYHold):
    """
    Approaches the windows until it's close enough to go
    after a specific window.
    """
    @staticmethod
    def transitions():
        return RangeXYHold.transitions(Approach,
                                       { RangeXYHold.IN_RANGE : ApproachAligning },
                                       Searching)

    def IN_RANGE(self, event):
        """
        Makes sure that the desired window has been found before continuing
        """
        windowData = self.ai.data['windowData']
        if self._desiredColor not in windowData:
            return False

    def enter(self):
        RangeXYHold.enter(self, average = True)

class CorrectHeight(WindowTrackingState):
    """
    Corrects the vehicle height to move upward or downward
    by a predetermined amount to match the currently seeked target
    """
    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : SeekingToCentered,
                 vision.EventType.WINDOW_LOST : Searching }

    @staticmethod
    def getattr():
        return set(['offset', 'speed'])

    def enter(self):
        WindowTrackingState.enter(self)
        depth = self.ai.data['config'].get('windowDepth', 12)
        offset = self._config.get('offset', 0.3)

        y = self.ai.data['windowData'][self._desiredColor].y
        # If y position is above, use a negative offset
        if y > 0.0:
            offset = -offset;

        m = motion.basic.RateChangeDepth(depth + offset,
                                         self._config.get('speed', 1.0/3.0))
        self.motionManager.setMotion(m)

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
        if self._depthGain != 0:
            self.controller.holdCurrentDepth()
        
    # def WINDOW_FOUND(self, event):
    #     """Update the state of the target, this moves the vehicle"""
    #     ret = RangeXYHold.WINDOW_FOUND(self, event)
    #     if ret is False:
    #         return ret

    #     self._updateFilters(event)

    #     # To keep the range the same, the vehicle saves its first range
    #     # value and keeps the vehicle at this range
    #     if (self._firstRun):
    #         self._offset = self._filterdRange
    #         self._firstRun = False
    #     # TODO: take a close look at range seeking here
    #     # We ignore azimuth and elevation because we aren't using them
    #     self._target.setState(0, 0, self._filterdRange, #- self._offset +
    #                           #self._desiredRange, 
    #                           self._filterdX,
    #                           self._filterdY,
    #                           event.timeStamp)
        
class SeekingToRange(RangeXYHold):
    """
    Heads toward the target until it reaches the desired range
    """
    
    @staticmethod
    def transitions():
        return RangeXYHold.transitions(SeekingToRange, {
            RangeXYHold.IN_RANGE : SeekingToAligned },
                                       lostState = FindAttemptRange)
        
class FireTorpedos(RangeXYHold):
    """
    Fires the two torpedos at the target, with 1 second in between each, but
    only if the target is within the desired bounds.
    """

    NUMBER_TORPEDOS = 2
    ARM_TORPEDOS = core.declareEventType('ARM_TORPEDOS_')
    MOVE_ON = core.declareEventType('MOVE_ON')
    REPOSITION = core.declareEventType('REPOSITION')
    MISALIGNED = core.declareEventType('MISALIGNED')

    @staticmethod
    def transitions():
        return RangeXYHold.transitions(FireTorpedos, {
            RangeXYHold.IN_RANGE : FireTorpedos,
            FireTorpedos.ARM_TORPEDOS: FireTorpedos,
            FireTorpedos.MOVE_ON : End,
            FireTorpedos.REPOSITION : Reposition,
            FireTorpedos.MISALIGNED : SeekingToAligned },
            lostState = FindAttemptFireTorpedos)

    @staticmethod
    def getattr():
        return set(['fireDelay', 'minSquareNess', 'startFireDelay']).union(
            RangeXYHold.getattr())

    def IN_RANGE(self, event):
        """
        Fires the torpedos only when armed, disarms the torpedos afterward so
        that next will be fired only after the delay period.
        """
        if self._armed and self.ai.data.get('torpedosFired', 0) \
                < FireTorpedos.NUMBER_TORPEDOS:
            squareNess = self._filterdAlign + 1
            if squareNess > self._minSquareNess:
                self.vehicle.fireTorpedo()
            else:
                self.publish(FireTorpedos.MISALIGNED, core.Event())
                return

            # Disarm torpedos, must be armed again, after a wait for the current
            # torpedo to get through
            self._armed = False

            # Increment number of torpedos fired
            fireCount = self.ai.data.get('torpedosFired', 0)
            self.ai.data['torpedosFired'] = fireCount + 1

            # Check which state we move to next
            self._check()

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
        self._minSquareNess = self._config.get('minSquareNess', 0.85)

        delay = self._config.get('startDelay', 0)
        if delay > 0:
            self._timer = self.timerManager.newTimer(FireTorpedos.ARM_TORPEDOS,
                                                     delay)
            self._timer.start()
        else:
            self._armed = True

    def exit(self):
        if self._timer is not None:
            self._timer.stop()

    @property
    def armed(self):
        return self._armed
    
    def _check(self):
        """
        Initiates a timer which arms the firing of a torpedo after the desired
        delay
        """
        if self.ai.data.get('torpedosFired', 0) < FireTorpedos.NUMBER_TORPEDOS:
            # Restart with the next window
            self.publish(FireTorpedos.REPOSITION, core.Event())
        else:
            # All torpedos fired, lets get out of this state
            self.publish(FireTorpedos.MOVE_ON, core.Event())
        
class TargetAlignState(FilteredState, WindowTrackingState):
    def WINDOW_FOUND(self, event):
        ret = WindowTrackingState.WINDOW_FOUND(self, event)
        if ret is False and self._average is False:
            return ret

        if self._average:
            x, y, range, squareNess = \
                self.mergeWindows(event)
        else:
            x, y, range, squareNess = \
                event.x, event.y, event.range, event.squareNess

        self._updateFilters(event.x, event.y, event.range, event.squareNess)
        """Update the state of the light, this moves the vehicle"""
        azimuth = self._filterdX * -107.0/2.0
        elevation = self._filterdY * -80.0/2.0
        self._target.setState(azimuth, elevation, self._filterdRange,
                              self._filterdX, self._filterdY,
                              self._filterdAlign * self._alignSign,
                              event.timeStamp)
        
    def enter(self, average = False):
        FilteredState.enter(self)
        
        # Ensure vision system is on
        self.visionSystem.windowDetectorOn()
        
        # Create tracking object
        self._target = ram.motion.duct.Duct(0, 0, 0, 0, 0, 0,
                                            vehicle = self.vehicle)
        self._average = average
        self._alignSign = 1
        
        # Read in configuration settings
        depthGain = self._config.get('depthGain', 1.5)
        iDepthGain = self._config.get('iDepthGain', 0)
        dDepthGain = self._config.get('dDepthGain', 0.75)
        maxDepthDt = self._config.get('maxDepthDt', 0.3)
        
        desiredRange = self._config.get('desiredRange', 0.5)
        maxRangeDiff = self._config.get('maxRangeDiff', 0.1)
        maxAlignDiff = self._config.get('maxAlignDiff', 0.5)
        alignGain = self._config.get('alignGain', 1.0)
        maxSpeed = self._config.get('maxSpeed', 0.75)
        maxSidewaysSpeed = self._config.get('maxSidewaysSpeed', 2)
        yawGain = self._config.get('yawGain', 1.0)

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
            maxDepthDt = maxDepthDt,
            yawGain = yawGain)
        
        self.motionManager.setMotion(motion)
        
    def exit(self):
        self.motionManager.stopCurrentMotion()
        
class SeekingToAligned(TargetAlignState):
    """
    Holds the target at range and in the center of the field of view while
    rotating around it.  If its rotating the wrong direction is will reverse
    that direction and keep going until its close enough to aligned.
    """
    ALIGNED = core.declareEventType('ALIGNED')
    CHECK_DIRECTION = core.declareEventType('CHECK_DIRECTION_')
    
    @staticmethod
    def transitions():
        return { vision.EventType.WINDOW_FOUND : SeekingToAligned,
                 vision.EventType.WINDOW_LOST : FindAttemptAligned,
                 SeekingToAligned.CHECK_DIRECTION : SeekingToAligned,
                 SeekingToAligned.ALIGNED : FireTorpedos }

    @staticmethod
    def getattr():
        return set(['depthGain', 'iDepthGain', 'dDepthGain', 'maxDepthDt',
                    'desiredRange', 'maxRangeDiff', 'maxAlignDiff',
                    'alignGain', 'maxSpeed', 'maxSidewaysSpeed', 'yawGain',
                    'minSquareNess', 'checkDelay', 'filterSize'])

    def WINDOW_FOUND(self, event):
        # Filter out undesired colors and upate motion (if necessary)
        ret = TargetAlignState.WINDOW_FOUND(self, event)
        if ret is False and self._average is False:
            return ret

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
            
    def enter(self, average = False):
        TargetAlignState.enter(self, average)
        WindowTrackingState.enter(self)

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

class ApproachAligning(SeekingToAligned):
    """
    Aligns to the targets while approaching them all as a group.
    """
    @staticmethod
    def transitions():
        trans = SeekingToAligned.transitions()
        trans.update({ SeekingToAligned.ALIGNED : CorrectHeight })
        return trans

    def enter(self):
        SeekingToAligned.enter(self, average = True)

class Reposition(state.State):
    """
    Moves the vehicle backwards to reposition it for looking for the
    targets.
    """
    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Searching }

    @staticmethod
    def getattr():
        return set(['speed', 'duration', 'diveSpeed'])

    def enter(self):
        # Don't pay attention to the detector while moving backwards
        self.visionSystem.windowDetectorOff()

        # Reset variables for searching
        self.ai.data['windowStartOrientation'] = \
            self.vehicle.getOrientation().getYaw().valueDegrees()
        self.ai.data['firstSearching'] = True

        speed = self._config.get('speed', 3)
        duration = self._config.get('duration', 5)
        backwardsMotion = motion.basic.TimedMoveDirection(desiredHeading = 180,
                                                         speed = speed,
                                                         duration = 5,
                                                         absolute = False)

        depth = self.ai.data['config'].get('windowDepth', 12)
        diveSpeed = self._config.get('diveSpeed', (1.0/3.0))
        diveMotion = motion.basic.RateChangeDepth(desiredDepth = depth,
                                                  speed = diveSpeed)

        self.motionManager.setMotion(backwardsMotion, diveMotion)

class End(state.State):
    def enter(self):
        self.publish(COMPLETE, core.Event())
