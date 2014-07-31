import time
import ram.ai.new.utilClasses as utilClasses
import searchPatterns as search
import ram.ai.new.utilStates as utilStates
import ram.ai.new.motionStates as motionStates
import ram.ai.new.approach as approach

import ram.ai.new.gate as gate
import ram.ai.new.uprights as uprights
import ram.ai.new.acousticServoing as acousticServoing
import ram.ai.new.acousticOrientation as acousticOrientation

from state import *
from stateMachine import *

import ext.math as math
import ram.motion as motion
from ram.motion.basic import Frame

class SonarNavigationTask(utilStates.Task):
    def __init__(self, sonarObject, success, failure, duration):
        super(SonarNavigationTask, self).__init__(SonarNavigation(), success, failure, duration)

class SonarNavigation(StateMachine):
    def __init__(self):
        super(SonarNavigation, self).__init__()
        
        pinger = utilClasses.OldSimulatorHackSonarObject(self.getLegacyState())
        
        start = self.addState('start',utilStates.Start())
        end = self.addState('end',utilStates.End())
        orientation =  self.addState('orientation', approach.SonarOrient(pinger, 'end', 'end', math.Vector3(0.0,0.0,3.0)))
        servoing = self.addState('servoing', approach.SonarCenter(pinger, 'end', 'end', math.Vector3(0.0,0.0,3.0), .1, .1))
        
        start.setTransition('next', 'orientation')
        orientation.setTransition('next', 'servoing')
        servoing.setTransition('next', 'end')
