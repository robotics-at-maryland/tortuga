import yaml

import ram.ai.new.state as state
import ram.ai.new.stateMachine as stateMachine

from ram.ai.new.motionStates import *

class Start(state.State):
    '''
    Marks the start of a state machine.  
    Sets the state machine's start state to self upon init.
    '''
    def __init__(self):
        super(Start, self).__init__()

    def addToStateMachine(self, name, machine):
        super(Start, self).addToStateMachine(name, machine)
        machine.setStartState(self)

    def enter(self):
        self.doTransition('next')

class End(state.State):
    '''
    Marks the end of a state machine.
    Sets the state machine to complete upon entry.
    '''
    def enter(self):
        self.getStateMachine().setComplete()

class NestedState(state.State):
    def __init__(self,internalMachine):
        super(NestedState, self).__init__()

        self._innerMachine = internalMachine

    def getInnerStateMachine(self):
        return self._innerMachine
        self.getInnerStateMachine().update()

    def addToStateMachine(self, name, machine):
        super(NestedState, self).addToStateMachine(name, machine)
        self._innerMachine.setLegacyState(machine.getLegacyState())

    def enter(self):
        self.getInnerStateMachine().start()

    def update(self):
        self.getInnerStateMachine().update()

class YAMLState(NestedState):
    def __init__(self, filename = None, states = None):
        super(YAMLState, self).__init__()
        
        if filename is not None:
            states = yaml.load(open(filename, 'r'))

        for name, config in states.iteritems():
            typ = state.MetaState.getStateType(config['type'])
            init = config.get('init', {})
            transitions = config.get('transitions', {})
            newState = typ(**init)
            self.getInnerStateMachine().addState(name, newState)
            for k, v in transitions.iteritems():
                newState.setNextState(v, k)

#internal statem machine, constraint(boolean returning function, true if constraint is being followed), state to transition to if inner machine finishes(success case), state to transition to if constraint is violated(failure case)
#Note that state will always go to success if failure is not set
class ConstrainedState(NestedState):
    def __init__(self, internalMachine, queryFunc, success, failure = None):
        super(ConstrainedState, self).__init(internalMachine)
        self._query = queryFunc
        self.setNextState(success,'success')
        if(failure == None):
            self.setNextState(success,'failure')
        else:
            self.setNextState(failure,'failure')

    #note that the constrain is prioritized over the state machine completing
    def update(self):
        if(self._query()):
            self.getInnerStateMachine().update()
        else:
            self.doTransition('failure')
        if(self.getInnerStateMachine.isComplete):
            self.doTransition('success')

#the following state is a simple counter, it merely goes to the next state instantly
#this can be used for counting the number of passes through a certain transition path

class PassCounter(state.State):
    def __init__(self,destination):
        super(PassCounter,self).__init__()
        self._count = 0
        self._setNextState(destination):
   
    def update(self):
        self._count = self._count + 1
        self.doTransition('next')

    def getPasses(self):
        return self._count
        
