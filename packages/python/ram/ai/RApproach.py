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
import ram.ai.Utility as util
import ram.ai.Approach as vs

#generic align state
class Align(state.State):
    @staticmethod
    def getattr():
        return {'yaw_bound' : .1}
    def run(self,angle):
        if((abs(angle)-90) < self._yaw_bound):
            self.publish(DONE,core.Event())
        else:
            currentOrientation = self.stateEstimator.getEstimatedOrientation()
            yawTrajectory = motion.trajectories.StepTrajectory(
            initialValue = currentOrientation,
            finalValue = yawVehicleHelper(currentOrientation, 
                                          -angle),
            initialRate = self.stateEstimator.getEstimatedAngularRate(),
            finalRate = math.Vector3.ZERO)
            yawMotion = motion.basic.ChangeOrientation(yawTrajectory)
            self.motionManager.setMotion(yawMotion) 
            
    @staticmethod
    def transitions():
        return {DONE: state.State, vision.EventType.PIPE_FOUND : Align}

#upright align state
class UAlign(Align):
    def GATE_FOUND(self,event):
        self.run(event.elevation)
    @staticmethod
    def transitions():
        return {DONE: state.State, vision.EventType.GATE_FOUND : UAlign}

#lower buoy aligning state
class DBuoyAlign(Align):
    def BUOY_FOUND(self,event):
        self.run(event.angle)
    @staticmethod
    def transitions():
        return {DONE: state.State, vision.EventType.BUOY_FOUND : DBuoyAlign}

#target Aligning state
class TAlign(Align):
    def TARGET_FOUND(self,event):
        self.run(event.angle)
    @staticmethod
    def transitions():
        return {DONE: state.State, vision.EventType.TARGET_FOUND : TAlign}
