import weakref
from types import *

import ext.core

from state import State

class StateMachine(object):
    _LEGACY_STATE = None

    def __init__(self):
        self._startState = None
        self._currentState = None
        self._currentTransition = None

        self._stateMap = {}

        self._started = False
        self._completed = False

    def hasState(self, stateName):
        assert type(stateName) is StringType
        return stateName in self._stateMap

    def getState(self, stateName):
        """Returns the state associated with the given state name."""
        assert type(stateName) is StringType
        assert self.hasState(stateName), \
            'State "%s" does not exist in the state machine.' % stateName

        return self._stateMap[stateName]

    def addState(self, stateName, state):
        """Adds a state to the state map."""
        assert type(stateName) is StringType
        assert isinstance(state, State)
        assert not self.hasState(stateName), \
            'State "%s" already exists in the state machine.' % stateName

        self._stateMap[stateName] = state
        state.added(stateName, self)
        return state

    def removeState(self, stateName):
        """Removes a state from the state map."""
        assert type(stateName) is StringType
        assert self.hasState(stateName), \
            'State "%s" does not exist in the state machine.' % stateName

        self._stateMap[stateName].removed()
        del self._stateMap[stateName]

    def getStates(self):
        return self._stateMap

    def addStates(self, states):
        """
        Adds all the states in a dictionary to this state machine.
        The dictionary should map from name to state.
        """
        for name, state in states.iteritems():
            self.addState(name, state)

        return self._stateMap

    def addTransition(self, state, transition, next):
        self.getState(state).setTransition(transition, next)
        return next

    def addTransitions(self, transitions):
        for state, transition, next in transitions:
            self.getState(state).setTransition(transition, next)

    def getStartState(self):
        """Returns the state this state machine will start on."""
        return self._startState

    def setStartState(self, state):
        """
        Set the start state, the stateMachine cannot start without doing this
        or having it set in the constructor
        """
        assert isinstance(state, State)
        self._startState = state

    def getCurrentState(self):
        return self._currentState

    def getLegacyState(self):
        """Returns the legacy state for use with the old software system."""
        return StateMachine._LEGACY_STATE

    def isStarted(self):
        return self._started

    def isCompleted(self):
        return self._completed
            
    def setComplete(self):
        """
        Notify the stateMachine it has reached a valid termination state
        """
        assert self.isStarted(), \
            'StateMachine cannot complete without starting.'
        
        self._completed = True

    def queueTransition(self, transitionName):
        """Queues a transition to be handled after the current callback 
        returns.
        """
        assert type(transitionName) is StringType
        self._currentTransition = transitionName

    def executeTransition(self):
        """Checks to see if there is a queued transition, and executes it if so."""
        while self._currentTransition is not None:
            transition = self._currentTransition
            self._currentState.doLeave(transition)
            self._currentState = self.getState(self._currentState.getTransition(transition))
            self._currentTransition = None
            self._currentState.doEnter(transition)

    def configure(self, config):
        pass

    def validate(self):
        for name, state in self._stateMap.iteritems():
            state.validate()

    def start(self):
        """
        Start the current stateMachine at the mentioned start State
        """
        assert self._startState is not None, \
            'StateMachine cannot start without a start state.'

        self._currentState = self._startState
        self._currentState.doEnter(None)
        self._started = True # self._currentState.enter() needs to succeed
        self._completed = False
        self.executeTransition()

    def update(self):
        """
        The update hook, this gets called to update the stateMachine and the
        status of the active state
        """    
        self._currentState.update()
        self.executeTransition()

    def stop(self):
        """
        stop the current stateMachine safely
        """
        self._currentState.doLeave(None)
        self._currentState = None
        self._completed = True
