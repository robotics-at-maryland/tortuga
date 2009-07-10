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
import ram.motion as motion
import ext.core as core

class State(object):
    """
    Basic state class, its provides empty implementation for all the needed
    methods of a state
    """
    def __init__(self, config = None, **subsystems):
        if config is None:
            config = {}
        self._config = config
        for name, subsystem in subsystems.iteritems():
            if 'config' == name:
                raise ValueError, "Subsystem cannot be named 'config'"
            setattr(self, name, subsystem)

    @staticmethod
    def transitions():
        """
        Returns a map of eventTypes -> resulting states, loopbacks are allowed
        """
        return {}

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
    
    def publish(self, eventType, event):
        """
        Publish an event, with the owning Machine object as publisher
        
        @warning: Only valid when the object is created by a Machine object
        """
        raise 

class FindAttempt(State):
    """
    Default state for finding a lost target
    """

    TIMEOUT = core.declareEventType("TIMEOUT")

    def __init__(self, config = None, **kwargs):
        State.__init__(self, config, **kwargs)

    @staticmethod
    def transitions(foundEvent, foundState, timeoutState, trans = None):
        if trans is None:
            trans = {}
        trans.update({foundEvent : foundState,
                      FindAttempt.TIMEOUT : timeoutState})

        return trans

    def enter(self, timeout = 2):
        # Turn off all motions, hold the current heading
        self.motionManager.stopCurrentMotion()
        self.controller.holdCurrentHeading()

        # Create a timer event
        self._timeout = self._config.get('timeout', timeout)
        # Timer will only state if the timeout is a positive number
        # A timer of 0 will turn it off, along with any negative number
        if self._timeout > 0:
            self.timer = \
                self.timerManager.newTimer(FindAttempt.TIMEOUT, self._timeout)
            self.timer.start()
        elif self._timeout < 0:
            # A negative timer will automatically move to the timeoutState
            self.timer = None
            self.publish(FindAttempt.TIMEOUT, core.Event())
        else:
            # A timer of zero will turn off the timer and will only allow
            # FindAttempt to exit by finding the target
            self.timer = None

    def exit(self):
        if self.timer is not None:
            self.timer.stop()
        self.motionManager.stopCurrentMotion()
        
class MultiMotion(State):
    """
    Default state for multiple motions in succession.
    
    ONLY USE MOTIONS THAT FINISH (the publish Motion.FINISHED at some point)
    """
    
    FINISH_MULTI_MOTION = core.declareEventType("FINISH_MULTI_MOTION")
    
    @staticmethod
    def transitions(currentState, nextState = None):
        if nextState is None:
            nextState = End
        return {motion.basic.Motion.FINISHED : currentState,
                MultiMotion.FINISH_MULTI_MOTION : nextState }
        
    def FINISHED(self, event):
        self._currentMotionNumber += 1
        if self._numberOfMotions > self._currentMotionNumber:
            self.motionManager.setMotion(
                self._motionList[self._currentMotionNumber])
        else:
            self.publish(MultiMotion.FINISH_MULTI_MOTION, core.Event())
    
    def enter(self, *motions):
        self._numberOfMotions = len(motions)
        self._motionList = motions
        self._currentMotionNumber = 0
        
        if self._numberOfMotions == 0:
            self.publish(MultiMotion.FINISH_MULTI_MOTION, core.Event())
        else:
            self.motionManager.setMotion(self._motionList[0])
            
    def exit(self):
        self.motionManager.stopCurrentMotion()

class End(State):
    """
    State machine which demarks a valid end point for a state machine
    
    Ensure that any "dead ends" (states with no out transitions) are actually
    intended to be that way.
    """
    def __init__(self, config = None, **kwargs):
        State.__init__(self, config, **kwargs)

class Branch(object):
    """
    A marker class indication we branch the state machine
    """
    def __init__(self, state):
        """
        @type state: ram.ai.state.State
        @param state: The state to branch to
        """
        self.state = state

class Machine(core.Subsystem):
    """
    An event based finite state machine.
    
    This machine works with graph of ram.ai.state.State classes.  This graph
    represents a state machine.  There can only be one current state at a time.
    When events are injected into the state machine the currents states 
    transition table determines which state to advance to next.
    
    Requires the following subsystems:
     - ext.core.QueuedEventHub
    
    @type _root: ram.ai.state.State
    @ivar _root: The first state of the Machine
    
    @type _currentState: ram.ai.state.State
    @ivar _currentState: The current state which is processing events
    
    @type _started: boolean
    @ivar _started: The Machine will not process events unless started
    
    @type _qeventHub: ext.core.QueuedEventHub
    @ivar _qeventHub: The thread safe to subscribe to events through
    
    @type _previousEvent: ext.core.Event
    @ivar _previousEvent: The last event injected into the state machine
    
    @type _connections: [ext.core.EventConnection]
    @ivar _connections: The list of event connections for current state
    
    @type _subsystems: {str : ext.core.Subsystem }
    @ivar _subsystems: The subsystems provided to each state
    """
    
    STATE_ENTERED = core.declareEventType('STATE_ENTERED')
    STATE_EXITED = core.declareEventType('STATE_EXITED')
    COMPLETE = core.declareEventType('COMPLETE')
    
    def __init__(self, cfg = None, deps = None):
        if deps is None:
            deps = []
        if cfg is None:
            cfg = {}
            
        core.Subsystem.__init__(self, cfg.get('name', 'StateMachine'),
                                deps)

        # Set default instance values
        self._config = cfg
        self._root = None
        self._currentState = None
        self._started = False
        self._complete = False
        self._qeventHub = None
        self._previousEvent = core.Event()
        self._connections = []
        self._subsystems = {}
        self._branches = {}
        
        # Deal with Subsystems
        self._qeventHub = core.Subsystem.getSubsystemOfType(core.QueuedEventHub,
                                                            deps)
        for subsystem in deps:
            # Make first letter lower case
            name = subsystem.getName()
            name = name[0].lower() + name[1:]
            
            # Set subsystem up
            self._subsystems[name] = subsystem
            
            # Handle special AI case where it needs a reference to us
            if name.lower() == 'ai' and hasattr(subsystem, '_stateMachine'):
                subsystem._stateMachine = self

    def update(self, timeStep):
        print 'STATE UPDATE'
	
    def backgrounded(self):
        return True

    def background(self, interval):
        # Add really nasty cirular reference to the AI subsystem
        for name, sys in self._subsystems.iteritems():
            if name.lower() == 'ai' and hasattr(sys, '_stateMachine'):
                sys._stateMachine = self

    def unbackground(self, force):
        # Remove really nasty cirular reference to the AI subsystem
        for name, sys in self._subsystems.iteritems():
            if name.lower() == 'ai' and hasattr(sys, '_stateMachine'):
                sys._stateMachine = None

    def currentState(self):
        return self._currentState

    def start(self, startState):
        """
        Starts or branches the state machine with the given state
        
        If the given state is really a branch, it will branch to that state
        instead.
        """
        
        if Branch == type(startState):
            # Determine if we are branching
            branching = True
            self._branchToState(startState.state)
        else:
            self._root = startState
            self._started = True
            self._complete = False

            self._enterState(startState)

    def stop(self):
        """
        Exits the current state, and stops if from responding to any more
        events. Also stops all branches
        """
        if self._currentState is not None:
            self._exitState()
        
        self._started = False
        self._previousEvent = core.Event()
        self._root = None
        self._currentState = None
        self._complete = False
        
        for branch in self._branches.itervalues():
            branch.stop()
        self._branches = {}
        
    def stopBranch(self, stateType):
        """
        Stops just the desired branch, and its current type
        """
        self._branches[stateType].stop()
        del self._branches[stateType]

    def injectEvent(self, event, _sendToBranches = False):
        """
        Sends an event into the state machine
        
        If currents states transition table has an entry for events of this 
        type this will cause a transition
        
        @type  event: ext.core.Event
        @param event: A new event for the state machine to process 
        
        @type _sendToBranches: bool
        @param _sendToBranches: Use only for testing, injects events into 
                                branched state machines
        """
        # If the state we just entered transitions on same kind of event that
        # caused the transition, we can be notified again with the same event!
        # This check here prevents that event from causing an unwanted 
        # transition.
        if event == self._previousEvent:
            return
        
        if not self._started:
            raise Exception("Machine must be started")
        
        transitionTable = self._currentState.transitions()
        nextState = transitionTable.get(event.type, None)
        if nextState is not None:
            # Determine if we are branching
            branching = False
            if Branch == type(nextState):
                branching = True
                nextState = nextState.state
                
            # Detemine if we are in a loopback
            loopback = False
            if nextState == type(self._currentState):
                loopback = True
                
            # For loops backs or branches we don't reenter, or exit from our 
            # state, just call the transition function
            leaveState = False
            if (not branching) and (not loopback):
                leaveState = True
            
            # We are leaving the current state
            currentState = self._currentState
            if leaveState:
                self._exitState()
            
            # Call the function for the transitions
            transFunc = self._getTransitionFunc(event.type, currentState)
            if transFunc is not None:
                transFunc(event)

            # Notify that we are entering the next state
            if (not loopback) and (not branching):
                # Create an instance of the next state's class
                self._enterState(nextState)
            elif branching:
                self._branchToState(nextState)
                
        # Record previous event
        self._previousEvent = event
        
        if _sendToBranches:
            for branch in self._branches.itervalues():
                branch.injectEvent(event)

    @property
    def complete(self):
        """Returns true when """
        return self._complete

    def _enterState(self, newStateClass):
        """
        Does all the house keeping when entering a new state
        """
        
        # Look up config based on full dotted name of state class
        fullClassName = '%s.%s' % (newStateClass.__module__, 
                                   newStateClass.__name__)
        stateCfg = self._config.get('States', {})
        config = stateCfg.get(fullClassName, {})
        
        # Add self to the list of subsystems
        subsystems = self._subsystems
        name = self.getName()
        subsystems[name[0].lower() + name[1:]] = self
        
        # Create state instance from class, make sure to pass all subsystems
        # along as well
        newState = newStateClass(config, **self._subsystems)
        newState.publish = self.publish
        
        # Subscribe to every event of the desired type
        transitionTable = newState.transitions()
        if self._qeventHub is not None:
            for eventType in transitionTable.iterkeys():
                conn = self._qeventHub.subscribeToType(eventType, 
                                                       self.injectEvent)
                self._connections.append(conn)
        
        # Actual enter the state and record it as our new current state
        self._currentState = newState
        self._currentState.enter()
        
        # Notify everyone we just entered the state
        event = core.StringEvent()
        event.string = self._currentState.__class__.__name__
        self.publish(Machine.STATE_ENTERED, event)
        
        # If we are in a state with no way out, exit the state and mark ourself
        # complete
        if 0 == len(transitionTable):
            self._exitState()
            self._complete = True
            self.publish(Machine.COMPLETE, core.Event())
        
    def _exitState(self):
        """
        Does all the house keeping for when you are exiting an old state
        """
        self._currentState.exit()
                
        for conn in self._connections:
            conn.disconnect()
        self._connections = []
                
        changeEvent = core.StringEvent()
        changeEvent.string = self._currentState.__class__.__name__
        self.publish(Machine.STATE_EXITED, changeEvent)
        
        self._currentState = None

    def _branchToState(self, nextState):
        if self._branches.has_key(nextState):
            raise Exception("Already branched to this state")
        
        # Create new state machine
        deps = core.SubsystemList()
        for subsystem in self._subsystems.itervalues():
            deps.append(subsystem)
        branchedMachine = Machine(self._config, deps)
        
        # Start it up with the proper state
        branchedMachine.start(nextState)
        
        # Store new state machine
        self._branches[nextState] = branchedMachine

    def _getTransitionFunc(self, etype, obj):
        """
        Determines which funtion during a transistaion between states
        
        This uses the event type of the event which caused the transition to
        determine which member funtion of the self._currentState to call.
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

    @property
    def branches(self):
        return self._branches

    @staticmethod
    def writeStateGraph(fileobj, state, ordered = False, noLoops = False):
        """
        Write the graph of the state machine starting at the given state to
        the fileobj.
    
        @type  fileobj: a file like object
        @param fileobj: The object to write the result graph to (ie:
                        fileobject.write(graphtext))
       
        @type  state: ram.ai.state.State
        @param state: The state to start the graph at
        
        @type  ordered: boolean
        @param ordered: Whether or not to alphabetize the states
        """
        fileobj.write("digraph aistate {\n")
        stateTransitionList = []
        traversedStates = []
        
        Machine._traverse(state, stateTransitionList, traversedStates, noLoops)
        
        # Sort list for determinism
        if ordered:
            stateTransitionList.sort()

        # Output Labels in Simple format        
        traversedStates.sort(key = Machine._dottedName)
        for state in traversedStates:
            fullName = Machine._dottedName(state)
            shortName = state.__name__
            # Shape denots "end" states with a "Stop Sign" type shape
            shape = 'ellipse'
            if 0 == len(state.transitions()):
                shape = 'doubleoctagon'
            fileobj.write('%s [label=%s,shape=%s]\n' % \
                          (fullName, shortName, shape))

        for item in stateTransitionList:
            fileobj.write(item + "\n")
        fileobj.write("}")
        fileobj.flush() # Push data to file
        
    @staticmethod
    def _traverse(currentState,stateList,traversedList,noLoops=False):
        if 0 == len(currentState.transitions()):
            if not currentState in traversedList:
                    traversedList.append(currentState)
        else:
            for aiEvent,aiState in currentState.transitions().iteritems():
                eventName = str(aiEvent).split(' ')[-1]
                
                # Style is determine whether or not we are branching
                style = "solid"
                if type(aiState) is Branch:
                    style = "dotted"
                    aiState = aiState.state
                
                # Determine state names
                startName = Machine._dottedName(currentState)
                endName = Machine._dottedName(aiState)

                if (not noLoops) or (startName != endName):
                    strStruct = "%s -> %s [label=%s,style=%s]" % \
                        (startName, endName, eventName, style)
                    stateList.append(strStruct)
                    if not currentState in traversedList:
                        traversedList.append(currentState)

                    # Don't recuse on a state we have already seen
                    if not aiState in traversedList:
                        Machine._traverse(aiState, stateList,
                                          traversedList, noLoops)
    @staticmethod
    def _dottedName(cls):
        return cls.__module__.replace('.','_') + '_' + cls.__name__
    
core.registerSubsystem('StateMachine', Machine)
