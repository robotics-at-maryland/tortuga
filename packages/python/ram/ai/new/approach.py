import ram.ai.new.motionStates as motion
import ram.ai.new.utilStates as utilStates
import ram.ai.new.stateMachine as stateMachine
import ram.ai.Utility as oldUtil
import ram.ai.new.searchPatterns as search
import ram.ai.new.state as state
import ram.ai.new.utilClasses as util
import downwardsVisualServoing as downVS


class Surrender(state.State):
    def __init__(self):
        super(Surrender,self).__init__()
        self.setTransition('next','end')

    def update(self):
        self.doTransition('next')




class Approach(search.SearchPattern):
    def __init__(self, stopConditions, servoingStateMachine,success, failure, retryConstraint = lambda : True, recoverAction = Surrender(), constraint = lambda : True):
        super(Approach,self).__init__(ServoingMachine(servoingStateMachine, retryConstraint, recoverAction), stopConditions, success, failure, constraint)

class ServoingMachine(stateMachine.StateMachine):
    def __init__(self, servoingStateMachine, retryConstraint, recoverAction):
        super(ServoingMachine,self).__init__()
        start = self.addState('start',utilStates.Start())
        end = self.addState('end',utilStates.End())
        #servo needs to be accesible, this is to allow for certain exit callbacks to get set if needed
        self._servo = self.addState('servo', utilStates.ConstrainedState(servoingStateMachine, retryConstraint, 'end',  'recover'))
        self._recover = self.addState('recover', recoverAction)
        #ensure appropriate transitions are set for recovery state
        self._recover.setTransition('success', 'servo')
        self._recover.setTransition('failure', 'end')
        #route start to the visual servoing
        start.setTransition('next', 'servo')


class VisualServoingStateMachine(stateMachine.StateMachine):
    def __init__(self, servoingState):
        super(VisualServoingStateMachine,self).__init__()
        start = self.addState('start',utilStates.Start())
        end = self.addState('end',utilStates.End())
        servo = self.addState('servo', servoingState)
        #set transition path
        servo.setTransition('next','end')
        start.setTransition('next','servo')
    

#does not use optional features: retry condition, recovery action and constraint
#Centers on an object in the view of the downwards camera
class DownCenter(Approach):
    def __init__(self, visionObject, success, failure, xBound = .1, yBound = .1, configNode = None):
        super(DownCenter, self).__init__(util.ObjectInVisionRangeQuery(visionObject, 0, 0, 0,xBound,yBound, 0).query, VisualServoingStateMachine(downVS.DownVisualServoing(visionObject, 0, 0, configNode)), success, failure)


#orients self with downwards object
class DownOrient(Approach):
    def __init__(self, visionObject, success, failure, angleBound = 1):
        super(DownOrient,self).__init__(lambda : (abs(visionObject.angle) < angleBound),VisualServoingStateMachine(downVS.YawVisualServoing(visionObject)),success,failure)
        
        
    
