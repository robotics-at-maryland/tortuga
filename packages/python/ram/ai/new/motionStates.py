import ext.math as math
from ext.control import yawVehicleHelper
import ram.motion as motion
from ram.motion.basic import Frame

from state import *
from stateMachine import *

@require_transitions('next')
class MotionState(State):
    def __init__(self):
        super(MotionState, self).__init__()
        self._motionID = -1

    def getMotionManager(self):
        return self.getStateMachine().getLegacyState().motionManager

    def getStateEstimator(self):
        return self.getStateMachine().getLegacyState().stateEstimator

    def getMotion(self):
        """Overridable method that returns what motion to perform."""
        pass

    def enter(self):
        self._motionID = self.getMotionManager().setMotion(self.getMotion())

    def update(self):
        if self.getMotionManager().queryMotionDone(self._motionID):
            self.doTransition('next')

class Move(MotionState):
    def __init__(self, x, y, rate = 0.15):
        super(Move, self).__init__()
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

class MoveTo(MotionState):
    def __init__(self, x, y, rate = 0.15):
        super(Move, self).__init__()
        self._vect = math.Vector2(x, y)
        self._rate = rate

    def getMotion(self):
        traj = motion.trajectories.Vector2CubicTrajectory(
            initialValue = self.getStateEstimator().getEstimatedPosition(),
            finalValue = self._vect,
            initialRate = self.getStateEstimator().getEstimatedVelocity(),
            avgRate = self._rate)
        mot = motion.basic.Translate(
            trajectory = traj,
            frame = Frame.GLOBAL)
        return mot

class MoveWithInitalRate(MotionState):
    def __init__(self, x, y, initalRate = 0.15, rate = 0.15):
        super(Move, self).__init__()
        self._vect = math.Vector2(x, y)
        self._initalRate = initalRate
        self._rate = rate

    def getMotion(self):
        traj = motion.trajectories.Vector2CubicTrajectory(
            initialValue = math.Vector2.ZERO,
            finalValue = self._vect,
            initialRate = self._initalRate,
            avgRate = self._rate)
        mot = motion.basic.Translate(
            trajectory = traj,
            frame = Frame.LOCAL)
        return mot

class Forward(Move):
    def __init__(self, distance, rate = 0.15):
        super(Forward, self).__init__(distance, 0, rate)

class ForwardWithInitalRate(MoveWithInitalRate):
    def __init__(self, distance, initalRate, rate):
        super(ForwardWithInitalRate, self).__init__(distance, 0, initalRate, rate)


class Strafe(Move):
    def __init__(self, distance, rate = 0.15):
        super(Strafe, self).__init__(0, distance, rate)

class Dive(MotionState):
    def __init__(self, depth, rate = 0.15):
        super(Dive, self).__init__()
        self._depth = depth
        self._rate = rate

    def getMotion(self):
        depth = self.getStateEstimator().getEstimatedDepth()
        traj = motion.trajectories.ScalarCubicTrajectory(
            initialValue = depth,
            finalValue = depth + self._depth,
            initialRate = self.getStateEstimator().getEstimatedDepthRate(),
            avgRate = self._rate)
        mot = motion.basic.ChangeDepth(trajectory = traj)
        return mot

class DiveTo(MotionState):
    def __init__(self, depth, rate = 0.15):
        super(DiveTo, self).__init__()
        self._depth = depth
        self._rate = rate

    def getMotion(self):
        traj = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.getStateEstimator().getEstimatedDepth(),
            finalValue = self._depth,
            initialRate = self.getStateEstimator().getEstimatedDepthRate(),
            avgRate = self._rate)
        mot = motion.basic.ChangeDepth(trajectory = traj)
        return mot

class Turn(MotionState):
    def __init__(self, angle):
        super(Turn, self).__init__()
        self._angle = angle

    def getMotion(self):
        currentOrientation = self.getStateEstimator().getEstimatedOrientation()
        initialRate = self.getStateEstimator().getEstimatedAngularRate()
        traj = motion.trajectories.SlerpTrajectory(
            initialValue = currentOrientation,
            finalValue = yawVehicleHelper(currentOrientation, 
                                          self._angle),
            #timePeriod = self._angle//,
            timePeriod = 1,
            initialTime = 0)
        mot = motion.basic.ChangeOrientation(traj)
        return mot

class TurnTo(MotionState):
    def __init__(self, angle):
        super(TurnTo, self).__init__()
        self._angle = angle

    def getMotion(self):
        currentOrientation = self.getStateEstimator().getEstimatedOrientation()
        traj = motion.trajectories.SlerpTrajectory(
            initialValue = currentOrientation,
            finalValue = math.Quaternion(math.Degree(self._angle), 
                                         math.Vector3.UNIT_Z),
            timePeriod = self._angle,
            initialTime = 0)
        mot = motion.basic.ChangeOrientation(traj)
        return mot
