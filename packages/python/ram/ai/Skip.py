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
        self.translate(0, 2, 0.15)

    def exit(self):
        utility.freeze(self)
        self.motionManager.stopCurrentMotion()

class BufferState(state.State):

    TRANS = core.declareEventType('TRANS')

    @staticmethod
    def transitions():
        return { BufferState.TRANS : Search }

    def enter(self):
        self.publish(BufferState.TRANS, core.Event())

class Search(utility.MotionState):

    @staticmethod
    def transitions():
        return { vision.EventType.PIPE_FOUND : End,
                 utility.DONE : End }

    def enter(self):
        self.visionSystem.pipeLineDetectorOn()
        self.translate( 0, 3, 0.1 )

    def exit(self):
        self.motionManager.stopCurrentMotion()
        utility.freeze(self)

class End(state.State):

    def enter(self):
        self.visionSystem.pipeLineDetectorOff()
        self.publish(COMPLETE, core.Event())
