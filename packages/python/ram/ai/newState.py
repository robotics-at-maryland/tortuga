#STD imports
import weakref
import stateMachine
import traceback

class State(object):
    """
    """
    
    def __init__(self, name, machine, config=None, transMap={}, oldStateRef):
        if config is None:
            config = {}
        self._name = name
        self._machine = weakref.ref(machine)
        self._config = config
        self._transMap = transMap
        self._enterCallBacks = {}
        self._leaveCallBacks = {}
        self._transition = None
        ''' 
        Building the {transition 'str' : state 'state object'} 
        weakref Map 
        '''
        self._transStateMap = None
        self._buildStateMap() 
        self._CONFIGURED = False
        oldStateRef = kwargs['oldStateRef']
        if not isinstance(oldStateRef, oldState.State):
            raise Exception('[%s] requires instance of legacy state' % self._name)
        else:
            self._oldStateRef = oldStateRef 
        
    def __del__(self):
        self.cleanUp()

    def setTransMap(self, transMap):
        """
        Set the map of transitions to state names to the specified
        map prvided by the user
        """
        if transMap is None:
           transMap = {}
        self._transMap = transMap

    def getTransMap(self):
        """
        returns the map of transitions to state names for this state
        """
        return self._transMap

    def getTransition(self):
        """     
        """
        return self._transition

    def setTransition(self, transition):
        """
        """
        self._transition = transition

    def _buildStateMap(self):
        """
        builds a map of weak references to states this state
        can transition to
        """
        machineStateMap = self._machine.getStateMap()
        self._transStateMap = weakref.WeakValueDictionary()
        for trans, stateName in self._transMap.iteritems():
            state = machineStateMap[stateName]
            if state is not None:
                self._transStateMap.update({trans : state})
                
            
    def getStateMap(self):
        return self._transStateMap

    def enter(self, trans):
        """return: method object for callback, defaultEnter by default"""
        return self._enterCallbacks.get(trans, defaultEnter)

    def leave(self, trans):
        """
        return: method object for callback, defaultLeave by default
        NOTE: this state's current transition is reset in here, if the
        user wishes to maintain the transition they must store it another
        way or override this method
        """
        #I set the current transition back to None before leaving
        self._transition = None 
        return self._leaveCallBacks.get(trans, defaultLeave)
    
    def configure(self):
        """
        Configure the variables of the state
        Note: this overrides the __init__ variables
        """
        config = self._config.get(self._name, {})
        try:
            for key, value in config.iteritems():
                if hasattr(self, key):
                    setattr(self, key, value)
                else:
                    print("[%s] has no attribute %s"%(self._name, key))
            self._CONFIGURED=True
        except Exception, e:
            print("%s: %s\n" % (type(e), e))
            trackback.print_exc()
            self._CONFIGURED=False
        return self._CONFIGURED

    def configured(self):
        return self._CONFIGURED

    def isEnd(self):
        """
        Generally all states that inherit this state are not the End state
        """
        return False

    def update(self):
        if self._transition is None:
            pass
        else:
            self._machine.nextState(self._transition)

    def defaultEnter(self):
        """
        default enter callback
        """
        pass
    
    def defaultLeave(self):
        """
        default leave callback
        """
        pass

    def cleanUp():
        """
        Add clean up in here
        """
        pass


    def publish(self, eventType):
        self._oldStateRef.publish(eventType, core.Event())

def End(state):
    """
    Basic End state.
    """

    def __init__(self, name, machine, config=None, oldStateRef):
        super(End, self).__init__(name, machine, config, oldStateRef)

    def defaultEnter(self):
        """
        The current stateMachine has reached an End state
        """
        self._machine.setComplete()

    def isEnd(self):
        """
        This is a valid end state
        """
        return True
    


    
