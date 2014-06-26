import ram.ai.new.state as state
import ram.ai.new.stateMachine as stateMachine

import time

import ram.motion as motion

class testState1(state.State):
    def enter(self):
        print "hi\n"
        self.fireTransition('NEXT')

    def differentEnter(self):
        print "hi again\n"
        self.fireTransition('END')

class testState2(state.State):
    def enter(self):
        print "goodbye\n"
        self.fireTransition('DIFFERENT')

class testState3(state.State):
    def enter(self):
        print "at the end\n"
        self.getStateMachine().setComplete()

class TestMachine(stateMachine.StateMachine):
    def __init__(self):
        super(TestMachine, self).__init__()

        test1 = self.createState('test1', testState1)
        test2 = self.createState('test2', testState2)
        test3 = self.createState('test3', testState3)

        test1.setNextState('NEXT', test2)
        test1.setNextState('END', test3)
        test1.setEnterCallback('DIFFERENT', test1.differentEnter)

        test2.setNextState('DIFFERENT', test1)

        self.setStartState(test1)
