import ram.ai.new.state as state
import ram.ai.new.stateMachine as stateMachine

import time

import ext.math as math
import ram.motion as motion
from ram.motion.basic import Frame

class TestMachine(stateMachine.StateMachine):
    def configure(self, config):
        start = self.addState('start', utilStates.Start())
        dive = self.addState('dive', motionStates.Dive(8))
        forward = self.addState('forward', motionStates.Forward(1))
        right = self.addState('right', motionStates.Strafe(1))
        back = self.addState('back', motionStates.Move(-1, 0))
        left = self.addState('left', motionStates.Move(0, -1))
        turn = self.addState('turn', motionStates.Turn(10))
        end = self.addState('end', utilStates.End())

        start.setNextState('dive')
        dive.setNextState('forward')
        forward.setNextState('right')               
        right.setNextState('back') 
        back.setNextState('left')   
        left.setNextState('turn')                
        turn.setNextState('end')

        self.setStartState(start)
