# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/ai/state.py

# STD Imports

# Project Imports
import ext.core as core
import ram.ai.state as state

# Special event that denotes 
TIMEOUT = core.declareEventType('TIMEOUT')        

class Next(state.State):
    """
    Special state denotes that the next task should be moved to
    """
    pass

class Failure(state.State):
    """
    Special state denotes that the task failed in an *unrecoverable* way.
    """
    pass

class End(state.State):
    """
    Special state that denotes the complete end of the state machine
    """
    pass

class Task(state.State):
    """
    Encapsulates a single AI task, like completing an objective.  It allows for
    the implementation and testing of such tasks without concern for what comes
    before, or after said task.
    
    It queries the AI subsystem to ask which state is after itself, and 
    replaces the marker ram.ai.task.Next state with that state. It also also
    handles all the timeout machinery internally.
    """
    def __init__(self, config = None, **subsystems):
        # Call the super class
        state.State.__init__(self, config, **subsystems)
        
        # Dynamically create our event
        self._timeoutEvent = core.declareEventType(
            'TIMEOUT_' + self.__class__.__name__)
        
        # From the AI grab our next task
        self._nextState = self.ai.getNextTask(type(self))
        self._failureState = self.ai.getFailureState(type(self))
    
        # Timeout related values, set later on
        self._hasTimeout = False
        self._timeoutDuration = None
        self._timer = None
    
    @property
    def timeoutEvent(self):
        return self._timeoutEvent
    
    @property
    def timeoutDuration(self):
        return self._timeoutDuration
        
    def transitions(self):
        """
        A dynamic transition function which allows you to wire together a 
        missions dynamically.
        """
        baseTrans = self._transitions()
        newTrans = {}
        for eventType, nextState in baseTrans.iteritems():
            # Catch the timeout event and replace with our class specific 
            # timeout event type
            if eventType == TIMEOUT:
                eventType = self._timeoutEvent
                self._hasTimeout = True
                
            
            if nextState == Next:
                # If the next state is the special Next marker state, swap it 
                # out for the real next state
                nextState = self._nextState
            elif nextState == Failure:
                # If that state is the special failure marker state, swap it 
                # out for the real failure state
                if self._failureState is None:
                    raise "ERROR: transition to non existent failure state"
                nextState = self._failureState
            
            # Store the event
            newTrans[eventType] = nextState
            
        return newTrans
    
    def enter(self, defaultTimeout = None):
        if self._hasTimeout:
            # Get timeout duration from configuration file
            if defaultTimeout is None:
                self._timeoutDuration = self._config.get('timeout')
            else:
                self._timeoutDuration = self._config.get('timeout', 
                                                          defaultTimeout)
            # Start our actual timeout timer
            self._timer = self.timerManager.newTimer(self._timeoutEvent, 
                                                    self._timeoutDuration)
            self._timer.start()
            
    def exit(self):
        if self._timer is not None:
            self._timer.stop()
