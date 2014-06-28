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
    def __init__(self):
        super(NestedState, self).__init__()

        self._innerMachine = stateMachine.StateMachine()

    def getInnerStateMachine(self):
        return self._innerMachine

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
