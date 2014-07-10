import time
import ram.ai.new.utilClasses as utilClasses
import searchPatterns as search
import ram.ai.new.utilStates as utilStates
import ram.ai.new.motionStates as motionStates
import approach as centering

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
        pipe = utilClasses.OldSimulatorHackVisionObject(self.getLegacyState())
        start = self.addState('start',utilStates.Start())
        end = self.addState('end',utilStates.End())
        center = self.addState('center', centering.ForwardsCenter(pipe, 'end', 'end',10))
        start.setTransition('next', 'center')
