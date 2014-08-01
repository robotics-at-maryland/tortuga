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

        self.addStates({
                'start':utilStates.Start(),
                'end': utilStates.End(),
                'orientation': approach.SonarOrient(pinger, 'buf1', 'buf4', math.Vector3(0.0,0.0,3.0)),
                'servoing1': approach.SonarCenter(pinger, 'buf2', 'buf4', math.Vector3(5.0,5.0,3.0), 0.15, 0.15),
                'servoing2': approach.SonarCenter(pinger, 'buf3', 'buf4', math.Vector3(0.0,0.0,3.0), .05, .05),
                'diveStart': motionStates.DiveTo(startDepth),
                'diveShort': motionStates.DiveTo(shortDepth),
                'diveEnd': motionStates.DiveTo(endDepth),
                'buf1': motionStates.Forward(0),
                'buf2': motionStates.Forward(0),
                'buf3': motionStates.Forward(0),
                'buf4': motionStates.Forward(0)})

        self.addTransitions(
            ('start', 'next', 'diveStart'),
            ('diveStart', 'next', 'servoing2'),#'orientation'),
            ('orientation', 'next', 'buf1'),
            ('buf1', 'next', 'servoing1'),
            ('buf2', 'next', 'diveShort'),
            ('diveShort', 'next', 'servoing2'),
            ('buf3', 'next', 'diveEnd'),
            ('diveEnd', 'next', 'end'),
            ('buf4', 'next', 'end'))
