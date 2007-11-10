
# STD Imports
import inspect
import types

# Project Imports
import event

class State(object):
    """
    Basic state class, its provides empty implementation for all the needed
    methods of a state
    """
    def __init__(self, machine):
        self.machine = machine

    @staticmethod
    def transitions():
        """
        Returns a map of eventTypes -> resulting states, loopbacks are allowed
        """
        pass

    def enter(self):
        """
        Called when the state is entered, loopbacks don't count
        """
        pass

    def exit(self):
        """
        Called when the state is exited, loopbacks don't count
        """
        pass

class Machine(object):
    STATE_ENTERED = 'StateEntered'
    STATE_EXITED = 'StateExited'

    @staticmethod
    def _getSystem(stype, subsystems):
        for sys in subsystems:
            if stype == type(sys):
                return sys
        return None
    
    def __init__(self, cfg = None, subsystems = None):
        if subsystems is None:
            subsystems = []
        
        self._root = None
        self._currentState = None
        self._started = False

        # Get the event subsystem
        self.eventMgr = Machine._getSystem(event.EventManager, subsystems)

    def currentState(self):
        return self._currentState

    def start(self, startState):
        self._root = startState
        self._started = True

        self._enterState(startState(self))

    def injectEvent(self, etype, sender = None, args = None):
        if not self._started:
            raise Exception("Machine must be started")
        
        nextState = type(self._currentState).transitions().get(etype, None)

        if nextState is not None:
            # For loops backs we don't reenter, or exit from our state, just
            # call the transition function
            loopback = False
            if nextState == type(self._currentState):
                loopback = True
            
            # We are leaving the current state
            if not loopback:
                self._currentState.exit()
                self._publishEvent(Machine.STATE_EXITED,
                                   state = self._currentState)
            
            # Call the function for the transitions
            transFunc = self._getTransitionFunc(etype, self._currentState)
            if transFunc is not None:
                transFunc(sender, args)

            # Notify that we are entering the next state
            if not loopback:
                # Create an instance of the next state's class
                self._enterState(nextState(self))

    def _enterState(self, newState):
        self._currentState = newState
        self._currentState.enter()
        self._publishEvent(Machine.STATE_ENTERED, state = self._currentState)

    def _getTransitionFunc(self, etype, obj):
        # Grab all member functions
        members = inspect.getmembers(obj, inspect.ismethod)

        # See if we have a matching method
        matches = [func for name,func in members if name == etype]

        # We found it
        assert len(matches) < 2
        if len(matches) > 0:
            return matches[0]

    def _publishEvent(self, etype, **args):
        if self.eventMgr is not None:
            self.eventMgr.publish(etype, self, **args)
