import ram.ai.new.motionStates as motion
import ram.ai.new.utilStates as utilStates
import ram.ai.new.stateMachine as stateMachine
import ram.ai.Utility as oldUtil
import ram.ai.new.searchPatterns as search
import ram.ai.new.state as state



class Surrender(state.State):
    def __init__(self):
        super(Surrender,self).__init__()
        self.setTransition('end')

    def update(self):
        self.doTransitions('next')
    

class Approach(search.SearchPattern):
    def __init__(self, stopConditions, servoingStateMachine,success, failure, failCondition = lambda : False, recoverAction = Surrender, constraint = lambda : True):
        super(Approach,self).__init__(ServoingMachine(servoingStateMachine, failCondition, recoverAction), stopConditions, success, failure, constraint)

class ServoingMachine(stateMachine.stateMachine):
    def __init__(self, servoingStateMachine, failCondition, recoverAction):
        super(ServoingMachine,self).__init__()
        start = self.addState('start',utilStates.Start())
        end = self.addState('end',utilStates.End())
        #servo needs to be accesible, this is to allow for certain exit callbacks to get set if needed
        self._servo = self.addState('servo', util.ConstrainedState(servoingStateMachine, failCondition, 'end',  'recover'))
        self._recover = self.addState('recover', recoverAction)
        #ensure appropriate transitions are set for recovery state
        recover.setTransition('success', servo)
        recover.setTransition('failure', end)
        #route start to the visual servoing
        start.setTransition('servo')


class VisualServoingStateMachine(stateMachine.stateMachine):
    def __init__(self, servoingState):
        start = self.addState('start',utilStates.Start())
        end = self.addState('end',utilStates.End())
        servo = self.addState('servo', servoingState)
        #set transition path
        servoingState.setTransition('end')
        start.setTransition('servo')
    
