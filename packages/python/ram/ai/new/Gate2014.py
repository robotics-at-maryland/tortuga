import ram.ai.new.utilClasses as utilClasses
import ram.ai.new.utilStates as utilStates
import ram.ai.new.motionStates as motion
import ram.ai.new.searchPatterns as searches
import ram.ai.new.stateMachine as stateMachine
import ram.ai.new.state as state
import ram.ai.new.approach as approach

class GateTask(utilStates.Task):
    def __init__(self, taskDepth, forwardDistance,  
                 success, failure, duration = 120):
        super(GateTask, self).__init__(GateTaskMachine(taskDepth, 
                                                       forwardDistance),
                                       success, failure,
                                       duration)

        self.getInnerStateMachine.addStates(
            {'start' : utilState.State(),
             'end' : utilState.End(),
             'dive' : motionStates.DiveTo(taskDepth),
             'forward' : motionStates.Forward(forwardDistance)})
 
        self.getInnerStateMachine.addTransitions(
            ('start', 'next', 'dive'),
            ('dive', 'next', 'forward'),
            ('forward', 'next', 'end'))

    def update(self):
        if self._InnerMachine().isCompleted() and \
                isinstance(self._InnerMachine().getCurrentState(), 
                           GateFailure):
            self.doTransition(failure)
        else:
            super(GateTask, self).update()


class GateTaskMachine(stateMachine.StateMachine):
    def __init__(self, taskDepth, forwardDistance):
        super(GateTaskMachine, self).__init__()

    def update(self):
        super(GateTaskMachine, self).update()

    
