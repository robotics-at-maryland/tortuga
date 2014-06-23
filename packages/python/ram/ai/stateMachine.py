#STD imports
import inspect
import types

#project imports
import ram.ai.state as oldState
import ram.ai.newState as state
import ext.core as core

class stateMachine(object):
    """
    The stateMachine class implements a finite state-machine:
        The current version of the state machine is 'event-driven'
        in reality it is function calls
    """

    STATE_ENTERED = core.declareEventType('STATE_ENTERED')
    STATE_EXITED = core.declareEventType('STATE_EXITED')
    COMPLETE = core.declareEventType('COMPLETE')
    
    machineCounter = 0 #used for unique generic name

    def __init__(self, config=None, name=None, **kwargs):
        """
        """
        machineCounter+=1
        #configNode, passing in config values.
        if config is None:
            config = {}
        if name is None:
            name = "machine%d"%machineCounter
        self._name = name
        self._config = config
        self._stateMap = {'defaultEnd': state.End('defaultEnd', self, oldStateRef)}
        self._stateMap.update(kwargs.get('stateMap',{}))
        if kwargs['startState'] is None:
            startState = 'defaultEnd'
        else:
            startState = kwargs['startState']
        self._startState = self._stateMap['startState']
        self._currentState = self._startState
        self._started = False
        self._finished = False    
        self._prevTrans = None
        #subsystem reference
        if subsystem is not None:
            subsystem = weakref.ref(subsystem)
        self._CONFIGURED = False
        oldStateRef = kwargs['oldStateRef']
        if not isinstance(oldStateRef, oldState.State):
            raise Exception('[%s] requires instance of legacy state' % self._name)
        else:
            self._oldStateRef = oldStateRef

                
    def __del__(self):
        """
        Make sure to clean up all states on destruction
        """
        self.cleanUp()


    def setStartState(self, startState):
        if self._started or self._finished:
            raise Exception('Machine has started or has ended, cannot set start')
        else:
            self._startState = startState
            self._currentState = startState


    def startState(self):
        return self._startState


    def setStateMap(self, stateMap):
        if stateMap is None:
            stateMap = {}
        stateMap.update({'defaultEnd' : state.End('defaultEnd', self)})
        self._stateMap = stateMap


    def getStateMap(self):
        return self._stateMap


    def configure(self):
        """
        """
        config = self._config.get(self._name, {})
        attributes = config.get('attributes', {})
        stateConfig = config.get('states', {})
        try:
            for key, value in attributes:
                setattr(self, key, value)
            for s in stateMap:
                s.configure(self._config.get(s._name, {}))
            self._CONFIGURED=True
        except Exception, e:
            self._CONFIGURED=False
            trackback.print_exc()
        return self._CONFIGURED


    def configured(self):
        """
        True if configure has been run, False otherwise
        """
        return self._CONFIGURED


    def cleanUp(self):
        """
        Makes sure that the current state is exited properly if state machine
        is destroyed
        """
        self._currentState.cleanUp()


    def currentState(self):
        """
        Returns a reference to the stateMachine's current state

        return: self._currentState
        """
        return self._currentState


    def start(self):
        """
        This will start the state machine

        raises: InvalidStateException
        """
        if not self._CONFIGURED:
            raise Exception("The machine has not been configured.")
        print("Starting state machine %s\n" % self._name)
        self._started = True
        if self._currentState is None:
            self._started = False
            raise Exception("StateMachine" + self._name + "failed to start." +\
                            "The current state is None\n")
        try:
            """TODO: add timer functionality later"""
            self._currentState.enter()()
        except Exception as e:
            strout = "Failed at start (Machine %s): \n" % self._name
            strout += str(type(e)) + ": " + str(e.value) + "\n"
            self._started = False
            traceback.print_exc()


    def update(self):
        """
        This is called to update the state machine.
        """
        if self.isComplete():
            self.publish(self._oldStateRef.MACHINE_COMPLETE)
        else:    
            self._currentState.update()

        
    def nextState(self, trans):
        self._prevTrans = trans
        tempState = self._currentState.getNextState(trans)
        self._currentState.leave(trans)
        self._currentState = tempState


    def setComplete(self):
        self._finished = True

            
    def isComplete(self):
        """
        Will only return True if the state machine has both started
        successfully and reached an End state successfully
        """
        return self._finished is True and self._started is True


    def publish(self, eventType):
        self._oldStateRef.publish(eventType, core.Event())

        

 
