# new project imports as of June 2014
import ram.ai.stateMachine as stateMachine
import ram.ai.newState as state

#Old project imports 2013
import ram.ai.state as oldState
import ext.core as core
import ext.math as math
import ram.timer as timer


#test imports
import ram.ai.testSquare as testSquare


COMPLETE = core.declareEventType('COMPLETE')
WAIT_LOOP = core.declareEventType('WAIT_LOOP')

global courseTimeOut = 1200 # 20 minutes
global updateDelay = 0.1 # update at 10 hertz

"""
Let's treat the naming for the states like a simple essay
"""

class Intro(oldState.state):
    """
    I am the intro, I'm supposed to lead you into the main body of the 'essay'
    with some understanding of what's going to happen...

    Good luck with that. I just tell you to go to the Body
    """

    @staticmethod
    def transitions():
        return {'Start' : body}
    

class Body(oldState.State):
    """
    Just like the body of an essay I contain the meat of this routine
    you'll find within me a different stateMachine, *cue the inception horn*.
    This stateMachine will actually perform the whole of the course.
    As of now this version is an experimental version, but just like
    in the real world beta = full release right?
    For more information view ram/ai/stateMachine.py and ram/ai/newState.py
    """
    # These tell the state to perform some clean up before exiting
    # and cause some logging for exit cases
    MACHINE_COMPLETE = core.declareEventType('MACHINE_COMPLETE')
    TIME_OUT = core.declareEventType('TIME_OUT')
    #Actally transition to the end state (in this case Conclusion)
    TERMINATE = core.declareEventType('TERMINATE')


    @staticmethod
    def transitions():
        return { Body.MACHINE_COMPLETE : Body,
                 Body.TIME_OUT : Body,
                 Body.UPDATE : Body,
                 Body.TERMINATE : Conclusion}

    def enter(self):
        global updateDelay
        global courseTimeOut
        """
        Since this will only be called on entry for the first time
        all initialization will be done here.
        """
        self._courseTimeOutTimer = self.timeManager.newTimer(Body.TIME_OUT,
                                                             courseTimeOut)
        self._updateDelay = updateDelay
        #Reminders
        '''
        Call configure in here
        Set up delayTimer for update event publishing at the end of this
        '''
        
    def exit(self):
        """
        Normally, clean up would be done here but in this case we're just
        leaving since clean up is done in our callbacks
        """
        self._machine = stateMachine.StateMachine(self._config, )

    def UPDATE(self, event):
        time.sleep(self._updateDelay)
        self._machine.update()

    def MACHINE_COMPLETE(self, event):
        pass """ I'll do something later i.e. cleanup """
        self.publish(Body.TERMINATE, core.Event())

    def TIME_OUT(self, event):
        pass """ I'll do something later i.e cleanup """
        self.publish(Body.TERMINATE, core.Event())

class Conclusion(oldState.State):
    """
    I'm the conclusion, mostly I'm just telling the stateMachine I'm in that
    this graph has reached an end state.
    """
    
    def enter(self):
        self.publish(COMPLETE, core.Event())
