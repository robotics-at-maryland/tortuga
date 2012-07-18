# Copyright (C) 2012 Maryland Robotics Club
# Copyright (C) 2012 Stephen Christian <schrist2@umd.edu>
# All rights reserved.
#
# Author: Stephen Christian <schrist2@terpmail.umd.edu>
# File:  packages/python/ram/ai/buoy.py

# Standard imports
import math as pmath
import string as pstring

# Project Imports
import ext.core as core
import ext.vision as vision
import ext.math as math
import ext.estimation as estimation
from ext.control import yawVehicleHelper

import ram.ai.state as state
import ram.motion as motion
import ram.timer
from ram.motion.basic import Frame

COMPLETE = core.declareEventType('COMPLETE')

class Start(state.State):

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Search }

    @staticmethod
    def getattr():
        return { 'diveRate' : 0.3 , 'speed' : 0.3 }

    def enter(self):
        self.visionSystem.buoyDetectorOn()

        buoyDepth = self.ai.data['config'].get('buoyDepth', -1)

        self._orientation = self.ai.data['buoyOrientation']
        self.ai.data['buoyData'] = {}
        self.ai.data['ignoreBuoy'] = {}
        for color in self.ai.data['buoyList']:
            self.ai.data['buoyData'][color.lower()] = []
            self.ai.data[color.lower() + 'FoundNum'] = 0
            self.ai.data['ignoreBuoy'][color.lower()] = False

        # Compute trajectories
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = buoyDepth,
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._diveRate)
        
        currentOrientation = self.stateEstimator.getEstimatedOrientation()
        yawTrajectory = motion.trajectories.StepTrajectory(
            initialValue = currentOrientation,
            finalValue = math.Quaternion(
                math.Degree(self._orientation), math.Vector3.UNIT_Z),
            initialRate = self.stateEstimator.getEstimatedAngularRate(),
            finalRate = math.Vector3.ZERO)

        # Dive yaw and translate
        diveMotion = motion.basic.ChangeDepth(trajectory = diveTrajectory)
        yawMotion = motion.basic.ChangeOrientation(yawTrajectory)
        
        self.motionManager.setMotion(diveMotion, yawMotion)

    def exit(self):
        self.motionManager.stopCurrentMotion()


class Search(state.ZigZag):

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Search ,
                 vision.EventType.BUOY_FOUND : Strafe , 
                 state.ZigZag.DONE : End }


class Strafe(state.State):

    NONE_LEFT = core.declareEventType('NONE_LEFT')
    DONE = core.declareEventType('DONE')
    FOUND_BUOY = core.declareEventType('FOUND_BUOY ')

    STEPNUM = 0

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Strafe ,
                 vision.EventType.BUOY_FOUND : Strafe ,
                 Strafe.FOUND_BUOY : Center ,
                 Strafe.DONE : Align,
                 Strafe.NONE_LEFT : End
                 }

    @staticmethod
    def getattr():
        return { 'distanceLeft' : 2 , 'distanceRight' : 2,  'speed' : 0.15,
                 'foundMin' : 10 }

    def enter(self):
        
        self.ignoreCount = 0

        for color in self.ai.data['ignoreBuoy']:
            if self.ai.data['ignoreBuoy'][color] == True:
                self.ignoreCount += 1

        if self.ignoreCount == len(self.ai.data['ignoreBuoy']):
            self.publish(Strafe.NONE_LEFT, core.Event())

        self.STEPNUM = 0
        self.nextStep()

    def exit(self):
        self.motionManager.stopCurrentMotion()

    def FINISHED(self, event):
        self.nextStep()

    def BUOY_FOUND(self, event):
        color = str(event.color).lower()
        if(color in self.ai.data['buoyData']):
            self.ai.data['buoyData'][color].append(
                self.stateEstimator.getEstimatedPosition())
            self.ai.data[color + 'FoundNum'] += 1
            if(self.ai.data[color + 'FoundNum'] >= self._foundMin and
               not self.ai.data['ignoreBuoy'][color]):
                self.ai.data['buoyColor'] = color
                self.publish(Strafe.FOUND_BUOY, core.Event())

    def move(self, distance):
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = math.Vector2.ZERO,
            finalValue = math.Vector2(0, distance),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._speed)
        translateMotion = motion.basic.Translate(
            trajectory = translateTrajectory,
            frame = Frame.LOCAL)

        self.motionManager.setMotion(translateMotion)

    def nextStep(self):
        #print("Step #: " + str(self.STEPNUM))
        if ( self.STEPNUM == 0):
            self.move(self._distanceRight)

        elif ( self.STEPNUM == 1 ):
            self.move(-self._distanceRight - self._distanceLeft)
        elif ( self.STEPNUM == 2 ):
            self.move(self._distanceLeft)
        else: 
            self.publish(Strafe.DONE, core.Event())

        self.STEPNUM += 1
        
        
class Align(state.State):

    NONE_LEFT = core.declareEventType('NONE_LEFT')
    ALIGNED = core.declareEventType('ALIGNED')

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Align ,
                 Align.NONE_LEFT : End , 
                 Align.ALIGNED : Center} 

    @staticmethod
    def getattr():
        return { 'speed' : 0.15 }

    def enter(self):
        if( len(self.ai.data['buoyList']) == 0 ):
            #print('All buoys hit')
            self.publish(Align.NONE_LEFT, core.Event())
            return
        
        color = self.ai.data['buoyList'].pop(0).lower()
        ignoreColor = self.ai.data['ignoreBuoy'][color]
        while ignoreColor:
            if( len(self.ai.data['buoyList']) == 0 ):
                #print('All buoys hit')
                self.publish(Align.NONE_LEFT, core.Event())
                return
            color = self.ai.data['buoyList'].pop(0).lower()
            ignoreColor = self.ai.data['ignoreBuoy'][color]

        #print('Aligning with ' + color + ' buoy')
        self.ai.data['buoyColor'] = color;
        count = 0
        x_total = 0
        y_total = 0
        for pos in self.ai.data['buoyData'][color]:
            count += 1
            x_total += pos.x
            y_total += pos.y
            
        x_avg = x_total / count;
        y_avg = y_total / count;
            
        #print(str(count) + ' entries in data')
        #print('X Average: ' + str(x_avg))
        #print('Y Average: ' + str(y_avg))

        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedPosition(),
            finalValue = math.Vector2(x_avg, y_avg),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._speed)
        translateMotion = motion.basic.Translate(
            trajectory = translateTrajectory,
            frame = Frame.GLOBAL)
        
        self.motionManager.setMotion(translateMotion)


    def FINISHED(self, event):
        self.publish(Align.ALIGNED, core.Event())


class Center(state.State):
    
    CENTERED = core.declareEventType('CENTERED')
    TIMEOUT = core.declareEventType('TIMEOUT')

    STEPNUM = 0

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Center,
                 vision.EventType.BUOY_FOUND : Center ,
                 Center.CENTERED : Attack , 
                 Center.TIMEOUT : Hit }

    @staticmethod
    def getattr():
        return { 'speed' : 0.15 , 'diveRate' : 0.15 , 'distance' : 2 ,
                 'xmin' : -0.05 , 'xmax' : 0.05 , 
                 'ymin' : -0.05 , 'ymax' : 0.05,
                 'timeout' : 20}

    def move(self, distance):
        if not self.move_again:
            return

        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = math.Vector2.ZERO,
            finalValue = math.Vector2(0, distance),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._speed)
        translateMotion = motion.basic.Translate(
            trajectory = translateTrajectory,
            frame = Frame.LOCAL)

        self.motionManager.setMotion(translateMotion)

        self.move_again = False

    def dive(self, distance):
        if not self.move_again:
            return

        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = self.stateEstimator.getEstimatedDepth() + distance,
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._diveRate)
        diveMotion = motion.basic.ChangeDepth(trajectory = diveTrajectory)

        self.motionManager.setMotion(diveMotion)

        self.move_again = False

    def enter(self):
        self.STEPNUM = 0

        self.move_again = True

        self.timer = self.timerManager.newTimer(Center.TIMEOUT, self._timeout)
        self.timer.start()

    def exit(self):
        if self.timer is not None:
            self.timer.stop()
        self.motionManager.stopCurrentMotion()


    def FINISHED(self, event):
        self.move_again = True

    def BUOY_FOUND(self, event):
        if(str(event.color).lower() != self.ai.data['buoyColor']):
            return

        if(self.STEPNUM == 0):
            #print("Buoy X: " + str(event.x))
            if(event.x <= self._xmin):
                #print("Moving left to compensate")
                self.move(-self._distance)
            elif(event.x >= self._xmax):
                #print("Moving right to compensate")
                self.move(self._distance)

            self.STEPNUM += 1
        
        elif(self.STEPNUM == 1):
            if(event.x > self._xmin and event.x < self._xmax):
                #print("X Axis Aligned")
                self.motionManager.stopCurrentMotion()
                self.move_again = True
                self.STEPNUM += 1
            else:
                self.STEPNUM -= 1

        elif(self.STEPNUM == 2):
            #print("Buoy Y: " + str(event.y))
            if(event.y <= self._ymin):
                #print("Moving down to compensate")
                self.dive(self._distance)
            elif(event.y >= self._ymax):
                #print("Moving up to compensate")
                self.dive(-self._distance)

            self.STEPNUM += 1

        elif(self.STEPNUM == 3):
            if(event.y > self._ymin and event.y < self._ymax):
                #print("Y Axis Aligned, All Done")
                self.motionManager.stopCurrentMotion()
                self.publish(Center.CENTERED, core.Event())
            else:
                self.STEPNUM -= 1

class Attack(state.State):

    DONE = core.declareEventType('DONE')

    @staticmethod
    def transitions():
        return { Attack.DONE : Hit ,
                 vision.EventType.BUOY_FOUND : Attack ,
                 motion.basic.MotionManager.FINISHED : Attack }
        
    @staticmethod
    def getattr():
        return { 'speed' : 0.3 , 'distance' : 0.5 , 
                 'correct_factor' : 2 , 'range_thresh' : 1.5,
                 'timeout' : 10}

    def enter(self):
        self.X = 0
        self.Y = 0
        self.RANGE = -1
        self.ai.data['ignoreBuoy'][self.ai.data['buoyColor']] = True
        self.timer = self.timerManager.newTimer(Attack.DONE, self._timeout)
        self.timer.start()
    
    def exit(self):
        if self.timer is not None:
            self.timer.stop()

    def update(self):
        
        if(self.RANGE < self._range_thresh):
            self.publish(Attack.DONE, core.Event())

        else:
            translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
                initialValue = math.Vector2.ZERO,
                finalValue = math.Vector2(self._distance, 
                                          self._correct_factor * self.X),
                initialRate = self.stateEstimator.getEstimatedVelocity(),
                avgRate = self._speed)
            translateMotion = motion.basic.Translate(
                trajectory = translateTrajectory,
                frame = Frame.LOCAL)
            
            self.motionManager.setMotion(translateMotion)
        

    def BUOY_FOUND(self, event):
        if(str(event.color).lower() != self.ai.data['buoyColor']):
            return

        temp = self.RANGE

        self.X = event.x
        self.Y = event.y
        self.RANGE = event.range

        if(temp == -1):
            self.update()
        
    def FINISHED(self, event):
        self.update()

class Hit(state.State):

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Retreat }

    @staticmethod
    def getattr():
        return { 'distance' : 1.5 , 'speed' : 0.15 }

    def enter(self):
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = math.Vector2.ZERO,
            finalValue = math.Vector2(self._distance, 0),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._speed)
        translateMotion = motion.basic.Translate(
            trajectory = translateTrajectory,
            frame = Frame.LOCAL)

        self.motionManager.setMotion(translateMotion)

class Retreat(state.State):

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Strafe }

    @staticmethod
    def getattr():
        return { 'distance' : 2.5 , 'speed' : 0.15 }

    def enter(self):
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = math.Vector2.ZERO,
            finalValue = math.Vector2(-self._distance, 0),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._speed)
        translateMotion = motion.basic.Translate(
            trajectory = translateTrajectory,
            frame = Frame.LOCAL)

        self.motionManager.setMotion(translateMotion)

class End(state.State):
    def enter(self):
        self.publish(COMPLETE, core.Event())
