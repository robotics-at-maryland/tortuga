import ram.ai.new.state as state
import ram.ai.new.stateMachine as stateMachine
import ram.ai.new.utilStates as utilStates
import ram.ai.new.motionStates as motionStates

import time

import ext.math as math
import ram.motion as motion
from ram.motion.basic import Frame

class TestMachine(stateMachine.StateMachine):
    def configure(self, config):
        start = self.addState(utilStates.Start('start'))
        dive = self.addState(motionStates.Dive('dive', 8))
        forward = self.addState(motionStates.Forward('forward', 1))
        right = self.addState(motionStates.Strafe('right', 1))
        back = self.addState(motionStates.Move('back', -1, 0))
        left = self.addState(motionStates.Move('left', 0, -1))
        turn = self.addState(motionStates.Turn('turn', 10))
        end = self.addState(utilStates.End('end'))

        start.setNextState(dive)    \
             .setNextState(forward) \
             .setNextState(right)   \
             .setNextState(back)    \
             .setNextState(left)    \
             .setNextState(turn)    \
             .setNextState(end)

        self.setStartState(start)
