
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

DONE = core.declareEventType('DONE')
class Centering(vs.DHyperApproach):

    @staticmethod
    def getattr():
        return { 'kx' : .15 ,  'ky' : .4 , 'kz' : 9, 'x_d' : 0, 'r_d' : 50 , 'y_d' : 0, 'x_bound': .05, 'r_bound': 20, 'y_bound':.025 ,'minvx': .1, 'minvy': .1 ,'minvz' : .1}   

    def PIPE_FOUND(self,event):
        run(event):

    def decideZ(self,event):
        return 0

    @staticmethod
    def transitions():
        return {vs.DONE : Align , vision.EventType.PIPE_FOUND : Centering}

class Align(state.State):
    @staticmethod
    def getattr():
        return {'yaw_bound' : .05}
    def PIPE_FOUND(self,event):
        if((abs(event.angle)-90) < self._yaw_bound):
            self.publish(DONE,core.Event())
        else:
            currentOrientation = self.stateEstimator.getEstimatedOrientation()
            yawTrajectory = motion.trajectories.StepTrajectory(
            initialValue = currentOrientation,
            finalValue = yawVehicleHelper(currentOrientation, 
                                          -event.angle),
            initialRate = self.stateEstimator.getEstimatedAngularRate(),
            finalRate = math.Vector3.ZERO)
            yawMotion = motion.basic.ChangeOrientation(yawTrajectory)
            self.motionManager.setMotion(yawMotion) 
            
    @staticmethod
    def transitions():
        return {DONE: state.State, vision.EventType.PIPE_FOUND : Align}
