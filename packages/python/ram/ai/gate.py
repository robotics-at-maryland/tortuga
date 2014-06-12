# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# Edited, 2013: Johnny Mao <jmao@umd.edu> 
# File:  packages/python/ram/ai/gate.py

"""
A state machine to go through the gate:
 - Waits for PipeDetector to turn on
 - Dives to depth
 - Goes forward while looking for a gate
 - 'Approaches' the gate
 - Moves forward through the gate.
 
 
Requires the following subsystems:
 - timerManager - ram.timer.TimerManager
 - motionManager - ram.motion.MotionManager
 - controller - ext.control.IController
"""

# TODO: Add some way for required subsystems to be checked more throughly

# Project Imports
import ext.core as core
import ext.math as math
import math as pmath

from ext.control import yawVehicleHelper

import ram.ai.state as state
import ram.ai.Utility as utility
import ram.ai.Approach as approach
import ram.motion as motion
import ram.motion.search
from  ram.motion.basic import Frame

#imports added as of 2013
import ext.vision as vision
import ram.timer as timer

# Denotes when this state machine finishes
COMPLETE = core.declareEventType('COMPLETE')

# used to loop the lost gate case
WAIT_LOOP = core.declareEventType('WAIT_LOOP')


# global variables
global lastVisionEvent # last vision event before it was lost
global lastLocation # last location before vision was lost
global lastDepth # last depth before vision was lost

global startingDepth # start depth to dive to
global searchDistance # initial distance to travel while searching
global totalDistance # total distance til you go through the gate
global timerDelay 
global startLocation

# boolean for GateApproach, if it's lost the gate for longer than a second
global foundGate
foundGate = True

# Tune these values
startingDepth = 3.5
searchDistance = 13
totalDistance = 13
timerDelay = 1
# --


class Wait(state.State):
    
    @staticmethod
    def transitions():
        return {'Start' : Start}

# Start state --
class Start(utility.MotionState):
    """
    This Start state dives down to a depth at which the gate should be around.
    Translate into a state that moves forward to look for the gate.
    """

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Start,
                 utility.DONE : FindGate }

    @staticmethod
    def getattr():
        global startingDepth
        return { 'rate': 0.3, 'startDepth' : startingDepth }
    
    def enter(self):
        self.dive(self._startDepth, self._rate)
 
    def exit(self):
        self.motionManager.stopCurrentMotion()    
#end of Start state
       
# added as of 2013
# FindGate state --
class FindGate(state.State):

    """
    The robot should be facing in the general direction of the gate at start off.
    Now the robot will move forward bit by bit until it "sees" the gate
    Will transition to approach to draw closer to the gate.
    """
    DONE = core.declareEventType('DONE')

    @staticmethod
    def transitions():
        return { #vision.EventType.GATE_FOUND : GateApproach,
                 motion.basic.MotionManager.FINISHED : End}

    @staticmethod
    def getattr():
        global searchDistance
        return { 'avgRate' : 0.3, 'distance' : searchDistance }

    def enter(self):
        global startLocation
        startLocation = self.stateEstimator.getEstimatedPosition()
        forwardTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = math.Vector2.ZERO,
            finalValue = math.Vector2(self._distance, 0),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._avgRate)
        forwardMotion = motion.basic.Translate(
            trajectory = forwardTrajectory,
            frame = Frame.LOCAL)
        self.motionManager.setMotion(forwardMotion)

    def exit(self):
        utility.freeze(self)
        self.motionManager.stopCurrentMotion()


# end of FindGate state



# start of GateApproach
class GateApproach(approach.SuperApproach):

    QUIT = core.declareEventType('QUIT')
    
    @staticmethod
    def transitions():
            return { approach.DONE : Forward,
                     vision.EventType.GATE_FOUND : GateApproach,
                     vision.EventType.GATE_LOST : GateApproach,
                     GateApproach.QUIT : Forward }

    @staticmethod
    def getattr():
        return { 'dx' : .25,'dy' : -.1, 'dz' : 0,
                 'xmin' : -0.05 , 'xmax' : 0.05,
                 'zmin' : 0.00, 'zmax': 0.5,
                 'rmin' : 400, 'rmax' : 450,
                 'xDisp' : 0, 'yDisp' : 0, 'zDisp' : 0 }

    def enter(self):
        global timerDelay
        self.timer = self.timerManager.newTimer(GateApproach.QUIT, timerDelay )
        global foundGate
        if foundGate:
            pass
        else:
            self.publish(GateApproach.QUIT, core.Event())

    def decideZ(self, event):
        return 0

    def GATE_FOUND(self, event):
        global lastVisionEvent
        global lastLocation # not used
        global lastDepth # not used
        global quitGate # not used
        self.timer.stop()
        foundGate = True
        lastVisionEvent = event
        lastLocation = self.stateEstimator.getEstimatedPosition()
        lastDepth = self.stateEstimator.getEstimatedDepth()
        #print '--------'
        #print 'Event x: ' + str(event.x)
        #print 'Event y: ' + str(event.y)
        #print 'Range: ' + str(event.range)
        #print self.decideZ(event)
        self.run(event)
    
    def GATE_LOST(self, event):
        
        if (self.decideX(event) == 0 and self.decideY(event) <= 0 and self.decideZ(event) <= 0) or \
                (self.decideX(event) != 0 and self.decideY(event) <= 0):
            self.publish(GateApproach.QUIT, core.Event())
        else:
            self.timer.start()

    def end_cond(self, event):
        return ( (self.decideY(event) == 0) and \
                 (self.decideX(event) == 0) and \
                 (self.decideZ(event) == 0) )

    def exit(self):
        utility.freeze(self)
        self.motionManager.stopCurrentMotion()

# end of GateApproach state
    

# added as of 2013
# Forward state --
class Forward(utility.MotionState):
    """
    A state in which the vehicle will move forward a specified amount
    """

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Forward,
                 utility.DONE : End}

    @staticmethod
    def getattr():
        
        return { 'avgRate' : 0.3 }


    def enter(self):
        global startLocation
        global totalDistance
        currLocation = self.stateEstimator.getEstimatedPosition()
        newDist = totalDistance - pmath.sqrt((currLocation.x-startLocation.x)**2 + (currLocation.y - startLocation.y)**2)
        print 'Dist: ' + str(newDist)
        self.translate(0, newDist, self._avgRate)


    def exit(self):
        self.motionManager.stopCurrentMotion()
# end of Forward state


# End state --
class End(state.State):
    def enter(self):
        self.publish(COMPLETE, core.Event())
# end
