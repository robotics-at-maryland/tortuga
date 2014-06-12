# Copyright (C) 2009 Maryland Robotics Club
# Copyright (C) 2009 Jonathan Sternberg <jsternbe@umd.edu>
# All rights reserved.
#
# Author: Jonathan Sternberg <jsternbe@umd.edu>

# STD Imports
from datetime import datetime
import sys
import traceback
import Ice
import ice.RemoteAi as RemoteAi

# Project Imports
import ext.core as core
from ram.logloader import resolve
import ram.ai.state
import ram.ai.task as task
import ram.timer as timer

# Any print has a small chance of crashing the program.
# Don't print anything.
class AiInformationImpl(RemoteAi.AiInformation):
    def __init__(self):
        self._eventList = []

    def RecordEvent(self, event):
        self._eventList.append(event)

    def GetInfo(self, seconds, current = None):
        return self._eventList

class AiInformationServer(core.Subsystem):
    """
    Tracks information about state transitions
    """

    def __init__(self, cfg, deps):
        core.Subsystem.__init__(self, cfg.get('name', 'RemoteAi'), deps)

        self._connections = []

        self.qeventHub = core.Subsystem.getSubsystemOfType(core.QueuedEventHub,
                                                           deps)

        if self.qeventHub is None:
            raise Exception("Could not find QueuedEventHub in"
                            " the dependencies.")

        conn = self.qeventHub.subscribeToType(
            ram.ai.state.Machine.STATE_ENTERED, self._onEntered)
        self._connections.append(conn)

        conn = self.qeventHub.subscribeToType(
            ram.ai.state.Machine.STATE_EXITED, self._onExited)
        self._connections.append(conn)

        self._eventList = []
        self._firstRun = True
        
        self.ic = None
        try:
            self.ic = Ice.initialize(sys.argv)
            adapter = self.ic.createObjectAdapterWithEndpoints(
                "RemoteAi", "default -p 10000")
            self._obj = AiInformationImpl()
            adapter.add(self._obj, self.ic.stringToIdentity("AiInfo"))

            adapter.activate()
        except:
            traceback.print_exc()

    def __del__(self):
        for conn in self._connections:
            conn.disconnect()

        if self.ic:
            # Clean up
            try:
                self.ic.destroy()
            except:
                traceback.print_exc()

    def _recordEvent(self, event):
        if self._firstRun:
            self._startTime = timer.time()
            self._firstRun = False

        newEvent = RemoteAi.StateEvent()

        # Explicit cast necessary for Ice
        newEvent.timeStamp = long(event.timeStamp - self._startTime)
        newEvent.eventType = event.type
        # Find the state name
        fullClassName = str(event.string)

        # Recreate the class
        stateClass = resolve(fullClassName)
        if issubclass(stateClass, task.Task):
            # It's a task, make that the type
            newEvent.type = "Task"
        else:
            newEvent.type = "State"
        # Parse out the last name from the fullClassName
        newEvent.name = fullClassName.split('.')[-1]

        # Record this event in the adapter
        self._obj.RecordEvent(newEvent)

    def _onEntered(self, event):
        self._recordEvent(event)

    def _onExited(self, event):
        self._recordEvent(event)

core.registerSubsystem('RemoteAi', AiInformationServer)
