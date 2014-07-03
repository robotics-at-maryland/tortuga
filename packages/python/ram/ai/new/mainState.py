# new project imports as of June 2014
import ram.ai.new.stateMachine as stateMachine
import ram.ai.new.state as state
import ram.ai.new.utilStates as utilStates
import ram.ai.new.motionStates as motionStates
import ram.ai.new.testing as testMachine

#Old project imports 2013
import ram.ai.state as oldState
import ram.motion as motion
import ext.core as core
import ext.math as math
import ram.timer as timer

#std imports
import time

UPDATE = core.declareEventType('UPDATE')
TIMEOUT = core.declareEventType('TIMEOUT')
MACHINE_COMPLETE = core.declareEventType('MACHINE_COMPLETE')
TERMINATE = core.declareEventType('TERMINATE')
COMPLETE = core.declareEventType('COMPLETE')

class Body(oldState.State):
    """
    Just like the body of an essay I contain the meat of this routine
    you'll find within me a different stateMachine, *cue the inception horn*.
    This stateMachine will actually perform the whole of the course.
    As of now this version is an experimental version, but just like
    in the real world beta = full release right?
    For more information view ram/ai/new/stateMachine.py and 
    ram/ai/new/state.py
    """

    @staticmethod
    def transitions():
        return { MACHINE_COMPLETE : Body,
                 TIMEOUT : Body,
                 UPDATE : Body,
                 TERMINATE : Conclusion }

    def enter(self):
        self._updateDelay = 1
        self._courseTimeOut = 20
        """
        Since this will only be called on entry for the first time
        all initialization will be done here.
        """

        stateMachine.StateMachine._LEGACY_STATE = self
        self._machine = testMachine.TestMachine()
        self._machine.configure(self._config)
        self._machine.validate()
        self._machine.start()

        self._updateTimer = timer.Timer(self.timerManager, UPDATE, self._updateDelay, True)
        self._updateTimer.start()

        self._courseTimeOutTimer = timer.Timer(self.timerManager, UPDATE, self._updateDelay)
        self._courseTimeOutTimer.start()
        
    def exit(self):
        """
        Normally, clean up would be done here but in this case we're just
        leaving since clean up is done in our callbacks
        """
        self._updateTimer.stop()
        self._courseTimeOutTimer.stop()

    def UPDATE(self, event):
        self._machine.update()
        if self._machine.isCompleted():
            self.publish(MACHINE_COMPLETE, core.Event())

    def TIMEOUT(self, event):
        """ I'll do something later i.e cleanup """
        self.publish(TERMINATE, core.Event())

    def MACHINE_COMPLETE(self, event):
        """ I'll do something later i.e. cleanup """
        self.publish(TERMINATE, core.Event())

class Conclusion(oldState.State):
    """
    I'm the conclusion, mostly I'm just telling the stateMachine I'm in that
    this graph has reached an end state.
    """
    
    def enter(self):
        self.publish(COMPLETE, core.Event())
