# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/ai/safe.py


# Project Imports
import ext.core as core
import ext.vision as vision
import ext.math as math

import ram.ai.state as state
import ram.motion as motion
import ram.motion.safe
#import ram.motion.search
import ram.motion.common

from ram.ai.sonar import PingerState, TranslationSeeking
import ram.ai.safe as safe

COMPLETE = core.declareEventType('COMPLETE')
        
class Settling(TranslationSeeking):
    SETTLED = core.declareEventType('SETTLED')
    
    @staticmethod
    def transitions():
        return TranslationSeeking.transitions(Settling,
            { Settling.SETTLED : Dive })
    
    def enter(self):
        duration = self._config.get('duration', 20)
        self.timer = self.timerManager.newTimer(Settling.SETTLED, duration)
        self.timer.start()
        
        TranslationSeeking.enter(self)
    
class Dive(TranslationSeeking):
    """Diving to the pre-grab depth"""

    @staticmethod
    def transitions():
        return TranslationSeeking.transitions(Dive,
            { ram.motion.basic.Motion.FINISHED : PreGrabSettling })
        
    def enter(self):
        safeDepth = self._config.get('safeDepth', 22)
        offset = self._config.get('depthOffset', 2)
        diveRate = self._config.get('diveRate', 0.4)
        
        targetDepth = safeDepth - offset
        diveMotion = motion.basic.RateChangeDepth(targetDepth, diveRate)
        self.motionManager.setMotion(diveMotion)
        
        TranslationSeeking.enter(self)
        
class PreGrabSettling(TranslationSeeking):
    SETTLED = core.declareEventType('SETTLED')
    
    @staticmethod
    def transitions():
        return TranslationSeeking.transitions(PreGrabSettling,
            { PreGrabSettling.SETTLED : Grabbing })
    
    def enter(self):
        duration = self._config.get('duration', 10)
        self.timer = self.timerManager.newTimer(PreGrabSettling.SETTLED, 
                                                duration)
        self.timer.start()
        
        TranslationSeeking.enter(self)
        
class Grabbing(safe.Grabbing):
    @staticmethod
    def transitions():
        trans = safe.Grabbing.transitions()
        trans.update({safe.Grabbing.GRABBED : Surface,
                      ram.motion.basic.Motion.FINISHED : Surface })
        return trans
    
class Surface(safe.Surface):
    pass
