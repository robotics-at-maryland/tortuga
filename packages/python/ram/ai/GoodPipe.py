
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

COMPLETE = core.declareEventType('DONE')

class Start(state.State):

    @staticmethod
    def transitions():
        return {vision.EventType.PIPE_FOUND : Centering}

    def enter(self):
        self.visionSystem.pipeLineDetectorOn()

    def exit(self):
        pass

class Centering(vs.DHyperApproach):

    @staticmethod
    def getattr():
        return { 'kx' : .15 ,  'ky' : .4 , 'kz' : 9, 'x_d' : 0, 'r_d' : 50 , 'y_d' : 0, 'x_bound': .15, 'r_bound': 20, 'y_bound':.15 ,'minvx': .2, 'minvy': .2 ,'minvz' : .1}   

    def enter(self):
        pass

    def PIPE_FOUND(self,event):
        self.run(event)

    def decideZ(self,event):
        return 0

    @staticmethod
    def transitions():
        return {vs.DONE : Buffer , vision.EventType.PIPE_FOUND : Centering}

    def exit(self):
        util.freeze(self)
        self.motionManager.stopCurrentMotion()
#--
# only used for NBRF pipe following
#--

class Buffer(state.State):

    TRANS = core.declareEventType('TRANS')

    @staticmethod
    def transitions():
        return { Buffer.TRANS : Rotate }

    def enter(self):
        self.publish(Buffer.TRANS, core.Event())

class Rotate(util.MotionState):

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Rotate,
                 util.YAWED : Rotate,
                 util.DONE : Align }

    def enter(self):
        self.yaw(10,3)

    def exit(self):
        util.freeze(self)
        self.motionManager.stopCurrentMotion()
#--

class Align(state.State):
    FIN = core.declareEventType('FIN')
    
    def enter(self):
        pass

    @staticmethod
    def getattr():
        return {'yaw_bound' : 1.0}
    def PIPE_FOUND(self,event):

        if(abs(event.angle.valueDegrees()) < self._yaw_bound):
            self.visionSystem.pipeLineDetectorOff()
            self.publish(Align.FIN,core.Event())
        else:
            currentOrientation = self.stateEstimator.getEstimatedOrientation()
            yawTrajectory = motion.trajectories.StepTrajectory(
            initialValue = currentOrientation,
            finalValue = yawVehicleHelper(currentOrientation, 
                                          -event.angle.valueDegrees()),
            initialRate = self.stateEstimator.getEstimatedAngularRate(),
            finalRate = math.Vector3.ZERO)
            yawMotion = motion.basic.ChangeOrientation(yawTrajectory)
            self.motionManager.setMotion(yawMotion) 
            
    @staticmethod
    def transitions():
        return {Align.FIN: End, vision.EventType.PIPE_FOUND : Align}

    def exit(self):
        self.motionManager.stopCurrentMotion()


class End(state.State):

    def enter(self):
        self.visionSystem.pipeLineDetectorOff()
        self.publish(COMPLETE, core.Event())
