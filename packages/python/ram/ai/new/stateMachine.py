import state
import ram.ai.state as oldState

import ext.core

class StateMachine(object):
    def __init__(self):
        self._legacyState = None
        self._startState = None
        self._currentState = None

        self._stateMap = {}

        self._currentTransition = None

        self._started = False
        self._completed = False

    def getState(self, stateName):
        '''Returns the state associated with the given state name.'''
        try:
            return self._stateMap.get[stateName]
        except KeyError:
            raise Exception('State "' + stateName + '" does not exist in the state machine.')

    def addState(self, state):
        '''Adds a state to the state map.'''

        self._stateMap[state.getName()] = state
        state.setStateMachine(self)
        return state

    def getLegacyState(self):
        '''Returns the legacy state for use with the old software system.'''
        return self._legacyState

    def setLegacyState(self, state):
        if not isinstance(state, oldState.State):
            raise Exception('setLegacyState(1) only takes in valid legacy states')
        self._legacyState = state

    def getStartState(self):
        '''Returns the state this state machine will start on.'''
        return self._startState

    def setStartState(self, state):
        """
        Set the start state, the stateMachine cannot start without doing this
        or having it set in the constructor
        """
        self._startState = state

    def getCurrentState(self):
        return self._currentState

    def setComplete(self):
        """
        Notify the stateMachine it has reached a valid termination state
        """
        if self._started is False:
            raise Exception('StateMachine cannot finish without starting.')
        self._completed = True

    def isStarted(self):
        return self._started

    def isCompleted(self):
        return self._completed
            
    def queueTransition(self, transitionName):
        '''Queues a transition to be handled after the current callback returns.'''
        self._currentTransition = transitionName

    def executeTransition(self):
        '''Checks to see if there is a queued transition, and executes it if so.'''
        while self._currentTransition is not None:
            transition = self._currentTransition
            self._currentState.doLeave(transition)
            self._currentState = self._currentState.getNextState(transition)
            self._currentTransition = None
            self._currentState.doEnter(transition)

    def configure(self, config):
        """
        Configure: implement in future version
        """
        pass

    def start(self):
        """
        Start the current stateMachine at the mentioned start State
        """
        self._currentState = self._startState
        self._currentState.doEnter(None)
        self._started = True # self._currentState.enter() needs to succeed
        self.executeTransition()

    def update(self):
        """
        The update hook, this gets called to update the stateMachine and the
        status of the active state
        """    
        self._currentState.update()
        self.executeTransition()
