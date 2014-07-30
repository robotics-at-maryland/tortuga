import time
import ram.ai.new.utilClasses as utilClasses
import searchPatterns as search
import ram.ai.new.utilStates as utilStates
import ram.ai.new.motionStates as motionStates

import ram.ai.new.acousticServoing as acousticServoing
import ram.ai.new.approach as approach
import ram.ai.new.checkpoints as checkpoints

import ram.ai.new.gate as gate
import ram.ai.new.Buoy2014 as buoy
#import ram.ai.new.SonarManip2014 as sonarm
import ram.ai.new.uprights as uprights


import ram.ai.new.Buoy2014 as buoy
import ram.ai.new.Pipe2014 as pipe

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
        
        self.addStates({
            'start' : utilStates.Start(),
            'dive' : motionStates.Dive(4),
            
            'save' : checkpoints.SaveCheckpoint(checkpoint = 'test'),
            'forward' : motionStates.Move(4, 2),
            'return' : checkpoints.GotoCheckpoint(checkpoint = 'test',
                                                  x_offset = 2,
                                                  y_offset = 2),

            'end' : utilStates.End()
          })

        self.addTransitions(
            ('start', 'next', 'dive'),
            ('dive', 'next', 'save'),
            ('save', 'next', 'forward'),
            ('forward', 'next', 'return'),
            ('return', 'next', 'end'),
          )
