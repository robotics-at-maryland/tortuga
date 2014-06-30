import ram.ai.new.state as state
import ram.ai.new.stateMachine as stateMachine

import time
import utilClasses
import searchPatterns as search
import ram.ai.new.utilStates as utilStates
import ram.ai.new.motionStates as motionStates

import ext.math as math
import ram.motion as motion
from ram.motion.basic import Frame

#a terrible pun
def reverseFun(fun):
    def rev():
        return not fun()
    return rev

class TestMachine(stateMachine.StateMachine):
    def configure(self, config):
        start = self.addState('start', utilStates.Start())
        testForwardsSearch = self.addState('search', search.ForwardsSearchPattern(1,reverseFun(utilClasses.timer(100).check),'dive','end', utilClasses.timer(100).check))
        dive = self.addState('dive', motionStates.Dive(8))
        forward = self.addState('forward', motionStates.Forward(1))
        right = self.addState('right', motionStates.Strafe(1))
        back = self.addState('back', motionStates.Move(-1, 0))
        left = self.addState('left', motionStates.Move(0, -1))
        turn = self.addState('turn', motionStates.Turn(10))
        end = self.addState('end', utilStates.End())

        start.setNextState('search')
        dive.setNextState('forward')
        forward.setNextState('right')               
        right.setNextState('back') 
        back.setNextState('left')   
        left.setNextState('turn')                
        turn.setNextState('end')

        self.setStartState(start)

class TesterState(utilStates.NestedState):
    def __init__(self):
        super(TesterState,self).__init__(TestMachine)
