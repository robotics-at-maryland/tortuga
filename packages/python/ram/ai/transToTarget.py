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
        self.translate(0, 4, 0.15)

    def exit(self):
        utility.freeze(self)
        self.motionManager.stopCurrentMotion()

class BufferState(state.State):

    TRANS = core.declareEventType('TRANS')

    @staticmethod
    def transitions():
        return { BufferState.TRANS : YawTar }

    def enter(self):
        self.publish(BufferState.TRANS, core.Event())

class YawTar(utility.MotionState):

    @staticmethod
    def transitions():
        return { utility.YAWED : YawTar,
                 utility.DONE : BufferStateTwo,
                 motion.basic.MotionManager.FINISHED : YawTar }

    def enter(self):
        self.yaw(-40, 10)

    def exit(self):
        utility.freeze(self)
        self.motionManager.stopCurrentMotion()

class BufferStateTwo(state.State):

    TRANS = core.declareEventType('TRANS')

    @staticmethod
    def transitions():
        return { BufferStateTwo.TRANS : Search }

    def enter(self):
        self.publish(BufferStateTwo.TRANS, core.Event())

class Search(utility.MotionState):

    @staticmethod
    def transitions():
        return { vision.EventType.TARGET_FOUND : End,
                 utility.DONE : End }

    def enter(self):
        self.visionSystem.cupidDetectorOn()
        self.translate( -3, 0, 0.08 )

    def exit(self):
        self.motionManager.stopCurrentMotion()
        utility.freeze(self)

class End(state.State):

    def enter(self):
        self.visionSystem.cupidDetectorOff()
        self.publish(COMPLETE, core.Event())
