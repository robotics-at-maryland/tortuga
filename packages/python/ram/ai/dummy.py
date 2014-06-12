import ram.ai.state as state
import ext.core as core
import ram.ai.Utility as utility
import ram.motion as motion

class one(utility.MotionState):
    DROPPED = core.declareEventType('DROPPED')
    def enter(self):
        self.translate(-0.5,0,.1)
    def exit(self):
        pass

    
