# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   packages/python/ram/motion/pipe.py

# STD Imports
import math

# Project Imports
import ram.timer
import ram.core as core
from ram.motion.basic import Motion
import ext.core

# basic SISO PID regulator control loop
#
# NOTE: this is a setpoint regulator designed to converge to a single setpoint
#
# this is ***NOT*** a tracking controller, so it is not designed to 
#follow trajectories
#
# intended to be a generic PID regulator control loop
# note that the user is expected to maintain copies of the controller gains
# as well as maintain copies of sum and xOld
#
# usage: if you have a measurable state x that you desire to go to the constant
#         value xd, then call this function for as long as you wish x to stay
#         at xd (even after x reaches xd).  
#
# input:
#    x - measured state
#    xd - desired state
#    dt - change in time from previous PIDLoop function call
#    dtTooSmall - smallest realistic value you expect dt could be
#    dtTooBig - largest realistic value you expect dt could be
#    kp - proportional gain
#    kd - derivative gain
#    ki - integral gain
#    sum - integral of (x-xd) w.r.t. time
#    xOld - previous x measurement 
#
# output:
#    u - control signal
#    xOld - new value for xOld
#    sum - new value of integral value
#
def PIDLoop(x,xd,dt,dtTooSmall,dtTooBig,kp,kd,ki,sum,xOld):
    #make sure that everything is float
    x = float(x)
    xd = float(xd)
    dt = float(dt)
    dtTooSmall = float(dtTooSmall)
    dtTooBig = float(dtTooBig)
    #don't need to worry about gains since int*float=float
    sum = float(sum)
    xOld = float(xOld)

    if dt < dtTooSmall:
        dt = dtTooSmall
    elif dt > dtTooBig:
        dt = dtTooBig

    #P term
    error = x - xd
    #I term
    sum = sum + error*dt
    #D term
    xDot = (x-xOld)/dt
    #save old state
    xOld=x
    #compute control
    u=-kp*error-kd*xDot-ki*sum

    return (u,sum,xOld)


class Target(ext.core.EventPublisher):
    """
    Represents a target with an X, Y position below the vehicle
    """
    UPDATE = ext.core.declareEventType('UPDATE')
    
    def __init__(self, x, y):
        ext.core.EventPublisher.__init__(self)
        Target.setState(self, x, y, publish = False)

    def setState(self, x, y, publish = True):
        self.x = x
        self.y = y

        if publish:
            self.publish(Target.UPDATE, ext.core.Event())
    
class Hover(Motion):
    """
    Base motion it centers the vehicle over the target
    """
    def __init__(self, target, maxSpeed = 0.0, maxSidewaysSpeed = 0.0,
                 speedGain = 1.0, sidewaysSpeedGain = 1.0,
                 _type = Motion.IN_PLANE,
                 iSpeedGain = 0.0, dSpeedGain = 0.0, iSidewaysSpeedGain = 0.0,
                 dSidewaysSpeedGain = 0.0):
        """
        @type  target: ram.motion.common.Target
        @param target: Target to attempt to reach
        """
        Motion.__init__(self, _type = _type)
        
        self._xDesired = 0
        self._yDesired = 0

        self._running = False
        self._target = target

        self._maxSpeed = maxSpeed
        self._minSpeed = -1 * maxSpeed
        self._sumSpeed = 0.0
        self._oldSpeed = 0.0
        self._iSpeedGain = iSpeedGain
        self._dSpeedGain = dSpeedGain
        self._lastSpeedRunTime = 0.0

        self._maxSidewaysSpeed = maxSidewaysSpeed
        self._minSidewaysSpeed = -1 * maxSidewaysSpeed
        self._sumSidewaysSpeed = 0.0
        self._oldSidewaysSpeed = 0.0
        self._iSidewaysSpeedGain = iSidewaysSpeedGain
        self._dSidewaysSpeedGain = dSidewaysSpeedGain
        self._lastSidewaysSpeedRunTime = 0.0

        self._speedGain = speedGain
        self._sidewaysSpeedGain = sidewaysSpeedGain
        
        self._conn = target.subscribe(Target.UPDATE, self._onTargetUpdate)
        
    def _start(self):
        self._running = True
        self._seek()
        
    def _setForwardSpeed(self):
        """Determin forward speed (and bound within limits)"""
        
        # Compute time since the last run
        deltaT = 1.0/40.0
        now = ram.timer.time()
        if self._lastSpeedRunTime != 0.0:
            deltaT = now - self._lastSpeedRunTime
        self._lastSpeedRunTime = now
        
        forwardSpeed, sum, old = PIDLoop(
            x = self._target.y,
            xd = self._xDesired, 
            dt = deltaT,
            dtTooSmall = 1.0/100.0, 
            dtTooBig = 1.0, 
            kp = self._speedGain, 
            kd = self._dSpeedGain, 
            ki = self._iSpeedGain,
            sum = self._sumSpeed, 
            xOld = self._oldSpeed)
        self._sumSpeed = sum
        self._oldSpeed = old
        
        # Flip speed
        forwardSpeed *= -1
        
        # Clamp speed ranges
        if forwardSpeed > self._maxSpeed:
            forwardSpeed = self._maxSpeed
        elif forwardSpeed < self._minSpeed:
            forwardSpeed = self._minSpeed

        self._forwardSpeed = forwardSpeed
        self._controller.setSpeed(forwardSpeed)
        
    def _setSidewaysSpeed(self):
        """Determine sideways speed (and bound within limits)"""
        
        # Compute time since the last run
        deltaT = 0.0
        now = ram.timer.time()
        if self._lastSidewaysSpeedRunTime != 0.0:
            deltaT = now - self._lastSidewaysSpeedRunTime
        self._lastSidewaysSpeedRunTime = now
        
        sidewaysSpeed, sum, old = PIDLoop(
            x = self._target.x,
            xd = self._yDesired,
            dt = deltaT,
            dtTooSmall = 1.0/100.0, 
            dtTooBig = 1.0, 
            kp = self._sidewaysSpeedGain, 
            kd = self._dSidewaysSpeedGain, 
            ki = self._iSidewaysSpeedGain,
            sum = self._sumSidewaysSpeed, 
            xOld = self._oldSidewaysSpeed)
        self._sumSidewaysSpeed = sum
        self._oldSidewaysSpeed = old
        
        # Flip sign
        sidewaysSpeed *= -1

        # Clamp speed ranges
        if sidewaysSpeed > self._maxSidewaysSpeed:
            sidewaysSpeed = self._maxSidewaysSpeed
        elif sidewaysSpeed < self._minSidewaysSpeed:
            sidewaysSpeed = self._minSidewaysSpeed

        self._sidewaysSpeed = sidewaysSpeed
        self._controller.setSidewaysSpeed(sidewaysSpeed)

    def _seek(self):
        self._setForwardSpeed()
        self._setSidewaysSpeed()

    @staticmethod
    def _limit(val, min, max):
        if val < min:
            return min
        elif val > max:
            return max
        return val

    def _onTargetUpdate(self, event):
        if self._running:
            # Data already updated, so lets just keep seeking
            self._seek()
            
    def stop(self):
        """
        Finishes off the motion, disconnects events, and putlishes finish event
        """
        self._running = False
        self._controller.setSpeed(0)
        self._controller.setSidewaysSpeed(0)
        if self._conn is not None:
            self._conn.disconnect()
            self._conn = None
        
