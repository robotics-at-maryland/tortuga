# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/ai/subsystem.py

# Project Imports
import ext.core as core
import ram.ai as ai

class AI(core.Subsystem):
    def __init__(self, cfg = None, deps = None):
        if deps is None:
            deps = []
        if cfg is None:
            cfg = {}
            
        core.Subsystem.__init__(self, cfg.get('name', 'AI'),
                                deps)
        
        # Gather all the state machines
        stateMachines = []
        for d in deps:
            if isinstance(d, ai.state.Machine):
                stateMachines.append(d)
        
        # Find the main one
        self._stateMachine = None
        for m in stateMachines:
            if m.getName() == cfg.get('AIMachineName', 'StateMachine'):
                self._stateMachine = m
                break
        assert (not (self._stateMachine is None)), "Could not find aistate machine"
                
        # Store inter state data
        self._data = {}
        
    @property
    def mainStateMachine(self):
        return self._stateMachine

    @property
    def data(self):
        return self._data 
    
    
core.registerSubsystem('AI', AI)