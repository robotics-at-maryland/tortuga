import ram.ai.state as state
import ram.ai.Utility as utility
import ext.core as core
import ram.motion as motion
import ram.motion.basic
import ext.vision as vision

COMPLETE = core.declareEventType('COMPLETE')

class Start(utility.MotionState):

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Start,
                 utility.DONE : BufferState }

    def enter(self):
        self.dive(5, 0.15)

    def exit(self):
        self.motionManager.stopCurrentMotion()
        utility.freeze(self)

class BufferState(state.State):

    FINISHED = core.declareEventType('FINISHED')

    @staticmethod
    def transitions():
        return { BufferState.FINISHED : GlobYaw }

    def enter(self):
        self.publish(BufferState.FINISHED, core.Event())

class GlobYaw(utility.MotionState):

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : GlobYaw,
                 utility.YAWED : GlobYaw,
                 utility.DONE : End }

    def enter(self):
        self.yawGlobal(5, 10)

    def exit(self):
        self.motionManager.stopCurrentMotion()
        utility.freeze(self)

class End(state.State):

    def enter(self):
        self.publish(COMPLETE, core.Event())

    
