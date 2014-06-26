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
        forward = self.addState(motionStates.Move('forward', 1, 0))
        right = self.addState(motionStates.Move('right', 0, 1))
        back = self.addState(motionStates.Move('back', -1, 0))
        left = self.addState(motionStates.Move('left', 0, -1))
        end = self.addState(utilStates.End('end'))

        start.setNextState('next', forward)
        forward.setNextState('next', right)
        right.setNextState('next', back)
        back.setNextState('next', left)
        left.setNextState('next', end)

        self.setStartState(start)
