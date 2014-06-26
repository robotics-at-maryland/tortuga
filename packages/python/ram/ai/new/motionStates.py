import ram.ai.new.state as state
import ram.ai.new.stateMachine as stateMachine

import ext.math as math
import ram.motion as motion
from ram.motion.basic import Frame

class MotionState(state.State):

    def getMotionManager(self):
        return self.getStateMachine().getLegacyState().motionManager

    def getStateEstimator(self):
        return self.getStateMachine().getLegacyState().stateEstimator

    def configure(self, *args, **kwargs):
        self.setEventCallback('FINISHED', self.finished)

    def finished(self, event):
        self.fireTransition('finished')
