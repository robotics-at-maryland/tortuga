# new project imports as of June 2014
import ram.ai.new.stateMachine as StateMachine
import ram.ai.new.state as state
import ram.ai.new.testMachine as testMachine

#Old project imports 2013
import ram.ai.state as oldState
import ext.core as core
import ext.math as math
import ram.timer as timer


#std imports
import time


COMPLETE = core.declareEventType('COMPLETE')


"""
Let's treat the naming for the states like a simple essay
"""

class Intro(oldState.State):
    """
    I am the intro, I'm supposed to lead you into the main body of the 'essay'
    with some understanding of what's going to happen...

    Good luck with that. I just tell you to go to the Body
    This is only used in autonomous runs, so I'm not actually accessed here
    (Yay, I'm useless)
    """

    @staticmethod
    def transitions():
        return {'Start' : Body}


class Start(oldState.State):
    """
    Just like the body of an essay I contain the meat of this routine
    you'll find within me a different stateMachine, *cue the inception horn*.
    This stateMachine will actually perform the whole of the course.
    As of now this version is an experimental version, but just like
    in the real world beta = full release right?
    For more information view ram/ai/new/stateMachine.py and 
    ram/ai/new/state.py

    """
    
    # These tell the state to perform some clean up before exiting
    # and cause some logging for exit cases
    machine_complete = core.declareEventType('MACHINE_COMPLETE')
    time_out = core.declareEventType('TIME_OUT')
    update = core.declareEventType('UPDATE')
    #Actally transition to the end state (in this case Conclusion)
    terminate = core.declareEventType('TERMINATE')

    @staticmethod
    def transitions():
        return { Start.machine_complete : Start,
                 Start.time_out : Start,
                 Start.update : Start,
                 Start.terminate : Conclusion }

    def enter(self):
        self._updateDelay = 0.1
        self._courseTimeOut = 1200
        """
        Since this will only be called on entry for the first time
        all initialization will be done here.
        """
        self._courseTimeOutTimer = self.timerManager.newTimer(Start.time_out,
                                                             self._courseTimeOut)
        self._updateDelay = self._updateDelay

        self._machine = testMachine.TestMachine()
        self._machine.setLegacyState(self)
        self._machine.setStartState('test1') # needed here because not set
                                             # manually in machine
        self._machine.configure(self._config)
        self._machine.start()
        self.publish(Start.update, core.Event())
        
    def exit(self):
        """
        Normally, clean up would be done here but in this case we're just
        leaving since clean up is done in our callbacks
        """
        pass

    def UPDATE(self, event):
        time.sleep(self._updateDelay)
        self._machine.update()
        if self._machine.completed():
            self.publish(Start.machine_complete, core.Event())
        else:
            self.publish(Start.update, core.Event())

    def MACHINE_COMPLETE(self, event):
        """ I'll do something later i.e. cleanup """
        self.publish(Start.terminate, core.Event())

    def TIME_OUT(self, event):
        """ I'll do something later i.e cleanup """
        self.publish(Start.terminate, core.Event())

#class CallBackHelper(oldState.State):

class Conclusion(oldState.State):
    """
    I'm the conclusion, mostly I'm just telling the stateMachine I'm in that
    this graph has reached an end state.
    """
    
    def enter(self):
        self.publish(COMPLETE, core.Event())
