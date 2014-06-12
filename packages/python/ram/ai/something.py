import math as m
import ext.control as control
import ram.ai.state as state
import ram.motion as motion
import ext.core as core
import ext.vision as vision
import ext.math as math
import ram.ai.Utility as util
from ram.motion.basic import Frame

DONE = core.declareEventType('DONE')

class TestState(state.State):

    EXIT = core.declareEventType('EXIT')

    @staticmethod
    def transitions():
        return { DONE : TestState, TestState.EXIT : SecondState }

    def DONE(self, event):
        print 'I did something'
        self.publish(TestState.EXIT, core.Event())

    def enter(self):
        self.publish(DONE, core.Event())

    def exit(self):
        print ''
        
class SecondState(state.State):

    @staticmethod
    def transitions():
        return { DONE : state.State }

    def enter(self):
        print 'hello'
        self.publish( DONE, core.Event())


    def exit(self):
        print 'goodbye'
