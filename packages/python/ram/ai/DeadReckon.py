import ext.core as core
import ext.control as control
from ext.control import yawVehicleHelper

import ram.motion as motion
import ram.motion.basic
import ram.ai.Utility as utility

import ram.ai.state as state

COMPLETE = core.declareEventType('COMPLETE')
global yawValue
global dist
dist = 30
yawValue = 25


class Start(utility.MotionState):

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Start,
                 utility.YAWED : Start,
                 utility.DONE : Forward }

    def enter(self):
        global yawValue
        self.yawGlobal(yawValue, 20)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class Forward(utility.MotionState):

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Forward,
                 utility.DONE : End }

    def enter(self):
        global dist
        self.translate(0, dist, 0.4)

class End(state.State):

    def enter(self):
        self.publish(COMPLETE, core.Event())
