import inspect

class State(object):
    def __init__(self, machine):
        self._machine = machine

    @staticmethod
    def transitions():
        pass

    def enter(self):
        pass

    def exit(self):
        pass

class Machine(object):
    def __init__(self, startState):
        self._root = startState
        self._currentState = startState(self)

    def currentState(self):
        return self._currentState

    def start(self):
        self._currentState.enter()
        
    def getTransitionFunc(self, etype, obj):
        # Grab all member functions
        members = inspect.getmembers(obj, inspect.ismethod)

        # See if we have a matching method
        matches = [func for name,func in members if name == etype]

        # We found it
        assert len(matches) < 2
        if len(matches) > 0:
            return matches[0]

    def injectEvent(self, etype, sender, args):
        nextState = type(self._currentState).transitions().get(etype, None)

        loopback = False
        if nextState == type(self._currentState):
            loopback = True

        if nextState is not None:
            # We are leaving the current state
            if not loopback:
                self._currentState.exit()
            
            # Call the function for the transitions
            transFunc = self.getTransitionFunc(etype, self._currentState)
            if transFunc is not None:
                transFunc(sender, args)

            # Notify that we are entering the next state
            if not loopback:
                # Create an instance of the next state's class
                self._currentState = nextState(self)

                self._currentState.enter()
