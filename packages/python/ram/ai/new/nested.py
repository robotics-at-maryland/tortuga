import weakref

import state
import stateMachine

class NestingState(state.State):
    """
    The NestingState is a State that contains a NestedStateMachine
    The NestingState is meant to run the StateMachine, the StateMachine
    must be set to a valid stateMachine before starting.

    The user must override defaultEnter and defaultLeave or add callbacks
    to choose where they would want the Machine to actually start, however
    the state will fail to enter if the StateMachine is None

    Transition becomes 'MACHINE_COMPLETE' when the update hook realizes
    the nested machine has reached an appropriate End condition

    The machine is not reset upon leaving. Multiple entries will result
    in a possibly changed StateMachine

    Any local cleanup must be performed before leaving the state.
    """

    def __init__(self, machine, nestedMachine=None):
        """
        """
        super(NestingState, self).__init__(machine)
        if isinstance(nestedMachine, NestedStateMachine):
            self._nestedMachine = nestedMachine
        else:
            self._nestedMachine = nestedMachine

    def enter(self):
        if self._nestedMachine is None:
            raise Exception('NestingState must have a valid StateMachie')
        return super(NestingState, self).enter(transition)

    def update(self):
        if self._nestedMachine.completed():
            self._transition = 'MACHINE_COMPLETE'
        else:
            self._nestedMachine.update()
        

class NestedStateMachine(stateMachine.StateMachine):
    """
    """

    def __init__(self, nestingState=None):
        super(NestedStateMachine, self).__init__()
        if nestingState is None or not isinstance(nestingState, NestingState):
            raise Exception('The nestingState must be valid')
        self._nestingState = weakref.proxy(nestingState)
        
    def publish(self, event):
        self._nestingState.publish(event)
