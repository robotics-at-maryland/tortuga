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
from ext.control import yawVehicleHelper

import ram.filter as filter
import ram.ai.state as state
import ram.ai.tracking as tracking
import ram.motion as motion
import ram.motion.search
import ram.motion.seek
import ram.timer
from ram.motion.basic import Frame

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

    @staticmethod
    def getattr():
        return set([])

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

    def enter(self, detector = True):
        if detector:
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

class Start(BuoyTrackingState):
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
        return { 'speed' : 0.3, 'diveRate' : 0.3 }
    
    def enter(self):
        BuoyTrackingState.enter(self, detector = False)

        # Store the initial orientation
        orientation = self.stateEstimator.getEstimatedOrientation()
        self.ai.data['buoyStartOrientation'] = \
            orientation.getYaw().valueDegrees()

        # Go to 5 feet in 5 increments
        buoyDepths = self.ai.data['config'].get('buoyDepth', {})

        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = buoyDepths.get(str(self._desiredColor).lower(), 5),
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._diveRate)
        currentOrientation = self.stateEstimator.getEstimatedOrientation()
        yawTrajectory = motion.trajectories.StepTrajectory(
            initialValue = currentOrientation,
            finalValue = math.Quaternion(
                math.Degree(self.ai.data['buoyStartOrientation']),
                            math.Vector3.UNIT_Z),
            initialRate = self.stateEstimator.getEstimatedAngularRate(),
            finalRate = math.Vector3.ZERO)

        diveMotion = motion.basic.ChangeDepth(
            trajectory = diveTrajectory)
        yawMotion = motion.basic.ChangeOrientation(yawTrajectory)

        self.motionManager.setMotion(yawMotion, diveMotion)

        self.ai.data['firstSearching'] = True
        
    def exit(self):
        #self.motionManager.stopCurrentMotion()
        pass

class Searching(BuoyTrackingState):

    BUOY_SEARCHING = core.declareEventType('BUOY_SEARCHING')

    @staticmethod
    def transitions():
        return BuoyTrackingState.transitions(Align, Searching)

    @staticmethod
    def getattr():
        return set(['legTime', 'sweepAngle', 'speed'])

    def enter(self):
        self.publish(Searching.BUOY_SEARCHING, core.Event())

        BuoyTrackingState.enter(self)

        # Turn on the vision system
        self.visionSystem.buoyDetectorOn()
        
        # Set the start orientation if it isn't already set
        orientation = self.stateEstimator.getEstimatedOrientation()
        direction = self.ai.data.setdefault('buoyStartOrientation',
                                orientation.getYaw().valueDegrees())

        # Create zig zag search to 
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = math.Vector2.ZERO,
            finalValue = math.Vector2(5,0),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = 0.3)
        
        translateMotion = ram.motion.basic.Translate(translateTrajectory,
                                                     frame = Frame.LOCAL)
        
        self.motionManager.setMotion(translateMotion)

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
                newDepth = self.stateEstimator.getEstimatedDepth()
                changeDepth = False
                
                if event.y > self._yThreshold:
                    newDepth = newDepth - self._closeDepthChange
                    changeDepth = True
                elif event.y < (0.0 - self._yThreshold):
                    newDepth = newDepth + self._closeDepthChange
                    changeDepth = True
                
                # Start the depth motion if necessary
                if changeDepth:
                    diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
                        initialValue = self.stateEstimator.getEstimatedDepth(),
                        finalValue = newDepth,
                        initialRate = self.stateEstimator.getEstimatedDepthRate(),
                        avgRate = self._diveSpeed)
                    self._diveMotion = motion.basic.ChangeDepth(
                        trajectory = diveTrajectory)
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
        self._reverseSpeed = self._config.get('reverseSpeed', .3)
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
                self.stateEstimator.getEstimatedOrientation().getYaw().valueDegrees()

            if event.range < self._closeRangeThreshold:
                # If the range is very close, backup and change depth
                # Find the backwards direction and create the motion

                translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
                    initialValue = math.Vector2.ZERO,
                    finalValue = math.Vector2(-5,0),
                    initialRate = self.stateEstimator.getEstimatedVelocity(),
                    avgRate = self._reverseSpeed)

                self._recoverMethod = "Close Range"
                self._recoverMotion = \
                    ram.motion.basic.Translate(translateTrajectory,
                                               frame = Frame.LOCAL)
            
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
                self.controller.yawVehicle(yawAngle, 0)
            
                # Change the depth if it's outside on the y-axis
                newDepth = self.stateEstimator.getEstimatedDepth()
                if event.y > self._radius:
                    newDepth = newDepth - self._depthChange
                elif event.y < (0.0 - self._radius):
                    newDepth = newDepth + self._depthChange
                    
                diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
                    initialValue = self.stateEstimator.getEstimatedDepth(),
                    finalValue = newDepth,
                    initialRate = self.stateEstimator.getEstimatedDepthRate(),
                    avgRate = self._diveSpeed)
                dive = motion.basic.ChangeDepth(
                    trajectory = diveTrajectory)
                self.motionManager.setMotion(dive)
                self._finished = False
            elif event.range > self._farRangeThreshold and \
                    vectorLength < self._radius:
                self._recoverMethod = "Far Range"
                # If the range is far and inside radius, move forwards slowly
                desiredDirection = math.Degree(vehicleOrientation)
                
                translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
                    initialValue = math.Vector2.ZERO,
                    finalValue = math.Vector2(5,0),
                    initialRate = self.stateEstimator.getEstimatedVelocity(),
                    avgRate = 0.2)
                
                recoverMotion = ram.motion.basic.Translate(translateTrajectory,
                                                             frame = Frame.LOCAL)
                self.motionManager.setMotion(recoverMotion)
                self._finished = True
            else:
                # Otherwise, wait for a symbol before continuing
                self._recoverMethod = "Wait"
                self._finished = True
                self.motionManager.stopCurrentMotion()

    def exit(self):
        self.motionManager.stopCurrentMotion()

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
            desiredDepth = self.stateEstimator.getEstimatedDepth()
            if event.y < 0.0:
                desiredDepth += self._depthGain
                desiredDepth = min(desiredDepth,
                                   self._midDepth + self._boxHeight)
            else:
                desiredDepth -= self._depthGain
                desiredDepth = max(desiredDepth,
                                   self._midDepth - self._boxHeight)

            diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
                initialValue = self.stateEstimator.getEstimatedDepth(),
                finalValue = desiredDepth,
                initialRate = self.stateEstimator.getEstimatedDepthRate(),
                avgRate = 0.3)
            dive = motion.basic.ChangeDepth(
                trajectory = diveTrajectory)
            self.motionManager.setMotion(dive)

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
        self._buoy = \
            ram.motion.seek.PointTarget(0, 0, 0, 0, 0,
                                        timeStamp = None,
                                        estimator = self.stateEstimator,
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

        self._buoy = \
            ram.motion.seek.PointTarget(0, 0, 0, 0, 0,
                                        timeStamp = None,
                                        estimator = self.stateEstimator)
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

class Reposition(BuoyTrackingState):
    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Searching }

    @staticmethod
    def getattr():
        return set(['speed', 'primaryDuration', 'secondaryDuration',
                    'diveSpeed', 'headingSpeed'])

    def enter(self):
        BuoyTrackingState.enter(self, detector = False)

        self._speed = self._config.get('speed', 3)
        self._primaryDuration = self._config.get('primaryDuration', 2)
        self._secondaryDuration = self._config.get('secondaryDuration', 4)

        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = math.Vector2.ZERO,
            finalValue = math.Vector2(-5,0),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._forwardSpeed)
        
        primaryMotion = ram.motion.basic.Translate(translateTrajectory,
                                                     frame = Frame.LOCAL)

        self._desiredHeading = self.ai.data['buoyStartOrientation']
        self._headingSpeed = self._config.get('headingSpeed', 20)
        
        currentOrientation = self.stateEstimator.getEstimatedOrientation()
        yawTrajectory = motion.trajectories.StepTrajectory(
            initialValue = currentOrientation,
            finalValue = math.Quaternion(
                math.Degree(self._desiredHeading),
                math.Vector3.UNIT_Z),
            initialRate = self.stateEstimator.getEstimatedAngularRate(),
            finalRate = math.Vector3.ZERO)

        headingMotion = motion.basic.ChangeOrientation(yawTrajectory)

        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = math.Vector2.ZERO,
            finalValue = math.Vector2(-5,0),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._forwardSpeed)
        
        secondaryMotion = ram.motion.basic.Translate(translateTrajectory,
                                                     frame = Frame.LOCAL)

        buoyDepths = self.ai.data['config'].get('buoyDepth', {})
        self._depth = buoyDepths.get(str(self._desiredColor).lower(), 5)
        self._diveSpeed = self._config.get('diveSpeed', 1.0/3.0)
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = self._depth,
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._diveSpeed)
        diveMotion = motion.basic.ChangeDepth(
            trajectory = diveTrajectory)

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
        self._forwardSpeed = self._config.get('forwardSpeed', 0.3)
        self._forwardDuration = self._config.get('forwardDuration', 8)
        self._turnSpeed = self._config.get('turnSpeed', 10)

        # Load the original orientation
        original = self.ai.data.get('buoyStartOrientation', None)
        
        # Create the motions
        if original is not None:
            currentOrientation = self.stateEstimator.getEstimatedOrientation()
            yawTrajectory = motion.trajectories.StepTrajectory(
                initialValue = currentOrientation,
                finalValue = math.Quaternion(
                    math.Degree(original),
                    math.Vector3.UNIT_Z),
                initialRate = self.stateEstimator.getEstimatedAngularRate(),
                finalRate = math.Vector3.ZERO)
            
            self._rotate = motion.basic.ChangeOrientation(yawTrajectory)

        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = math.Vector2.ZERO,
            finalValue = math.Vector2(-2,0),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._forwardSpeed)
        
        self._backward = ram.motion.basic.Translate(translateTrajectory,
                                                     frame = Frame.LOCAL)

        currentDepth = self.stateEstimator.getEstimatedDepth()
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = currentDepth,
            finalValue = currentDepth - self._upwardDepth,
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = 0.3)
        self._upward = motion.basicChangeDepth(
            trajectory = diveTrajectory)

        forwardTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = math.Vector2.ZERO,
            finalValue = math.Vector2(7,0),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._forwardSpeed)
        
        self._forward = ram.motion.basic.Translate(translateTrajectory,
                                                     frame = Frame.LOCAL)
        
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
