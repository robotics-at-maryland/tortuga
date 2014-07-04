import time
import utilClasses
import searchPatterns as search
import ram.ai.new.utilStates as utilStates
import ram.ai.new.motionStates as motionStates

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
        s = self.addStates({
                'start' : utilStates.Start(),
                'forward' : search.ForwardsSearchPattern(2, lambda: False, 'end', 'end'),
                'end' : utilStates.End()
            })

        s['start'].setTransition('next', 'forward')
        s['forward'].setTransition('next', 'end')
