# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/ai/gate.py

"""
A state machine to go through the gate:
 - Waits for Switch
 - Dives to depth
 - Goes forward for a time
 
 
Requires the following subsystems:
 - timerManager - ram.timer.TimerManager
 - motionManager - ram.motion.MotionManager
 - controller - ext.control.IController
"""

# TODO: Add some way for required subsystems to be checked more throughly

# Project Imports
import ext.core as core
import ext.math as math
from ext.control import yawVehicleHelper

import ram.ai.state as state
import ram.motion as motion
import ram.motion.search
from  ram.motion.basic import Frame

# Denotes when this state machine finishes
COMPLETE = core.declareEventType('COMPLETE')

class Wait(state.State):
    @staticmethod
    def transitions():
        # TODO: Replace with a start event from the vehicle or sensor board
        return {"Start" : Start}
    
class Start(state.State):
    """
    Gets the vehicle to the proper depth, so the forward charge through the 
    gate can happen.
    """
    
    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Forward }

    @staticmethod
    def getattr():
        return { 'rate': 0.3 }
    
    def enter(self):
        # Go to 5 feet down
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = self.ai.data['config'].get('gateDepth', 5),
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._rate)
        diveMotion = motion.basic.ChangeDepth(
            trajectory = diveTrajectory)
        self.motionManager.setMotion(diveMotion)
        
    def exit(self):
        self.motionManager.stopCurrentMotion()
    
class Forward(state.State):
    """
    A simple timed forward run through the gate.
    """
    
    DONE = core.declareEventType('DONE')

    @staticmethod
    def transitions():
        return {motion.basic.MotionManager.FINISHED : End}

    @staticmethod
    def getattr():
        return { 'distance' : 5 , 'avgRate' : 0.15}

    def enter(self):
        if(self.ai.data['fakeGate']):
            self._distance = self.ai.data['fakeGateDistance']

        currentOrientation = self.stateEstimator.getEstimatedOrientation()
        yawTrajectory = motion.trajectories.StepTrajectory(
            initialValue = currentOrientation,
            finalValue = math.Quaternion(
                math.Degree(self.ai.data['gateOrientation']), math.Vector3.UNIT_Z),
            initialRate = self.stateEstimator.getEstimatedAngularRate(),
            finalRate = math.Vector3.ZERO)
        forwardTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = math.Vector2.ZERO,
            finalValue = math.Vector2(self._distance,0),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._avgRate)

        forwardMotion = motion.basic.Translate(
            trajectory = forwardTrajectory,
            frame = Frame.LOCAL)

        yawMotion = motion.basic.ChangeOrientation(yawTrajectory)

        # Full speed ahead!!
        self.motionManager.setMotion(yawMotion, forwardMotion)
        #self.motionManager.setMotion(forwardMotion)
    
    def exit(self):
        self.motionManager.stopCurrentMotion()
        
class End(state.State):
    def enter(self):
        self.publish(COMPLETE, core.Event())
