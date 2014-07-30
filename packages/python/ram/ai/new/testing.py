import time
import ram.ai.new.utilClasses as utilClasses
import searchPatterns as search
import ram.ai.new.utilStates as utilStates
import ram.ai.new.motionStates as motionStates

import ram.ai.new.acousticServoing as acousticServoing
import ram.ai.new.approach as approach
import ram.ai.new.checkpoints as checkpointsimport ram.ai.new.gate as gate
import ram.ai.new.Buoy2014 as buoy
#import ram.ai.new.SonarManip2014 as sonarm
import ram.ai.new.uprights as uprights


import ram.ai.new.Buoy2014 as buoy


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

        buoy = utilClasses.BuoyVisionObject(self.getLegacyState())

        self.addStates({
            'start' : utilStates.Start(),
            'forward' : gate.GateTask(buoy, 4, 2, 2, 'end', 'end'),
            'end' : utilStates.End()
          })

        self.addTransitions(
            ('start', 'next', 'forward'),
            ('forward', 'next', 'end')
          )

