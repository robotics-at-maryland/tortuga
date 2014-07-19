import time
import ram.ai.new.utilClasses as utilClasses
import searchPatterns as search
import ram.ai.new.utilStates as utilStates
import ram.ai.new.motionStates as motionStates
import ram.ai.new.approach as approach

import ram.ai.new.gate as gate
import ram.ai.new.uprights as uprights
import ram.ai.new.sonarNavigation as sonarNavigation

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
        pipe = utilClasses.PipeVisionObject(self.getLegacyState())
        pinger = utilClasses.OldSimulatorHackSonarObject(self.getLegacyState())
        
        start = self.addState('start',utilStates.Start())
        end = self.addState('end',utilStates.End())

        sonar = self.addState('sonar', sonarNavigation.SonarNavigationTask(pinger, 
                                                                 'end',
                                                                 'end',
                                                                 300)) 

        start.setTransition('next', 'sonar')


