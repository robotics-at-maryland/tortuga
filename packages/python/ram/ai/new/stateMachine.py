import state
import ram.ai.state as oldState

import ext.core

class StateMachine(object):
    """
    """

    def __init__(self):
        """
        """

        self._startState = None
        self._currentState = None
        self._stateMap = {}
        self._prevTransition = None
        self._currentTransition = None
        self._started = False
        self._completed = False
        self._legacyState = None

    def getState(self, stateID):
        return self._stateMap.get(stateID)

    def setLegacyState(self, state):
        if not isinstance(state, oldState.State):
            raise Exception('setLegacyState(1) only takes in valid legacy states')
        self._legacyState = state

    def setStartState(self, stateID):
        """
        Set the start state, the stateMachine cannot start without doing this
        or having it set in the constructor
        """
        if self._stateMap.get(stateID) is None:
            raise Exception(('[%s]: The stateMap does not ' % type(self)) + \
                            ('contain the stateID'))
        else:
            self._startState = self._stateMap[stateID]

    def getCurrentState(self):
        return self._currentState

    def getStartState(self):
        return self._startState

    def getLegacyState(self):
        return self._legacyState

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
        self._currentState.enter(None)()
        self._started = True # self._currentState.enter() needs to succeed

    def update(self):
        """
        The update hook, this gets called to update the stateMachine and the
        status of the active state
        """
        
        self._currentTransition = self._currentState.getTransition()
        if self._currentTransition is None:
            self._currentState.update()
        else:
            self.toNextState()

    def toNextState(self):
        """
        Method to transition this StateMachine to the next state as dictated
        by the currentTransition
        """
        if self._completed:
            raise Exception('Transitioning out of a valid end state is' + \
                            'only allowed if the end state \ndoes not' + \
                            'set the parent StateMachine as completed.' + \
                            'check your end state transitions')
        nextStateID = self._currentState.getNextState(self._currentTransition)
        if nextStateID is None:
            raise Exception('Transition %s is not supported' % self._currentTransition)
        nextState = self._stateMap.get(nextStateID)
        self._currentState.leave(self._currentTransition)()
        self._currentState = nextState
        self._prevTransition = self._currentTransition
        self._currentTransition = None
        self._currentState.enter(self._prevTransition)()
        
    def setComplete(self):
        """
        Notify the stateMachine it has reached a valid termination state
        """
        if self._started is False:
            raise Exception('StateMachine cannot finish without starting')
        self._completed = True

    def started(self):
        return self._started

    def completed(self):
        return self._completed

    def publish(self, event):
        if self._legacyState is None:
            raise Exception('Machine is unable to publish without legacy state')
        self._legacyState.publish(event, core.Event())
            
