import ram.ai.new.state as state
import ram.ai.new.stateMachine as stateMachine

import time

import ram.motion as motion

class testState1(state.State):

    def __init__(self, machine):
        super(testState1, self).__init__(machine)
        self._transToStateID = {'NEXT' : 'test2', 'NEXT2' : 'test1'}
        self._enterCallBacks = {'NEXT2' : self.enterT}

    def defaultEnter(self):
        print "hi\n"
        time.sleep(2)
        self.setTransition('NEXT2')
        
    def enterT(self):
        print "snoooze\n"
        time.sleep(2)
        self.setTransition('NEXT')


class testState2(state.State):

    def __init__(self, machine):
        super(testState2, self).__init__(machine)
        self._transToStateID = {'NEXT' : 'test3'}

    def defaultEnter(self):
        print "goodbye\n"
        self.setTransition('NEXT')


class testState3(state.State):

    def __init__(self, machine):
        super(testState3, self).__init__(machine)
        self._transToStateID = {'NEXT' : 'test3', 'NEXT2' : 'test3'}
        self._enterCallBacks = {'NEXT' : self.enterT}

    def enterT(self):
        print "notyet\n"
        self.setTransition('NEXT2')

    def defaultEnter(self):
        print "ending\n"
        self._machine.setComplete()


