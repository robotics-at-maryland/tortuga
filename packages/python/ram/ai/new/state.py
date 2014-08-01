import weakref

from types import *

class MetaState(type):
    _STATES = {}

    def __init__(self, name, bases, dict):
        #Add the state type to the global state map
        MetaState._STATES[name] = self

        self._REQ_TRANSITIONS = set()
        #Inherit required transitions from base classes
        for base in bases:
            if hasattr(base, '_REQ_TRANSITIONS'):
                self._REQ_TRANSITIONS |= base._REQ_TRANSITIONS

    @staticmethod
    def getStateType(name):
        """Returns the state type associated with the given name."""
        return MetaState._STATES[name]

    @staticmethod
    def getStateTypes():
        """Returns the map of state names to their state types."""
        return MetaState._STATES

def require_transitions(*args):
    def decorator(cls):
        cls._REQ_TRANSITIONS |= set(args)
        return cls
    return decorator

##################################################

class State(object):
    __metaclass__ = MetaState

    def __init__(self):
        self._name = None
        self._machine = None

        self._transitions = {}      # Map from transition name to weakref of next state
        self._enterCallbacks = {}   # Map from transition name to callback function
        self._leaveCallbacks = {}   # Map from transition name to callback function

    def getName(self):
        """Returns the name of this state."""
        return self._name

    def getState(self, stateName):
        """Returns the state in the parent state machine associated with the given name."""
        assert type(stateName) is StringType
        self.getStateMachine().getState(stateName)

    def getStateMachine(self):
        """Returns the state machine for this state."""
        return self._machine()

    def getRequiredTransitions(self):
        """Returns a set of transition names that must be fulfilled for the state to run."""
        return type(self)._REQ_TRANSITIONS

    def hasTransition(self, transitionName):
        """Returns if the state has the given transition or not."""
        assert type(transitionName) is StringType
        return transitionName in self._transitions

    def getTransition(self, transitionName):
        """Returns the next state for the given transition."""
        assert type(transitionName) is StringType
        assert self.hasTransition(transitionName), \
            'Transition "%s" in "%s" does not exist.' % (transitionName, self._name)
        return self._transitions[transitionName]

    def setTransition(self, transitionName, stateName):
        """Sets the next state for the given transition."""
        assert type(transitionName) is StringType
        assert type(stateName) is StringType
        self._transitions[transitionName] = stateName

    def getEnterCallback(self, transitionName):
        """Returns the enter callback for the given transition."""
        return self._enterCallbacks.get(transitionName, self.enter)

    def setEnterCallback(self, transitionName, callback):
        """Sets the enter callback for the given transition."""
        def callBFun():
            super(self.__class__,self).enter()
            self.enter()
            callback()
        self._enterCallbacks[transitionName] = callBFun

    def getLeaveCallback(self, transitionName):
        """Returns the leave callback for the given transition."""
        return self._leaveCallbacks.get(transitionName, self.leave)

    def setLeaveCallback(self, transitionName, callback):
        """Sets the leave callback for the given transition."""
        def callBFun():
            super(self.__class__,self).leave()
            self.leave()
            callback()
        self._leaveCallbacks[transitionName] = callBFun

    def doTransition(self, transitionName):
        """Signals the state machine to execute the given transition."""
        assert self.hasTransition(transitionName)
        self.getStateMachine().queueTransition(transitionName)

    def doEnter(self, transitionName):
        """Signals the state to perform its enter callback."""
        #print("Entering State " + self._name)
        self.getEnterCallback(transitionName)()

    def doLeave(self, transitionName):
        """Signals the state to perform its leave callback."""
        #print("Leaving State " + self._name)
        self.getLeaveCallback(transitionName)()

    def added(self, name, machine):
        self._name = name
        self._machine = weakref.ref(machine)

    def removed(self):
        self._name = None
        self._machine = None

    def configure(self, config):
        pass

    def validate(self):
        for transitionName in self.getRequiredTransitions():
            assert transitionName in self._transitions, \
                'Transition "%s" in "%s" was not set up.' % (transitionName, self._name)
        
        for transitionName, stateName in self._transitions.iteritems():
            assert self.getStateMachine().hasState(stateName), \
                'Transition "%s" in "%s" leads to "%s", which does not exist.' % (transitionName, self._name, stateName)

    def enter(self):
        pass

    def update(self):
        pass

    def leave(self):
        pass
