import weakref

import stateMachine


class State(object):
    """
    """

    def __init__(self, machine):
        if not isinstance(machine, stateMachine.StateMachine):
            raise Exception(('[%s]: machine must be of type ' % type(self)) + \
                            ('StateMachine from ram/ai/new/stateMachine.py'))
        self._machine = weakref.proxy(machine) # weak reference to parent machine
        
        self._enterCallBacks = {}   # map of transition : methods for callbacks
                                    # on entry
                                    
        self._leaveCallBacks = {}   # map of transition : methods for callbacks
                                    # on leaving

        self._transToStateID = {}   # map of transition : stateIDs (as they're
                                    # stored in the parent machine). Used to
                                    # generate the transToStateMap
                                                                        
        self._transToStateMap = {}  # map of transition : weakrefs of states
                                    # generated using map from parent machine
                                    # used to construct full state map
                                    
        self._transition = None     # current transition
        self._generateTransToStateMap()

    def _generateTransToStateMap(self):
        """
        Generates the transToStateMap of transitions to state references
        Transition Validation (verifying that the transition goal state is
        feasible / in existance in the parent machine)
        """
        for key, value in self._transToStateID.iteritems():
            state = self._machine.getState(value)
            if state is not None:
                self._transToStateMap.update({key : weakref.proxy(state)})
            else:
                raise Exception(('[%s]: transition goal ' % type(self)) + \
                                ('must be a valid state in the parent machine'))

    def configure(self, config):
        """
        Configure the state, configuration will be added later
        """
        pass

    def getStateMap(self):
        """
        returns the mapping of transitions to state references
        """
        return self._transToStateMap

    def getTransMap(self):
        """
        returns the mapping of transitions to stateIDs
        """
        return self._transToStateID

    def setTransition(self, transition):
        """
        Sets this State's current transition
        Transitions are currently strings. No checks are done in this method
        """
        self._transition = transition

    def getTransition(self):
        """
        Returns this State's current transition
        """
        return self._transition

    def getNextState(self, transition):
        """
        retrieves the next stateID this state will transition to
        according to self._transToStateID

        supports artificially injected transitions
        """
        return self._transToStateID.get(transition)

    def publish(self, event):
        """
        Uses the State's parent machine to publish an event
        """
        self._machine.publish(event)

    def enter(self, transition=None):
        """
        Enters state on a transition which will be used to pull a callback,
        if applicable, from the enterCallback map

        defaultEnter by default
        """
        return self._enterCallBacks.get(transition, self.defaultEnter)

    def leave(self, transition=None):
        """
        Leaves the state on a transition which will be used to pull a callback,
        if applicable, from the leaveCallback map
        """
        self._transition = None
        return self._leaveCallBacks.get(transition, self.defaultLeave)

    def update(self):
        """
        Update call: This updates the current state, various cases can be
        handled here, but mostly transition checks and such are done here
        """
        raise Exception(('[%s]: update method needs to be ' % type(self)) + \
                        ('overridden to prevent endless update scenario.\n'))

    def defaultEnter(self):
        """
        Default entry method called via enter
        Override to add additional functionality, or add callbacks
        """
        pass

    def defaultLeave(self):
        """
        Default leave method called via leave
        Override to add additional functionality, or add callbacks
        """
        pass


class End(State):
    """
    """

    def __init__(self, machine):
        super(End, self).__init__(machine)


    def defaultEnter(self):
        """
        The default entry sets the parent machine to completed.
        You are not allowed to transition out once this is done.
        If you wish to transition out, or perform callbacks you must modify
        the callbacks and methods accordingly.
        """
        self._machine.setComplete()
