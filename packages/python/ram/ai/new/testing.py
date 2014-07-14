import time
import ram.ai.new.utilClasses as utilClasses
import searchPatterns as search
import ram.ai.new.utilStates as utilStates
import ram.ai.new.motionStates as motionStates
import ram.ai.new.approach as approach
import ram.ai.new.motionStates as motionStates

import ram.ai.new.gate as gate

from state import *
from stateMachine import *

import ext.math as math
import ram.motion as motion
from ram.motion.basic import Frame

#a terrible pun
def reverseFun(fun):
    def rev():
        return not fun()
    return rev

class TestMachine(StateMachine):
    def __init__(self):
        super(TestMachine, self).__init__()

        pipe = utilClasses.OldSimulatorHackPipe(self.getLegacyState())
        
        start = self.addState('start',utilStates.Start())
        end = self.addState('end',utilStates.End())
        gateTask = self.addState('gate', gate.GateTask(pipe, 4, .5, 4,
                                                       'end', 'yaw', 
                                                       300))
        yaw = self.addState('yaw', motionStates.Turn(30))
        start.setTransition('next', 'gate')

        yaw.setTransition('next', 'end')
        print 'testing'
