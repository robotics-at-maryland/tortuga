import ram.ai.new.state as state
import ram.ai.new.stateMachine as stateMachine

import ext.math as math
import ram.motion as motion
from ram.motion.basic import Frame

class MotionState(state.State):
    def __init__(self, name):
        super(MotionState, self).__init__(name)
        self._motionID = -1

    def getMotionManager(self):
        return self.getStateMachine().getLegacyState().motionManager

    def getStateEstimator(self):
        return self.getStateMachine().getLegacyState().stateEstimator

    def getMotion(self):
        '''Overridable method that returns what motion to perform.'''
        pass

    def enter(self):
        self._motionID = self.getMotionManager().setMotion(self.getMotion())

    def update(self):
        if self.getMotionManager().queryMotionDone(self._motionID):
            self.doTransition('next')

class Move(MotionState):
    def __init__(self, name, x, y, rate = 0.15):
        super(Move, self).__init__(name)
        self._vect = math.Vector2(x, y)
        self._rate = rate

    def getMotion(self):
        traj = motion.trajectories.Vector2CubicTrajectory(
            initialValue = math.Vector2.ZERO,
            finalValue = self._vect,
            initialRate = self.getStateEstimator().getEstimatedVelocity(),
            avgRate = self._rate)
        mot = motion.basic.Translate(
            trajectory = traj,
            frame = Frame.LOCAL)
        return mot
