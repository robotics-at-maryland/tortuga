# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/ai/subsystem.py

# Project Imports
import ext.core as core
import ram.ai as ai
import ram.ai.task
from ram.logloader import resolve

class AI(core.Subsystem):
    """
    The subsystem which forms the core of the AI.  It allows for data storage
    and transfer between states, as well the management of the task system.
    """
    def __init__(self, cfg = None, deps = None):
        if deps is None:
            deps = []
        if cfg is None:
            cfg = {}
            
        core.Subsystem.__init__(self, cfg.get('name', 'Ai'),
                                deps)
        self._connections = []
        
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
        #assert (not (self._stateMachine is None)), "Could not find aistate machine"
                
        # Store inter state data
        self._data = {}
        self._data['config'] = cfg.get('config', {})

        self._checkConfig(self._data['config'])
        
        # Build list of next states
        self._nextTaskMap = {}
        self._taskOrder = []
        
        taskOrder = cfg.get('taskOrder', None)
        if taskOrder is None:
            taskOrder = []
        for i, taskName  in enumerate(taskOrder):
            # Determine which task is really next
            nextTask = 'ram.ai.task.End'
            if i != (len(taskOrder) - 1):
                nextTask = taskOrder[i + 1]
            
            # Resolve dotted task names into classes
            taskClass = resolve(taskName)
            nextClass = resolve(nextTask)
            
            # Store the results
            self._nextTaskMap[taskClass] = nextClass
            
            # Record the current class
            self._taskOrder.append(taskClass)
            
        # Build list of failure tasks
        self._failureTaskMap = {}
        failureTasks = cfg.get('failureTasks', None)
        if failureTasks is None:
            failureTasks = {}
        for taskName, failTaskName in failureTasks.iteritems():
            # Resolve dotted task names into classes
            taskClass = resolve(taskName)
            failureTaskClass = resolve(failTaskName)
            
            # Store results
            self._failureTaskMap[taskClass] = failureTaskClass
    
    def _checkConfig(self, cfg):
        options = set(['gateDepth', 'lightDepth', 'pipeDepth', 'bwireDepth',
                         'targetDepth', 'binStartDepth', 'binDepth',
                         'targetSymbols', 'sonarDepth', 'safeDepth',
                         'safeOffset'])
        pipeOptions = set(['biasDirection', 'threshold'])
        pipeObjective = set(['biasDirection', 'threshold', 'rotation',
                             'duration', 'legTime', 'sweepAngle', 'sweepSpeed'])
        moveOptions = set(['heading', 'speed', 'absolute',
                           'forwardDuration', 'forwardSpeed'])
        for item in cfg.iterkeys():
            if item == 'Pipe' or item == 'Pipe1' or item == 'Pipe2' or \
                    item == 'Pipe3' or item == 'PipeGate' or \
                    item == 'PipeStaged':
                for innerItem in cfg[item].iterkeys():
                    if innerItem not in pipeOptions:
                        raise Exception("'%s' is not a valid config "
                                        "option for %s." % (innerItem, item))
            elif item == 'PipeBarbedWire' or item == 'PipeBin' or \
                    item == 'PipeTarget':
                for innerItem in cfg[item].iterkeys():
                    if innerItem not in pipeObjective:
                        raise Exception("'%s' is not a valid config "
                                        "option for %s." % (innerItem, item))
            elif item == 'Light' or item == 'BarbedWire' \
                    or item == 'Target' or item == 'Bin' or \
                    item == 'LightStaged':
                for innerItem in cfg[item].iterkeys():
                    if innerItem not in moveOptions:
                        raise Exception("'%s' is not a valid config "
                                        "option for %s." % (innerItem, item))
            else:
                if item not in options:
                    raise Exception("'%s' is not a valid config option." % (
                            item))

    # IUpdatable methods
    def update(self, timeStep):
        print 'AI'

    def backgrounded(self):
        return True
        
    def unbackground(self, join = False):
        core.Subsystem.unbackground(self, join)
        for conn in self._connections:
            conn.disconnect()
    def backgrounded(self):
        return True

    # Properties
    @property
    def mainStateMachine(self):
        return self._stateMachine

    @property
    def data(self):
        return self._data

    # Other methods
    def start(self):
        """
        Starts the state machine with the first task from the task list
        """
        self._stateMachine.start(self._taskOrder[0])
    
    def stop(self):
        """
        Stops the currently running state machine.
        """
        self._stateMachine.stop()
    
    def addConnection(self, conn):
        self._connections.append(conn)

    def getNextTask(self, task):
        """
        Returns the task which will be transitioned to when the given task 
        transitions to the next task.
        """
        return self._nextTaskMap[task]
    
    def getFailureState(self, task):
        """
        Returns the state which will be transitioned to when the given task
        fails in an unrecoverable way.  Recoverable failure are handled 
        internally by that task. 
        """
        return self._failureTaskMap.get(task, None)
    
    
core.registerSubsystem('AI', AI)
