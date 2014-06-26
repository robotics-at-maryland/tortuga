import ram.ai.new.state as state
import ram.ai.new.stateMachine as stateMachine

import time

import ext.math as math
import ram.motion as motion
from ram.motion.basic import Frame

from motionStates import MotionState

class testTranslation(MotionState):
    def enter(self):
        print "moving..."
        
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = math.Vector2.ZERO,
            finalValue = math.Vector2(self.x, self.y),
            initialRate = self.getStateEstimator().getEstimatedVelocity(),
            avgRate = 0.15)
        translateMotion = motion.basic.Translate(
            trajectory = translateTrajectory,
            frame = Frame.LOCAL)
        self.getMotionManager().setMotion(translateMotion)

class testEnd(state.State):
    def enter(self):
        print "at the end"
        self.getStateMachine().setComplete()

class TestMachine(stateMachine.StateMachine):
    def __init__(self):
        super(TestMachine, self).__init__()

        test1 = self.createState('test1', testTranslation)
        test2 = self.createState('test2', testTranslation)
        test3 = self.createState('test3', testTranslation)
        test4 = self.createState('test4', testTranslation)
        test5 = self.createState('test5', testEnd)

        test1.x, test1.y = 1, 0
        test1.setNextState('finished', test2)
        test1.configure()

        test2.x, test2.y = 0, 1
        test2.setNextState('finished', test3)
        test2.configure()

        test3.x, test3.y = -1, 0
        test3.setNextState('finished', test4)
        test3.configure()

        test4.x, test4.y = 0, -1
        test4.setNextState('finished', test5)
        test4.configure()

        self.setStartState(test1)
