import ext.math as math
import ram.motion as motion
from ram.motion.basic import Frame

from ram.ai.new.state import *
from ram.ai.new.stateMachine import *

import ram.ai.new.motionStates as motionStates

_CHECKPOINTS = {}

def getCheckpoint(checkpoint):
    return _CHECKPOINTS.get(checkpoint, None)

def setCheckpoint(checkpoint, vector):
    _CHECKPOINTS[checkpoint] = vector

@require_transitions('next')
class SaveCheckpoint(State):
    def __init__(self, checkpoint = 'default', x_offset = 0, y_offset = 0):
        super(SaveCheckpoint, self).__init__()
        self._checkpoint = checkpoint
        self._offset = math.Vector2(x_offset, y_offset)
    
    def enter(self):
        pos = self.getStateMachine().getLegacyState().stateEstimator.getEstimatedPosition()
        pos += self._offset
        _CHECKPOINTS[self._checkpoint] = pos
        self.doTransition('next')


class GotoCheckpoint(motionStates.MoveTo):
    def __init__(self, checkpoint = 'default', rate = 0.15, x_offset = 0, y_offset = 0):
        super(GotoCheckpoint, self).__init__(0, 0, rate)
        self._checkpoint = checkpoint
        self._offset = math.Vector2(x_offset, y_offset)

    def getMotion(self):
        self._vect = _CHECKPOINTS[self._checkpoint] + self._offset
        return super(GotoCheckpoint, self).getMotion()
