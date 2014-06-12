import ram.ai.state as state
import ram.ai.Utility as utility
import ram.ai.Approach as approach

import ext.core as core
import ext.math as math
import ext.vision as vision
import math as pmath

import ram.motion as motion
import ram.motion.search
from  ram.motion.basic import Frame
import ram.timer as timer

COMPLETE = core.declareEventType('COMPLETE')

class Skip(utility.MotionState):

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Skip,
                 utility.DONE : End }

    def enter(self):
        self.translate(0, 8.5, 0.15)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class Straighten(utility.MotionState):

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Straighten,
                 utility.YAWED : Straighten,
                 utility.DONE : End }

    def enter(self):
        self.yaw(-90, 15)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class End(state.State):

    def enter(self):
        self.publish(COMPLETE, core.Event())
