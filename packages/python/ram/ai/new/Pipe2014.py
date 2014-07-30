import ram.ai.new.utilClasses as utilClasses
import ram.ai.new.utilStates as utilStates
import ram.ai.new.motionStates as motionStates
import ram.ai.new.searchPatterns as searches
import ram.ai.new.stateMachine as stateMachine
import ram.ai.new.state as state
import ram.ai.new.approach as approach
import ram.ai.new.checkpoints as checkpoints

import ext.math as ram_math
import ram.motion as motion
from ram.motion.basic import Frame

import math

class PipeCheckpoint(checkpoints.SaveCheckpoint):
    def __init__(self, checkpoint, nextCheckpoint, distanceToNextPipe):
        super(PipeCheckpoint, self).__init__(checkpoint, 0, 0)
        self._nextCheckpoint = nextCheckpoint
        self._transdecYawOffset = 0
        self._magnitude = distanceToNextPipe
        self._leaveCallbacks = { 'next' : self.storeNextPipe }

    def storeNextPipe(self):
        radians = self.getStateMachine().getLegacyState().stateEstimator.getEstimatedOrientation().getYaw().valueDegrees()
        radians = math.radians(self._transdecYawOffset + radians)
                  
        x_offset = math.cos(radians) * self._magnitude
        y_offset = math.sin(radians) * self._magnitude
        vec = checkpoints.getCheckpoint(self._checkpoint) + \
              ram_math.Vector2(x_offset, y_offset)
        checkpoints.setCheckpoint(self._nextCheckpoint, vec)


class GotoPipe(motionStates.Move):
    def __init__(self, prevCheckpoint, nextCheckpoint, rate = 0.15):
        super(GotoPipe, self).__init__(0, 0, rate)
        self._prev = prevCheckpoint
        self._next = nextCheckpoint

    def getMotion(self):
        prevLoc = checkpoints.getCheckpoint(self._prev)
        nextLoc = checkpoints.getCheckpoint(self._next)
        currentLoc = self.getStateMachine().getLegacyState().stateEstimator.getEstimatedPosition()
        self._vect = (nextLoc - prevLoc) - (currentLoc - prevLoc)
        return super(GotoPipe, self).getMotion()

class PipeAlign(utilStates.NestedState):
    def __init__(self, searchDist):
        super(PipeAlign, self).__init__()
        
        self._visionObj = utilClasses.PipeVisionObject(
            self.getInnerStateMachine().getLegacyState())

        self.getInnerStateMachine().addStates({
            'start' : utilStates.Start(),
            'search' : searches.ForwardsSearchMachine(searchDist
                                                      self._visionObj.isSeen,
                                                      'buf1', 'end'),
            'buf1' : motionStates.Forward(0),
            'center' : approach.DownCenter(self._visionObj, 
                                           'buf2', 'end'),
            'buf2' : motionStates.Forward(0),
            'align' : approach.DownAlign(self._visionObj,
                                         'end', 'end'),
            'end' : utilStates.End()
            })
        
        self.getInnerStateMachine().addTransitions(
            ('start' ,'next', 'search'),
            ('buf1', 'next', 'center'),
            ('buf2', 'next', 'align')
            )
        
