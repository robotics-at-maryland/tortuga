import time
import ram.ai.new.utilClasses as utilClasses
import searchPatterns as search
import ram.ai.new.utilStates as utilStates
import ram.ai.new.motionStates as motionStates
import ram.ai.new.approach as approach
import ram.ai.new.motionStates as motionStates

import ram.ai.new.gate as gate
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

        buoyVis = utilClasses.OldSimulatorHackVisionObject(self.getLegacyState())

        self.addStates({
            'start'     : utilStates.Start(),
            'search'    : buoy.BuoySearchState(buoyVis, -2, 3, 0.25, 1),
            'end'       : utilStates.End()
            })

        self.addTransitions([
            ('start'    , 'next'    , 'search'  ),
            ('search'   , 'complete' , 'end'     ),
            ('search'   , 'failure' , 'end'     )
            ])

        print 'testing'
