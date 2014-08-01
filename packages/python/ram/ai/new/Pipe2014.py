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

_YAW = {}

def setYaw(name, value):
    _YAW[name] = value

def getYaw(name):
    return _YAW.get(name, None)

class PipeCheckpoint(checkpoints.SaveCheckpoint):
    def __init__(self, checkpoint, nextCheckpoint, distanceToNextPipe):
        super(PipeCheckpoint, self).__init__(checkpoint, 0, 0)
        self._nextCheckpoint = nextCheckpoint
        self._transdecYawOffset = 0
        self._magnitude = distanceToNextPipe
        self._leaveCallbacks = { 'next' : self.storeNextPipe }

    def storeNextPipe(self):
        degrees = self.getStateMachine().getLegacyState().stateEstimator.getEstimatedOrientation().getYaw().valueDegrees()
        _YAW[self._checkpoint] = degrees
        radians = math.radians(self._transdecYawOffset + degrees)
                  
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

    def getYawMotion(self):
        currentOrientation = self.getStateEstimator().getEstimatedOrientation()
        traj = motion.trajectories.StepTrajectory(
            initialValue = currentOrientation,
            finalValue = math.Quaternion(math.Degree(_YAW.get(self._prev, 
                                                              None)), 
                                         math.Vector3.UNIT_Z),
            initialRate = self.getStateEstimator().getEstimatedAngularRate(),
            finalRate = math.Vector3.ZERO)
        mot = motion.basic.ChangeOrientation(traj)
        return mot

    def enter(self):
        self._motionID = self.getMotionManager().setMotion(self.getYawMotion())
        super(GotoPipe, self).enter(self)
        

class PipeAlign(utilStates.NestedState):
    def __init__(self, searchDist):
        super(PipeAlign, self).__init__(stateMachine.StateMachine())
        
        self._visionObj = utilClasses.PipeVisionObject(
            self.getInnerStateMachine().getLegacyState())

        self.getInnerStateMachine().addStates({
            'start' : utilStates.Start(),
            'search' : searches.ForwardsSearchPattern(searchDist,
                                                      utilClasses.hasQueryBeenFalse(2, self._visionObj.isSeen).query,
                                                      'buf1', 'buf3'),
            'buf1' : motionStates.Forward(0),
            'center' : approach.DownCenter(self._visionObj, 
                                           'buf2', 'buf3'),
            'buf2' : motionStates.Forward(0),
            'align' : approach.DownOrient(self._visionObj,
                                         'buf3', 'buf3'),
            'buf3' : motionStates.Forward(0),
            'end' : utilStates.End()
            })
        
        self.getInnerStateMachine().addTransitions(
            ('start' ,'next', 'search'),
            ('buf1', 'next', 'center'),
            ('buf2', 'next', 'align'),
            ('buf3', 'next', 'end')
            )

    def enter(self):
        self.getInnerStateMachine().getLegacyState().visionSystem.pipeLineDetectorOn()
        super(PipeAlign, self).enter()

    def leave(self):
        self.getInnerStateMachine().getLegacyState().visionSystem.pipeLineDetectorOff()
        super(PipeAlign, self).leave()
        
