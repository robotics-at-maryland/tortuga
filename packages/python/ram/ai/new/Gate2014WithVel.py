import ram.ai.new.utilClasses as utilClasses
import ram.ai.new.utilStates as utilStates
import ram.ai.new.motionStates as motionStates
import ram.ai.new.searchPatterns as searches
import ram.ai.new.stateMachine as stateMachine
import ram.ai.new.state as state
import ram.ai.new.approach as approach

import ram.motion as motion
from ram.motion.basic import Frame
import ext.math as ram_math
import ram.ai.Utility as oldUtils

import time
import math

class GateTask(utilStates.Task):
    def __init__(self, taskDepth, forwardDistance,
                 success, failure, duration = 120):
        super(GateTask, self).__init__(GateTaskMachine(taskDepth,
                                                       forwardDistance),
                                       success, failure,
                                       duration)

    def update(self):
        if self.getInnerStateMachine().isCompleted() and \
                isinstance(self.getInnerStateMachine().getCurrentState(), 
                           GateFailure):
            self.doTransition(failure)
        else:
            super(GateTask, self).update()


class GateTaskMachine(stateMachine.StateMachine):
    def __init__(self, taskDepth, forwardDistance):
        super(GateTaskMachine, self).__init__()
        

        # Add states
        start = self.addState('start', utilStates.Start())
        end = self.addState('end', utilStates.End())
        failure = self.addState('failure', GateFailure())
        dive = self.addState('dive', motionStates.DiveTo(taskDepth))
        forward = self.addState('forward', GateForward(forwardDistance))

        start.setTransition('next', 'dive')
        dive.setTransition('next', 'forward')
        forward.setTransition('next', 'end')

    def update(self):
        super(GateTaskMachine, self).update()

class GateFailure(utilStates.End):
    def __init__(self):
        super(GateFailure, self).__init__()
    
class GateForward(motionStates.MotionState):
    def __init__(self, distance, rate = 0.5, count = 0):
        super(GateForward, self).__init__()
        self._count = count
        self._rate = rate
        self._distance = distance

    def runMotion(self):
        self.motionManager._controller.moveVel(self._rate, 0, 0)

    def enter(self):
        self._start = time.time()
        self._counter = 0
        self._startVect = self.getStateEstimator().getEstimatedPosition()
        self.motionManager = self.getStateMachine().getLegacyState().motionManager

    def update(self):
        currVect = self.getStateEstimator().getEstimatedPosition()
        travelDist = math.sqrt(math.pow(currVect.y - self._startVect.y, 2) + \
                                   math.pow(currVect.x - self._startVect.x, 2))
        #print 'Update, distance traveled: %f' % travelDist
        if travelDist >= self._distance:
            oldUtils.freeze(self.getStateMachine().getLegacyState())
            self.doTransition('next')
        elif self._counter == 0:
            self.runMotion()
        elif self._counter >= self._count:
            self._counter = 0
        else:
            self._counter += 1



