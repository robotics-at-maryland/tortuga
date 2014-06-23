
#STD imports
import inspect
import types

#project imports
import newState as state
import stateMachine

class NestedState(state.State):
    """
    """

    def __init__(self, name, machine, config=None, transMap={}, oldStateRef
                 nestedMachine=None):
        super(NestedState, self).__init__(name, machine, config, transMap, oldStateRef)
        if isinstance(nestedMachine, stateMachine.StateMachine):
            self._nestedMachine = weakref.ref(NestedStateMachine.__init__(nestedMachine, self))
        else:
            raise Exception('[%s] requires a stateMachine to be nested' % self._name)

    def update(self):
        self._nestedMachine.update()

    def cleanUp(self):
        self._nestedMachine.cleanUp()
        self._nestedMachine = None

    def setMachine(self, machine):
        if machine is None:
            raise Exception("[%s] Machine cannot be none" % self._name)
        elif isinstance(machine, stateMachine.stateMachine):
            self._nestedMachine = weakref.ref(NestedStateMachine.__init__(machine, self))
        else:
            reaise Exception("[%s] Machine must be instance of a StateMachine" % self._name)
        


class NestedStateMachine(stateMachine.StateMachine):
    """
    """

    def __init__(self, config=None, name=None, nestingState=None
                 **kwargs):
        super(NestedStateMachine, self).__init__(config, name,
                                                 oldStateRef=kwargs['oldStateRef']
                                                 kwargs)
        self._nestingState = nestingState


    def __init__(self, machine, nestingState):
        super(NestedStateMachine, self).__init__(machine._config, machine._name,
                                                 startState = machine._startState,
                                                 stateMap = machine._stateMap,
                                                 oldStateRef = nestingState._oldStateRef)
        self._nestingState = nestingState


    def configure(self):
        try:
            self._CONFIGURED = super(NestedStateMachine, self).configure()
            if self._nestingState is None:
                self._CONFIGURE = False
                raise Exception("The nesting state cannot be None")
        except Exception as e:
            self._CONFIGURE = False
            traceback.print_exc()
        return self._CONFIGURED

    
        
