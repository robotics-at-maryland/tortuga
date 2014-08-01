import time
import ram.ai.new.utilClasses as utilClasses
import searchPatterns as search
import ram.ai.new.utilStates as utilStates
import ram.ai.new.motionStates as motionStates
import ram.ai.new.approach as approach

import ram.ai.new.acousticServoing as acousticServoing
import ram.ai.new.acousticOrientation as acousticOrientation

from state import *
from stateMachine import *

import ext.math as math
import ram.motion as motion
from ram.motion.basic import Frame

class SonarNavigationTask(utilStates.Task):
    def __init__(self, success, failure, startDepth, shortDepth, endDepth, 
                 duration):
        super(SonarNavigationTask, self).__init__(SonarNavigation(startDepth, shortDepth, endDepth), success, failure, duration)

class SonarNavigation(StateMachine):
    def __init__(self, startDepth, shortDepth, endDepth):
        super(SonarNavigation, self).__init__()
        
        pinger = utilClasses.OldSimulatorHackSonarObject(self.getLegacyState())
        
        start = self.addState('start',utilStates.Start())
        end = self.addState('end',utilStates.End())
        orientation =  self.addState('orientation', approach.SonarOrient(pinger, 'end', 'end', math.Vector3(0.0,0.0,3.0)))
        broadRange = self.addState('servoing1', approach.SonarCenter(pinger, 'diveShort', 'end', math.Vector3(5.0,5.0,3.0), 0.15, 0.15))
        servoing = self.addState('servoing2', approach.SonarCenter(pinger, 'diveEnd', 'end', math.Vector3(0.0,0.0,3.0), .1, .1))

        startDive = self.addState('diveStart', motionStates.DiveTo(startDepth))
        shortDive = self.addState('diveShort', motionStates.DiveTo(shortDepth))
        endDive = self.addState('diveEnd', motionStates.DiveTo(endDepth))

        start.setTransition('next', 'diveStart')
        startDive.setTransition('next', 'orientation')
        orientation.setTransition('next', 'servoing1')
        shortDive.setTransition('next', 'servoing2')
        endDive.setTransition('next', 'end')
