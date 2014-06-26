import weakref

import stateMachine

class State(object):
    def __init__(self, name, machine):
        self._name = name
        self._machine = weakref.ref(machine)

        self._transitions = {}      # Map from transition name to weakref of next state
        self._enterCallbacks = {}   # Map from transition name to callback function
        self._leaveCallbacks = {}   # Map from transition name to callback function
        self._eventCallbacks = {}   # Map from event name to callback function

        self.initialize()

    def getName(self):
        '''Returns the name of this state.'''
        return self._name

    def getStateMachine(self):
        '''Returns the state machine for this state.'''
        return self._machine()

    def getNextState(self, transitionName):
        '''Returns the next state for the given transition.'''
        try:
            return self._transitions[transitionName]()
        except KeyError:
            raise Exception('Transition "' + transitionName + '" in "' + self._name + '" does not exist.')

    def setNextState(self, transitionName, nextState):
        '''Sets the next state for the given transition.'''
        self._transitions[transitionName] = weakref.ref(nextState)

    def getEnterCallback(self, transitionName):
        '''Returns the enter callback for the given transition.'''
        return self._enterCallbacks.get(transitionName, self.enter)

    def setEnterCallback(self, transitionName, callback):
        '''Sets the enter callback for the given transition.'''
        self._enterCallbacks[transitionName] = callback

    def getLeaveCallback(self, transitionName):
        '''Returns the leave callback for the given transition.'''
        return self._leaveCallbacks.get(transitionName, self.leave)

    def setLeaveCallback(self, transitionName, callback):
        '''Sets the leave callback for the given transition.'''
        self._leaveCallbacks[transitionName] = callback

    def getEventCallback(self, eventName):
        '''Returns the event callback for the given event name.'''
        try:
            return self._eventCallbacks[eventName]
        except KeyError:
            raise Exception('Event callback for "' + eventName + '" in "' + self._name + '" does not exist.')

    def setEventCallback(self, eventName, callback):
        '''Sets the event callback for the given event name.'''
        self._eventCallbacks[eventName] = callback

    ## These methods should be called by the state machine
    def doEnter(self, transitionName):
        '''Signals the state to perform its enter callback.'''
        self.getEnterCallback(transitionName)()

    def doLeave(self, transitionName):
        '''Signals the state to perform its leave callback.'''
        self.getLeaveCallback(transitionName)()

    def doEvent(self, eventName, event):
        '''Signals the state that an event has been fired.'''
        cb = self._eventCallbacks.get(eventName, None)
        if cb is not None:
            cb(event)

    def fireTransition(self, transitionName):
        '''Signals the state machine to execute the given transition.'''
        self.getStateMachine().queueTransition(transitionName)

    def fireEvent(self, event):
        '''Signals the state machine to fire an event.'''
        self.getStateMachine().fireEvent(event)

    def initialize(self):
        pass

    def configure(self, *args, **kwargs):
        pass

    def enter(self):
        pass

    def leave(self):
        pass

    def update(self):
        pass

class End(State):

    def enter(self):
        """
        The default entry sets the parent machine to completed.
        You are not allowed to transition out once this is done.
        If you wish to transition out, or perform callbacks you must modify
        the callbacks and methods accordingly.
        """
        self.getStateMachine().setComplete()
