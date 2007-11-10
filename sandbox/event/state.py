
# STD Imports
import inspect

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
    
    def __init__(self, startState):
        self._root = startState
        self._currentState = startState(self)

    def currentState(self):
        return self._currentState

    def start(self):
        self._currentState.enter()

    def injectEvent(self, etype, sender = None, args = None):
        nextState = type(self._currentState).transitions().get(etype, None)

        # For loops backs we don't reenter, or exit from our state, just call
        # the transition function
        loopback = False
        if nextState == type(self._currentState):
            loopback = True

        if nextState is not None:
            # We are leaving the current state
            if not loopback:
                self._currentState.exit()
#            event.publish(Machine.STATE_EXITED, self,
#                          {'state' : self._currentState })

            
            # Call the function for the transitions
            transFunc = self._getTransitionFunc(etype, self._currentState)
            if transFunc is not None:
                transFunc(sender, args)

            # Notify that we are entering the next state
            if not loopback:
                # Create an instance of the next state's class
                self._currentState = nextState(self)
                self._currentState.enter()
#                event.publish(Machine.STATE_ENTERED, self,
#                              {'state' : self._currentState })

    def _getTransitionFunc(self, etype, obj):
        # Grab all member functions
        members = inspect.getmembers(obj, inspect.ismethod)

        # See if we have a matching method
        matches = [func for name,func in members if name == etype]

        # We found it
        assert len(matches) < 2
        if len(matches) > 0:
            return matches[0]
