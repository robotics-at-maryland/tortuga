# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/ai/state.py

# STD Imports
import inspect
import types

# Project Imports
import ext.core as core

class State(object):
    """
    Basic state class, its provides empty implementation for all the needed
    methods of a state
    """
    def __init__(self, machine):
        self.machine = machine

    @staticmethod
    def transitions():
        """
        Returns a map of eventTypes -> resulting states, loopbacks are allowed
        """
        pass

    def enter(self):
        """
        Called when the state is entered, loopbacks don't count
        """
        pass

    def exit(self):
        """
        Called when the state is exited, loopbacks don't count
        """
        pass

class Machine(core.Subsystem):
    """
    An event based finite state machine.
    
    This machine works with graph of ram.ai.state.State classes.  This graph
    represents a state machine.  There can only be one current state at a time.
    When events are injected into the state machine the currents states 
    transition table determines which state to advance to next.
    
    @type _root: ram.ai.state.State
    @ivar _root: The first state of the Machine
    
    @type _currentState: ram.ai.state.State
    @ivar _currentState: The current state which is processing events
    
    @type _started: boolean
    @ivar _started: The Machine will not process events unless started
    """
    
    STATE_ENTERED = core.declareEventType('STATE_ENTERED')
    STATE_EXITED = core.declareEventType('STATE_EXITED')
    
    def __init__(self, cfg = None, deps = None):
        if deps is None:
            deps = []
        if cfg is None:
            cfg = {}
            
        core.Subsystem.__init__(self, cfg.get('name', 'StateMachine'),
                                deps)

        self._root = None
        self._currentState = None
        self._started = False
        self._qeventHub = None
        
        self._qeventHub = core.Subsystem.getSubsystemOfType(core.QueuedEventHub,
                                                            deps)

    def currentState(self):
        return self._currentState

    def start(self, startState):
        self._root = startState
        self._started = True

        self._enterState(startState(self))

    def injectEvent(self, event):
        """
        Sends an event into the state machine
        
        If currents states transition table has an entry for events of this 
        type this will cause a transition
        
        @type  event: ext.core.Event
        @param event: A new event for the state machine to process 
        """
        if not self._started:
            raise Exception("Machine must be started")
        
        transitionTable = type(self._currentState).transitions()
        nextState = transitionTable.get(event.type, None)

        if nextState is not None:
            # For loops backs we don't reenter, or exit from our state, just
            # call the transition function
            loopback = False
            if nextState == type(self._currentState):
                loopback = True
            
            # We are leaving the current state
            if not loopback:
                self._exitState()
            
            # Call the function for the transitions
            transFunc = self._getTransitionFunc(event.type, self._currentState)
            if transFunc is not None:
                transFunc(event)

            # Notify that we are entering the next state
            if not loopback:
                # Create an instance of the next state's class
                self._enterState(nextState(self))

    def _enterState(self, newState):
        """
        Does all the house keeping when entering a new state
        """
        self._currentState = newState
        self._currentState.enter()
        
        event = core.Event()
        event.state = self._currentState
        self.publish(Machine.STATE_ENTERED, event)
        
    def _exitState(self):
        """
        Does all the house keeping for when you are exiting an old state
        """
        self._currentState.exit()
                
        changeEvent = core.Event()
        changeEvent.state = self._currentState
        self.publish(Machine.STATE_EXITED, changeEvent)

    def _getTransitionFunc(self, etype, obj):
        """
        Determines which funtion during a transistaion between states
        
        This uses the event type of the event which caused the transition to
        determine which member funtion of the state for which the transition
        occured.
        """
        # Trim etype of namespace stuff
        etype = etype.split(' ')[-1]
        
        # Grab all member functions
        members = inspect.getmembers(obj, inspect.ismethod)

        # See if we have a matching method
        matches = [func for name,func in members if name == etype]

        # We found it
        assert len(matches) < 2
        if len(matches) > 0:
            return matches[0]

core.registerSubsystem('StateMachine', Machine)