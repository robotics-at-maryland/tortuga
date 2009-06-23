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

from ram.ai.sonar import PingerState, TranslationSeeking,\
    PingerLost, CloseSeeking
import ram.ai.safe as safe

COMPLETE = core.declareEventType('COMPLETE')
        
class Settling(TranslationSeeking):
    SETTLED = core.declareEventType('SETTLED')
    
    @staticmethod
    def transitions():
        return TranslationSeeking.transitions(Settling, PingerSettling,
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
        return TranslationSeeking.transitions(Dive, PingerDive,
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
    SETTLED = core.declareEventType('_SETTLED')
    MOVE_ON = core.declareEventType('_MOVE_ON')    

    @staticmethod
    def transitions():
        return TranslationSeeking.transitions(PreGrabSettling,
            PingerPreGrabSettling,
            { PreGrabSettling.SETTLED : PreGrabSettling,
              TranslationSeeking.CLOSE : PreGrabSettling,
              PreGrabSettling.MOVE_ON : Grabbing })
    
    def CLOSE(self, event):
        if self._timerDone:
            self.publish(PreGrabSettling.MOVE_ON, core.Event())

    def _SETTLED(self, event):
        self._timerDone = True

    def enter(self):
        duration = self._config.get('duration', 10)
        self._timerDone = False

        self.timer = self.timerManager.newTimer(PreGrabSettling.SETTLED, 
                                                duration)
        self.timer.start()
        
        TranslationSeeking.enter(self)

        self._closeZ = self._config.get('closeZ', 0.97)
        
class Grabbing(safe.Grabbing):
    @staticmethod
    def transitions():
        trans = safe.Grabbing.transitions()
        trans.update({safe.Grabbing.GRABBED : Surface,
                      ram.motion.basic.Motion.FINISHED : Surface })
        return trans
    
class Surface(safe.Surface):
    pass

class SafeCloseSeeking(CloseSeeking):
    @staticmethod
    def transitions():
        return TranslationSeeking.transitions(SafeCloseSeeking, PingerLost,
            { TranslationSeeking.CLOSE : Settling } )

    def enter(self):
        CloseSeeking.enter(self, timeout = 0)

class PingerSettling(PingerLost):
    @staticmethod
    def transitions():
        return PingerLost.transitions(Settling, SafeCloseSeeking)

class PingerDive(PingerLost):
    @staticmethod
    def transitions():
        return PingerLost.transitions(Dive, Settling)

class PingerPreGrabSettling(PingerLost):
    @staticmethod
    def transitions():
        return PingerLost.transitions(PreGrabSettling, Settling)
