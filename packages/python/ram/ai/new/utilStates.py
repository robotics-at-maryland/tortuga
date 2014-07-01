import yaml

from state import *
from stateMachine import *
from motionStates import *

@require_transitions('next')
class Start(State):
    """
    Marks the start of a state machine.  
    Sets the state machine's start state to self upon init.
    """
    def added(self, name, machine):
        super(Start, self).added(name, machine)
        machine.setStartState(self)

    def enter(self):
        self.doTransition('next')

class End(State):
    """
    Marks the end of a state machine.
    Sets the state machine to complete upon entry.
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
        print self._query()
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
        self.setTransition(destination)
   
    def update(self):
        self._count = self._count + 1
        self.doTransition('next')

    def getPasses(self):
        return self._count
    
    #this function returns a function that checks the check you pass in
    #aren't closures wonderful?
    #returns true if under the count and false if over it
    def getPassChecker(self,countToCheck):
        def checkPasses(self):
            return (self._count > countToCheck)
        return checkPasses
