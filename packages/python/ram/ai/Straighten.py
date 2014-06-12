import ram.ai.state as state
import ram.ai.Utility as utility
import ext.core as core
import ram.motion as motion
import ram.motion.basic

COMPLETE = core.declareEventType('COMPLETE')

class Start(utility.MotionState):

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Start,
                 utility.YAWED : Start,
                 utility.DONE : End }

    def enter(self):
        self.yaw(-90, 15)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class End(state.State):

    def enter(self):
        self.publish(COMPLETE, core.Event())
