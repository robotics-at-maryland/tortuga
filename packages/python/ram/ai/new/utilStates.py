import yaml

from state import *
from stateMachine import *
from motionStates import *

import ram.ai.new.utilClasses as utilClasses

@require_transitions('next')
class Start(State):
    """
    Marks the start of a state machine.  
    Sets the state machine's start state to self upon init.
    """
    def added(self, name, machine):
        super(Start, self).added(name, machine)
        machine.setStartState(self)

    def update(self):
        print 'start'
        self.doTransition('next')

class End(State):
    """
    Marks the end of a state machine.
    Sets the state machine to complete upon entry.
    """
    def enter(self):
        self.getStateMachine().setComplete()

class Failure(End):
    """
    User needs to check to see if the end state was a failure
    to use this
    """
    def enter(self):
        self.getStateMachine().setComplete()

class NestedState(State):
    def __init__(self, innerMachine = StateMachine()):
        super(NestedState, self).__init__()
        assert isinstance(innerMachine, StateMachine)

        self._innerMachine = innerMachine

    def getInnerStateMachine(self):
        return self._innerMachine

    def configure(self, config):
        self.getInnerStateMachine().configure(config)

    def validate(self):
        super(NestedState, self).validate()
        self.getInnerStateMachine().validate()

    def enter(self):
        self.getInnerStateMachine().start()
        if self.getInnerStateMachine().isCompleted():
            self.doTransition('complete')

    def update(self):
        self.getInnerStateMachine().update()
        if self.getInnerStateMachine().isCompleted():
            self.doTransition('complete')

    def exit(self):
        if not self.getInnerStateMachine().isCompleted():
            self.getInnerStateMachine().stop()

#internal statem machine, constraint(boolean returning function, true if constraint is being followed), state to transition to if inner machine finishes(success case), state to transition to if constraint is violated(failure case)
#Note that state will always go to success if failure is not set
class ConstrainedState(NestedState):
    def __init__(self, internalMachine, queryFunc, success, failure = None):
        super(ConstrainedState, self).__init__(internalMachine)
        self._query = queryFunc
        self.setTransition('complete', success)
        if(failure is None):
            self.setTransition('failure', success)
        else:
            self.setTransition('failure', failure)

    #note that the constrain is prioritized over the state machine completing
    def update(self):
        if(self._query()):
            self.getInnerStateMachine().update()
        else:
            self.doTransition('failure')
        if(self.getInnerStateMachine().isCompleted()):
            self.doTransition('complete')

#the following state is a simple counter, it merely goes to the next state instantly
#this can be used for counting the number of passes through a certain transition path

class PassCounter(State):
    def __init__(self, destination):
        super(PassCounter,self).__init__()
        self._count = 0
        self.setTransition('next', destination)
   
    def update(self):
        self._count = self._count + 1
        self.doTransition('next')

    def getPasses(self):
        return self._count

    def reset(self):
        self._count = 0
    
    #this function returns a function that checks the check you pass in
    #aren't closures wonderful?
    #returns true if under or equal to the count and false if over it
    def getPassChecker(self,countToCheck):
        def checkPasses():
            return (self._count > countToCheck)
        return checkPasses

#this class will transition to first while check is false, if check is true it goes to second
class Switch(State):
    def __init__(self, first, second, check):
        super(Switch, self).__init__()
        self.setTransition('next', first)
        self.setTransition('switch', second)
        self.check = check
    
    def update(self):
        if self.check():
            self.doTransition('switch')
        else:
            self.doTransition('next')

class Task(ConstrainedState):
    def __init__(self, internalMachine, success, failure = 'end', 
                 timerDuration = None, maxDistance = None):
        self._taskTimer = None
        if timerDuration is not None:
            self._taskTimer = utilClasses.Timer(timerDuration)
        self._startPos = None
        self._maxDist = maxDistance
        super(Task, self).__init__(internalMachine, self.constrain,
                                   success, failure)

    def enter(self):
        self._taskTimer.reset()
        self._startPos = self.getStateMachine().getLegacyState().stateEstimator.getEstimatedPosition()
        super(Task, self).enter()

    def constrain(self):
        if self._taskTime is not None and self._taskTimer.check():
            return True
        currPos = self.getStateMachine().getLegacyState().stateEstimator.getEstimatedPosition()
        if self._maxDist is not None and currPos.distance(self._startPos) > self._maxDist:
            return True
        return False
