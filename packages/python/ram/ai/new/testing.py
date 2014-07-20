import time
import ram.ai.new.utilClasses as utilClasses
import searchPatterns as search
import ram.ai.new.utilStates as utilStates
import ram.ai.new.motionStates as motionStates

import ram.ai.new.acousticServoing as acousticServoing
import ram.ai.new.approach as approach

import ram.ai.new.gate as gate
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
        #pipe = utilClasses.OldSimulatorHackPipe(self.getLegacyState())
        pinger = utilClasses.OldSimulatorHackSonarObject(self.getLegacyState())
        start = self.addState('start',utilStates.Start())
        end = self.addState('end',utilStates.End())
        #center = self.addState('center', buoy.BuoySearchState(utilClasses.OldSimulatorHackVisionObject(self.getLegacyState()), 2, 100, 0.25, 2))
        center = self.addState('center', centering.SonarCenter(pinger, 'end', 'end', math.Vector3(0.0,0.0,3.0)))
        #align = self.addState('align', centering.DownOrient(pipe, 'end', 'end'))
        #acoustic = self.addState('acoustic', acousticServoing.AcousticServoing(pinger, math.Vector3(0.0,0.0,3.0)))
        start.setTransition('next', 'center')
        center.setTransition('complete', 'end')
        center.setTransition('failure', 'end')
