# Copyright (C) 2010 Maryland Robotics Club
# Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
# All rights reserved.
#
# Author: Jonathan Sternberg <jsternbe@umd.edu>
# File:  packages/python/ram/ai/buoy.py

# Standard imports
import math as pmath

# Project Imports
import ext.core as core
import ext.vision as vision
import ext.math as math

import ram.filter as filter
import ram.ai.state as state
import ram.ai.tracking as tracking
import ram.motion as motion
import ram.motion.search
import ram.motion.seek
import ram.timer

BUOY_HIT = core.declareEventType('BUOY_HIT')
COMPLETE = core.declareEventType('COMPLETE')


class BuoyTrackingState(state.State):
    """
    Set up for any state that tracks a buoy. Filters out incorrect
    buoy colors.
    """

    FINISHED = core.declareEventType('FINISHED')

    @staticmethod
    def transitions(foundState = None, lostState = None):
        return { vision.EventType.BUOY_FOUND : foundState,
                 vision.EventType.BUOY_LOST : lostState,
                 BuoyTrackingState.FINISHED : End }

    def BUOY_FOUND(self, event):
        # Store data and veto the event if the wrong color
        self.ai.data['buoyData'][event.color] = event
        if event.color != self._desiredColor:
            return False

    def BUOY_LOST(self, event):
        # Remove the stored data and veto the event if the wrong color
        #del self.ai.data['buoyData'][event.color]
        if event.color != self._desiredColor:
            return False

    def enter(self):
        self.visionSystem.buoyDetectorOn()

        self.ai.data.setdefault('buoyData', {})
        colorList = self.ai.data['config'].get('targetBuoys', [])
        buoysHit = self.ai.data.get('buoysHit', 0)

        # Error check that we have not overstepped the target buoys
        if buoysHit >= len(colorList):
            # Finish the state
            self._desiredColor = vision.Color.UNKNOWN
            self.publish(BuoyTrackingState.FINISHED, core.Event())
        else:
            # Get the desired color (case doesn't matter)
            self._desiredColor = getattr(vision.Color,
                                         colorList[buoysHit].upper())

class Start(state.State):
    """
    Does all the setup work for the buoy task. This checks how many
    buoys have been hit. It then sets the next target. If there are
    no remaining buoys, it skips to End.
    """
    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Searching }

    @staticmethod
    def getattr():
        return set(['speed'])
    
    def enter(self):
        # Store the initial orientation
        orientation = self.vehicle.getOrientation()
        self.ai.data['buoyStartOrientation'] = \
            orientation.getYaw().valueDegrees()

        # Go to 5 feet in 5 increments
        headingMotion = motion.basic.RateChangeHeading(
            desiredHeading = self.ai.data['buoyStartOrientation'],
            speed = 10)
        diveMotion = motion.basic.RateChangeDepth(
            desiredDepth = self.ai.data['config'].get('buoyDepth', 5),
            speed = self._config.get('speed', 1.0/3.0))
        self.motionManager.setMotion(headingMotion, diveMotion)

        self.ai.data['firstSearching'] = True
        
    def exit(self):
        #self.motionManager.stopCurrentMotion()
        pass

class Searching(BuoyTrackingState):
    @staticmethod
    def transitions():
        return BuoyTrackingState.transitions(Align, Searching)

    @staticmethod
    def getattr():
        return set(['legTime', 'sweepAngle', 'speed'])

    def enter(self):
        BuoyTrackingState.enter(self)

        # Turn on the vision system
        self.visionSystem.buoyDetectorOn()
        
        # Set the start orientation if it isn't already set
        orientation = self.vehicle.getOrientation()
        direction = self.ai.data.setdefault('buoyStartOrientation',
                                orientation.getYaw().valueDegrees())

        # Create the forward motion
        self._duration = self.ai.data['config'].get('Buoy', {}).get(
            'forwardDuration', 2)
        self._forwardSpeed = self.ai.data['config'].get('Buoy', {}).get(
            'forwardSpeed', 3)
        self._forwardMotion = motion.basic.TimedMoveDirection(
            desiredHeading = 0,
            speed = self._forwardSpeed,
            duration = self._duration,
            absolute = False)
        
        # Create zig zag search to 
        self._legTime = self._config.get('legTime', 15)
        self._sweepAngle = self._config.get('sweepAngle', 60)
        self._speed = self._config.get('speed', 2.5)
        self._zigZag = motion.search.ForwardZigZag(
            legTime = self._legTime,
            sweepAngle = self._sweepAngle,
            speed = self._speed,
            direction = direction)

        if self.ai.data.get('firstSearching', True) and self._duration > 0:
            self.motionManager.setMotion(self._forwardMotion,
                                         self._zigZag)
        else:
            self.motionManager.setMotion(self._zigZag)
        self.ai.data['firstSearching'] = False

    def exit(self):
        self.motionManager.stopCurrentMotion()

class FindAttempt(state.FindAttempt, BuoyTrackingState):
    @staticmethod
    def transitions(myState = None):
        if myState is None:
            myState = Align
        return state.FindAttempt.transitions(vision.EventType.BUOY_FOUND,
                                             myState, Recover)

    def BUOY_FOUND(self, event):
        return BuoyTrackingState.BUOY_FOUND(self, event)
        
    def enter(self):
        state.FindAttempt.enter(self, timeout = 2)
        BuoyTrackingState.enter(self)

class FindAttemptSeek(FindAttempt):
    @staticmethod
    def transitions():
        return FindAttempt.transitions(Seek)

class Recover(state.FindAttempt, BuoyTrackingState):
    REFOUND_BUOY = core.declareEventType('REFOUND_BUOY')
    
    @staticmethod
    def transitions():
        trans = state.FindAttempt.transitions(Recover.REFOUND_BUOY,
                                              Align, Searching)
        trans.update(BuoyTrackingState.transitions(Recover, Searching))
        trans.update({ motion.basic.MotionManager.FINISHED : Recover })
        
        return trans

    @staticmethod
    def getattr():
        return set(['yThreshold', 'reverseSpeed', 'advanceSpeed',
                    'closeDepthChange', 'depthChange', 'diveSpeed',
                    'yawChange', 'radius', 'closeRangeThreshold',
                    'farRangeThreshold']).union(state.FindAttempt.getattr())
    
    def FINISHED(self, event):
        self._finished = True
        
    def BUOY_FOUND(self, event):
        ret = BuoyTrackingState.BUOY_FOUND(self, event)
        if ret is False:
            return ret

        if self._recoverMethod == "Close Range":
            # Turn off the timer and backwards motion
            if self.timer is not None:
                self.timer.stop()
                self.timer = None

                self.motionManager._stopMotion(self._recoverMotion)
                
                # Create the depth motion if needed
                newDepth = self.vehicle.getDepth()
                changeDepth = False
                
                if event.y > self._yThreshold:
                    newDepth = newDepth - self._closeDepthChange
                    changeDepth = True
                elif event.y < (0.0 - self._yThreshold):
                    newDepth = newDepth + self._closeDepthChange
                    changeDepth = True
                
                # Start the depth motion if necessary
                if changeDepth:
                    self._diveMotion = motion.basic.RateChangeDepth(
                        desiredDepth = newDepth, speed = self._diveSpeed)
                    self.motionManager.setMotion(self._diveMotion)
                    self._finished = False
                else:
                    self._finished = True
            
            # Check if the motion is finished
            if self._finished:
                self.publish(Recover.REFOUND_BUOY, core.Event())
            
            # Check if we should finish it early
            if ((0.0 - self._yThreshold) < event.y < self._yThreshold):
                self.motionManager.stopCurrentMotion()
                self.controller.holdCurrentDepth()
                self.publish(Recover.REFOUND_BUOY, core.Event())
        else:
            self.publish(Recover.REFOUND_BUOY, core.Event())
        
    def enter(self):
        state.FindAttempt.enter(self, timeout = 4)
        BuoyTrackingState.enter(self)

        event = self.ai.data['buoyData'][self._desiredColor]
        
        self._recoverMethod = "Default"
        
        # Load the thresholds for searching
        self._yThreshold = self._config.get('yThreshold', 0.05)
        self._reverseSpeed = self._config.get('reverseSpeed', 4)
        self._advanceSpeed = self._config.get('advanceSpeed', 1)
        self._closeDepthChange = self._config.get('closeDepthChange', 1)
        self._depthChange = self._config.get('depthChange', 1)
        self._diveSpeed = self._config.get('diveSpeed', 0.3)
        self._yawChange = self._config.get('yawChange', 15)
        self._radius = self._config.get('radius', .7)
        self._closeRangeThreshold = self._config.get('closeRangeThreshold', 5)
        self._farRangeThreshold = self._config.get('farRangeThreshold', 8)
        
        if event is None:
            self._recoverMethod = "None"
            self.motionManager.stopCurrentMotion()
        else:
            vectorLength = math.Vector2(event.x, event.y).length()
            vehicleOrientation = \
                self.vehicle.getOrientation().getYaw().valueDegrees()

            if event.range < self._closeRangeThreshold:
                # If the range is very close, backup and change depth
                # Find the backwards direction and create the motion
                self._recoverMethod = "Close Range"
                self._recoverMotion = \
                    motion.basic.MoveDirection(-180, self._reverseSpeed,
                                                absolute = False)
            
                # Set the backwards motion
                self.motionManager.setMotion(self._recoverMotion)
                self._finished = False
            elif vectorLength > self._radius and event.range < \
                    self._farRangeThreshold:
                self._recoverMethod = "Mid Range"
                yawAngle = 0.0
                if event.x > self._radius:
                    yawAngle = (0.0 - self._yawChange)
                elif event.x < (0.0 - self._radius):
                    yawAngle = self._yawChange
                self.controller.yawVehicle(yawAngle)
            
                # Change the depth if it's outside on the y-axis
                newDepth = self.vehicle.getDepth()
                if event.y > self._radius:
                    newDepth = newDepth - self._depthChange
                elif event.y < (0.0 - self._radius):
                    newDepth = newDepth + self._depthChange
                dive = motion.basic.RateChangeDepth(desiredDepth = newDepth,
                                                    speed = self._diveSpeed)
                self.motionManager.setMotion(dive)
                self._finished = False
            elif event.range > self._farRangeThreshold and \
                    vectorLength < self._radius:
                self._recoverMethod = "Far Range"
                # If the range is far and inside radius, move forwards slowly
                desiredDirection = math.Degree(vehicleOrientation)
                recoverMotion = motion.basic.MoveDirection(
                    0, self._advanceSpeed, absolute = False)
                self.motionManager.setMotion(recoverMotion)
                self._finished = True
            else:
                # Otherwise, wait for a symbol before continuing
                self._recoverMethod = "Wait"
                self._finished = True
                self.motionManager.stopCurrentMotion()

    def exit(self):
        self.motionManager.stopCurrentMotion()
        self.controller.setSpeed(0)
        self.controller.setSidewaysSpeed(0)

class CorrectDepth(BuoyTrackingState):

    FINISHED = core.declareEventType('FINISHED')

    @staticmethod
    def transitions():
        trans = BuoyTrackingState.transitions(CorrectDepth, FindAttempt)
        trans.update({ CorrectDepth.FINISHED : Align })

        return trans

    @staticmethod
    def getattr():
        attr = BuoyTrackingState.getattr()
	attr.update(set(['yThreshold', 'depthGain', 'speed', 'boxHeight']))
        return BuoyTrackingState.getattr().union(
            set(['yThreshold', 'depthGain', 'speed', 'boxHeight']))

    def BUOY_FOUND(self, event):
        ret = BuoyTrackingState.BUOY_FOUND(self, event)
        if ret is False:
            return ret

        if abs(event.y) <= self._yThreshold:
            self.publish(CorrectDepth.FINISHED, core.Event())
        else:
            desiredDepth = self.vehicle.getDepth()
            if event.y < 0.0:
                desiredDepth += self._depthGain
                desiredDepth = min(desiredDepth,
                                   self._midDepth + self._boxHeight)
            else:
                desiredDepth -= self._depthGain
                desiredDepth = max(desiredDepth,
                                   self._midDepth - self._boxHeight)

            #m = motion.basic.RateChangeDepth(desiredDepth, self._speed)
            #self.motionManager.setMotion(m)
            self.controller.setDepth(desiredDepth)

    def enter(self):
        BuoyTrackingState.enter(self)

        self._yThreshold = self._config.get('yThreshold', 0.05)
        self._depthGain = self._config.get('depthGain', 0.3)
        self._speed = self._config.get('speed', (1.0/3.0))
        self._boxHeight = self._config.get('boxHeight', 1.5)
        self._midDepth = self.ai.data['config'].get('buoyDepth', 5)

    def exit(self):
        self.motionManager.stopCurrentMotion()
        self.controller.holdCurrentDepth()

class Align(BuoyTrackingState):

    SEEK_BUOY = core.declareEventType('SEEK_BUOY')
    INCORRECT_DEPTH = core.declareEventType('INCORRECT_DEPTH')

    @staticmethod
    def transitions():
        trans = BuoyTrackingState.transitions(Align, FindAttempt)
        trans.update({ motion.seek.SeekPoint.POINT_ALIGNED : Align,
                       Align.INCORRECT_DEPTH : CorrectDepth,
                       Align.SEEK_BUOY : Seek })

        return trans

    @staticmethod
    def getattr():
        return set(['depthGain', 'iDepthGain', 'dDepthGain', 'maxDepthDt',
                    'desiredRange', 'speed', 'alignmentThreshold',
                    'translate', 'translateGain', 'iTranslateGain',
                    'dTranslateGain', 'planeThreshold', 'kp', 'kd',
                    'yawGain', 'maxYaw'])

    def POINT_ALIGNED(self, event):
        """Holds the current depth when we find we are aligned"""
        if self._depthGain != 0:
            self.controller.holdCurrentDepth()

    def _compareChange(self, pvalues, dvalues):
        x, y = pvalues
        dx, dy = dvalues

        if self._depthGain == 0:
            y = 0

        finalx = self._kp * abs(x) + self._kd * abs(dx)
        finaly = self._kp * abs(y) + self._kd * abs(dy)

        return finalx < self._planeThreshold and \
            finaly < self._planeThreshold

    def BUOY_FOUND(self, event):
        """Update the state of the buoy, this moves the vehicle"""
        ret = BuoyTrackingState.BUOY_FOUND(self, event)
        if ret is False:
            return ret

        self._buoy.setState(event.azimuth, event.elevation, event.range,
                            event.x, event.y, event.timeStamp)

        change = self._buoy.changeOverTime()
        if self._compareChange((event.x, event.y),
                               (change[3], change[4])):
            # Only applies if depthGain is set to 0
            if abs(event.y) > self._planeThreshold:
                # Need to recorrect the height
                self.publish(Align.INCORRECT_DEPTH, core.Event())
            else:
                # We are properly aligned
                self.publish(Align.SEEK_BUOY, core.Event())

    def enter(self):
        BuoyTrackingState.enter(self)

        self._kp = self._config.get('kp', 1.0)
        self._kd = self._config.get('kd', 1.0)
        self._buoy = ram.motion.seek.PointTarget(0, 0, 0, 0, 0,
                                                  timeStamp = None,
                                                  vehicle = self.vehicle,
                                                  kp = self._kp, kd = self._kd)
        self._planeThreshold = self._config.get('planeThreshold', 0.03)
        self._depthGain = self._config.get('depthGain', 3)
        iDepthGain = self._config.get('iDepthGain', 0.5)
        dDepthGain = self._config.get('dDepthGain', 0.5)
        maxDepthDt = self._config.get('maxDepthDt', 1)
        desiredRange = self._config.get('desiredRange', 5)
        speed = self._config.get('speed', 3)
        translate = self._config.get('translate', False)
        translateGain = self._config.get('translateGain', 1.0)
        iTranslateGain = self._config.get('iTranslateGain', 0)
        dTranslateGain = self._config.get('dTranslateGain', 0)
        alignmentThreshold = self._config.get('alignmentThreshold', 0.1)
        yawGain = self._config.get('yawGain', 1.0)
        maxYaw = self._config.get('maxYaw', 2.0)
        motion = ram.motion.seek.SeekPointToRange(target = self._buoy,
                                                  alignmentThreshold = alignmentThreshold,
                                                  desiredRange = desiredRange,
                                                  maxRangeDiff = 5,
                                                  maxSpeed = speed,
                                                  depthGain = self._depthGain,
                                                  iDepthGain = iDepthGain,
                                                  dDepthGain = dDepthGain,
                                                  maxDepthDt = maxDepthDt,
                                                  translate = translate,
                                                  translateGain = translateGain,
                                                iTranslateGain = iTranslateGain,
                                                dTranslateGain = dTranslateGain,
                                                  yawGain = yawGain,
                                                  maxYaw = maxYaw)
        self.motionManager.setMotion(motion)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class Seek(BuoyTrackingState):
    @staticmethod
    def transitions():
        trans = BuoyTrackingState.transitions(Seek, FindAttemptSeek)
        trans.update({ vision.EventType.BUOY_ALMOST_HIT : Hit })

        return trans

    @staticmethod
    def getattr():
        return set(['depthGain', 'iDepthGain', 'dDepthGain', 'speed',
                    'planeThreshold'])

    def BUOY_FOUND(self, event):
        """Update the state of the buoy, this moves the vehicle"""
        ret = BuoyTrackingState.BUOY_FOUND(self, event)
        if ret is False:
            return ret

        if event.y > self._planeThreshold:
            self.publish(Align.INCORRECT_DEPTH, core.Event())
        else:
            self._buoy.setState(event.azimuth, event.elevation, event.range,
                                event.x, event.y, event.timeStamp)

    def enter(self):
        BuoyTrackingState.enter(self)

        self._buoy = ram.motion.seek.PointTarget(0, 0, 0, 0, 0,
                                                  timeStamp = None,
                                                  vehicle = self.vehicle)
        self._planeThreshold = self._config.get('planeThreshold', 0.1)
        depthGain = self._config.get('depthGain', 0)
        iDepthGain = self._config.get('iDepthGain', 0)
        dDepthGain = self._config.get('dDepthGain', 0)
        speed = self._config.get('speed', 3)
        motion = ram.motion.seek.SeekPoint(target = self._buoy,
                                                  maxSpeed = speed,
                                                  depthGain = depthGain,
                                                  iDepthGain = iDepthGain,
                                                  dDepthGain = dDepthGain)
        self.motionManager.setMotion(motion)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class Hit(state.State):
    REPOSITION = core.declareEventType('REPOSITION')
    CONTINUE = core.declareEventType('CONTINUE')
    
    @staticmethod
    def transitions():
        return { Hit.REPOSITION : Reposition,
                 Hit.CONTINUE : Continue }

    @staticmethod
    def getattr():
        return set(['duration', 'speed'])

    def enter(self):
        self.visionSystem.buoyDetectorOff()

        # Increment the number of buoys hit
        buoysHit = self.ai.data.get('buoysHit', 0) + 1
        self.ai.data['buoysHit'] = buoysHit

        # Check how many buoys there are
        colorList = self.ai.data['config'].get('targetBuoys', [])

        # Timer goes off in 3 seconds then sends off FORWARD_DONE
        duration = self._config.get('duration', 3)
        speed = self._config.get('speed', 3)
        if buoysHit < len(colorList):
            self.timer = self.timerManager.newTimer(Hit.REPOSITION, duration)
        else:
            self.timer = self.timerManager.newTimer(Hit.CONTINUE, duration)
        self.timer.start()
        self.controller.setSpeed(speed)
    
    def exit(self):
        self.timer.stop()
        self.controller.setSpeed(0)
        self.publish(BUOY_HIT, core.Event())

class Reposition(state.State):
    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Searching }

    @staticmethod
    def getattr():
        return set(['speed', 'primaryDuration', 'secondaryDuration',
                    'diveSpeed', 'headingSpeed'])

    def enter(self):
        self._speed = self._config.get('speed', 3)
        self._primaryDuration = self._config.get('primaryDuration', 2)
        self._secondaryDuration = self._config.get('secondaryDuration', 4)

        primaryMotion = motion.basic.TimedMoveDirection(180, self._speed,
                                                        self._primaryDuration,
                                                        absolute = False)

        self._desiredHeading = self.ai.data['buoyStartOrientation']
        self._headingSpeed = self._config.get('headingSpeed', 20)
        headingMotion = motion.basic.RateChangeHeading(self._desiredHeading,
                                                       self._headingSpeed)

        secondaryMotion = \
            motion.basic.TimedMoveDirection(180, self._speed,
                                            self._secondaryDuration,
                                            absolute = False)

        self._depth = self.ai.data['config'].get('buoyDepth', 5)
        self._diveSpeed = self._config.get('diveSpeed', 1.0/3.0)
        diveMotion = motion.basic.RateChangeDepth(self._depth, self._diveSpeed)

        self.motionManager.setMotion(primaryMotion, headingMotion,
                                     secondaryMotion, diveMotion)

    def exit(self):
        self.motionManager.stopCurrentMotion()
        self.controller.holdCurrentDepth()
        self.controller.holdCurrentPosition()
        
class Continue(state.State):
    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : End }

    @staticmethod
    def getattr():
        return set(['backwardSpeed', 'backwardDuration', 'upwardDepth',
                    'upwardSpeed', 'forwardSpeed', 'forwardDuration',
                    'turnSteps'])
    
    def enter(self):
        # Load config settings
        self._backwardSpeed = self._config.get('backwardSpeed', 3)
        self._backwardDuration = self._config.get('backwardDuration', 4)
        self._upwardDepth = self._config.get('upwardDepth', 2.5)
        self._upwardSpeed = self._config.get('upwardSpeed', 0.3)
        self._forwardSpeed = self._config.get('forwardSpeed', 3)
        self._forwardDuration = self._config.get('forwardDuration', 8)
        self._turnSpeed = self._config.get('turnSpeed', 10)

        # Load the original orientation
        original = self.ai.data.get('buoyStartOrientation', None)
        
        # Create the motions
        if original is not None:
            self._rotate = motion.basic.RateChangeHeading(
                desiredHeading = original, speed = self._turnSpeed)
        self._backward = motion.basic.TimedMoveDirection(desiredHeading = 180,
            speed = self._backwardSpeed, duration = self._backwardDuration,
            absolute = False)
        currentDepth = self.vehicle.getDepth()
        self._upward = motion.basic.RateChangeDepth(
            desiredDepth = currentDepth - self._upwardDepth,
            speed = self._upwardSpeed)
        self._forward = motion.basic.TimedMoveDirection(desiredHeading = 0,
            speed = self._forwardSpeed, duration = self._forwardDuration,
            absolute = False)
        
        # Queue the motions
        if original is None:
            self.motionManager.setMotion(self._backward, self._upward,
                                         self._forward)
        else:
            self.motionManager.setMotion(self._rotate, self._backward,
                                         self._upward, self._forward)

class End(state.State):
    def enter(self):
        self.motionManager.stopCurrentMotion()
        self.publish(COMPLETE, core.Event())
