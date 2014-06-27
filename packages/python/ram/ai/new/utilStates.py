import ram.ai.new.state as state
import ram.ai.new.stateMachine as stateMachine

class Start(state.State):
    '''
    Marks the start of a state machine.  
    Sets the state machine's start state to self upon init.
    '''
    def __init__(self):
        super(Start, self).__init__()

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
        self._innerMachine.setLegacyState(self.getStateMachine().getLegacyState())

    def getInnerStateMachine(self):
        return self._innerMachine

    def enter(self):
        self.getInnerStateMachine().start()

    def update(self):
        self.getInnerStateMachine().update()
