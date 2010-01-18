# Copyright (C) 2009 Maryland Robotics Club
# Copyright (C) 2009 Jonathan Sternberg <jsternbe@umd.edu>
# All rights reserved.
#
# Author: Jonathan Sternberg <jsternbe@umd.edu>
# File: packages/python/ram/monitor.py

# STD Imports
import os
import time
from datetime import datetime

# Project imports
import ext.core as core
import ext.math as math
import ext.vehicle
import ram.timer as timer
from ram.logloader import resolve

class Monitor(core.Subsystem):
    """
    The subsystem that monitors values and sends out a warning or failure
    signal if the values get too large.
    """

    NOMINAL = core.declareEventType("NOMINAL")
    WARNING = core.declareEventType("WARNING")
    CRITICAL = core.declareEventType("CRITICAL")

    def __init__(self, cfg, deps):
        core.Subsystem.__init__(self, cfg.get('name', 'Monitor'), deps)

        # Get essential subsystems
        self._qeventHub = \
            core.Subsystem.getSubsystemOfType(core.QueuedEventHub,
                                              deps, nonNone = True)

        self._signals = {}

        # Clear config file of extra values
        if cfg.has_key('type'):
            cfg.pop('type')
        if cfg.has_key('depends_on'):
            cfg.pop('depends_on')
        cfg.pop('name')

        # Iterate through the config file subscribing to any events it lists
        for name, data in cfg.iteritems():
            publisher = self._lookupByName(data.get('publisher', None), deps)
            self._signals[name] = Signal(self._qeventHub,
                                         publisher,
                                         data['eventType'],
                                         data['property'],
                                         data['name'],
                                         data['warning'],
                                         data['critical'])

    def _lookupByName(self, publisher, deps):
        # Subscribe to the queued event hub if no publisher is specified
        if publisher is None:
            return self._qeventHub

        # If the publisher is a device, different operations are performed
        if publisher.startswith('Vehicle.Device.'):
            deviceName = publisher[len('Vehicle.Device.'):]
            vehicle = core.Subsystem.getSubsystemOfType(
                ext.vehicle.IVehicle, deps)
            device = vehicle.getDevice(deviceName)
            return device
        else:
            # Normal operations for any other publisher
            systemName = publisher.split('.')[0]
            for d in deps:
                depName = d.getPublisherName().split('.')[-1]

                if depName == systemName:
                    return d

    def unbackground(self, join = False):
        # Delete all signals
        for signal in self._signals.itervalues():
            del signal

    def backgrounded(self):
        return True

class Signal(object):
    def __init__(self, qeventHub, publisher, eventType, propName, name,
                 warning, critical):
        self._qeventHub = qeventHub
        self._publisher = publisher
        self._eventType = eventType
        self._property = propName
        self._name = name
        self._warning = warning
        self._critical = critical
        self._lastValue = 0

        eventType = resolve(self._eventType)

        self._conn = self._qeventHub.subscribe(eventType, self._publisher,
                                               self._handler)

    def __del__(self):
        self._conn.disconnect()

    def _checkMaximum(self, value, event):
        if value > self._lastValue:
            if value > self._critical and self._lastValue <= self._critical:
                # Publish a critical event
                self._qeventHub.publish(Monitor.CRITICAL, event)
            elif value > self._warning and self._lastValue <= self._warning:
                # Publish a warning event
                self._qeventHub.publish(Monitor.WARNING, event)
        else:
            if value <= self._warning and self._lastValue > self._warning:
                # Publish a nominal event
                self._qeventHub.publish(Monitor.NOMINAL, event)
            elif value <= self._critical and self._lastValue > self._critical:
                # Publish a warning event
                self._qeventHub.publish(Monitor.WARNING, event)

    def _checkMinimum(self, value, event):
        if value < self._lastValue:
            if value < self._critical and self._lastValue >= self._critical:
                # Publish a critical event
                self._qeventHub.publish(Monitor.CRITICAL, event)
            elif value < self._warning and self._lastValue >= self._warning:
                # Publish a warning event
                self._qeventHub.publish(Monitor.WARNING, event)
        else:
            if value >= self._warning and self._lastValue < self._warning:
                # Publish a nominal event
                self._qeventHub.publish(Monitor.NOMINAL, event)
            elif value >= self._critical and self._lastValue < self._critical:
                # Publish a warning event
                self._qeventHub.publish(Monitor.WARNING, event)

    def _handler(self, event):
        value = getattr(event, self._property)

        newEvent = core.StringEvent()
        newEvent.string = '%s:%s:%s' % (self._eventType,
                                        self._property,
                                        self._name)

        if self._critical > self._warning:
            self._checkMaximum(value, newEvent)
        else:
            self._checkMinimum(value, newEvent)

        self._lastValue = value

core.registerSubsystem('Monitor', Monitor)

class CPUData(object):
    def __init__(self, f, size):
        self._size = size
        self._file = f
        self._data = []

        if self._file is not None:
            self.appendData = self._appendData
            self.flushData = self._flushData
        else:
            self.appendData = self._pass
            self.flushData = self._pass

    def __del__(self):
        self.flushData(True)

    def _pass(self, *args, **kwargs):
        pass

    def _appendData(self, user, sys, idle):
        self._data.append((user, sys, idle))

    def _averageStats(self):
        userAvg, sysAvg, idleAvg = 0, 0, 0
        size = len(self._data)
        if size == 0:
            return userAvg, sysAvg, idleAvg

        for (u, s, i) in self._data:
            userAvg, sysAvg, idleAvg = userAvg + u,sysAvg + s,idleAvg + i
        return (userAvg / size, sysAvg / size, idleAvg / size)

    # Flushes data to disk if the amount of data is greater than the size
    def _flushData(self, force = False):
        if len(self._data) != 0 and (len(self._data) >= self._size or force):
            self._file.write('User: %%%5.2f Syst: %%%5.2f Idle: %%%5.2f\n' \
                                 % self._averageStats())
            self._data = []

class CPUMonitor(core.Subsystem):

    USR_UPDATE = core.declareEventType('USR_UPDATE')
    SYS_UPDATE = core.declareEventType('SYS_UPDATE')
    IDLE_UPDATE = core.declareEventType('IDLE_UPDATE')

    def __init__(self, cfg = None, deps = None):
        if deps is None:
            deps = []
        if cfg is None:
            cfg = {}

        core.Subsystem.__init__(self, cfg.get('name', 'CPUMonitor'), deps)

        self._qeventHub = \
            core.Subsystem.getSubsystemOfType(core.QueuedEventHub,
                                              deps, nonNone = True)

        self._config = cfg

        self._bufSize = cfg.get('bufferSize', 1)
        self._log = cfg.get('log_results', True)

        if self._log:
            # Generate log name
            timeStamp = datetime.fromtimestamp(timer.time())
            directory = timeStamp.strftime("%Y%m%d%H%M%S")
            
            # Create the directory, the directory name should never conflict
            # If it does, this should crash
            os.mkdir(directory)
            self._file = open(directory + '/cpu.log', 'w')
        else:
            self._file = None

        # Setup logfile
        self._data = CPUData(self._file, self._bufSize)

        # Initial values
        f = open('/proc/stat')
        self._start_values = f.readline().split()[1:]
        self._start_time = time.time()
        f.close()

    def unbackground(self, join = False):
        core.Subsystem.unbackground(self, join)

        # Flush the buffer and close the file
        del self._data
        if self._file is not None:
            self._file.close()

    def _createEvent(self, perc):
        event = math.NumericEvent()
        event.number = perc
        return event

    def update(self, timeStep):
        # Open /proc/stat to get CPU information
        f = open('/proc/stat')

        self._end_values = f.readline().split()[1:]
        self._end_time = time.time()
        f.close()

        # Determine the difference in ticks between two valeus
        start_sum = 0
        for i in self._start_values:
            start_sum += int(i)
            
        end_sum = 0
        for i in self._end_values:
            end_sum += int(i)
    
        total_diff = float(end_sum - start_sum)

        # Find the fractional difference of each section
        
        # Order of elements: user nice system idle
        # user: normal processes executing in user mode
        # nice: niced processes executing in user mode
        # system: processes executing in kernel mode
        # idle: twiddling thumbs 
        
        user_diff = (int(self._end_values[0]) - \
                         int(self._start_values[0])) * 100
        system_diff = (int(self._end_values[2]) - \
                           int(self._start_values[2])) * 100
        idle_diff = (int(self._end_values[3]) - \
                        int(self._start_values[3])) * 100

        # Generate usage percentages
        userPer, sysPer, idlePer = user_diff / total_diff, \
            system_diff / total_diff, idle_diff / total_diff

        # Publish updates
        self._qeventHub.publish(CPUMonitor.USR_UPDATE,
                                self._createEvent(userPer))
        self._qeventHub.publish(CPUMonitor.SYS_UPDATE,
                                self._createEvent(sysPer))
        self._qeventHub.publish(CPUMonitor.IDLE_UPDATE,
                                self._createEvent(idlePer))

        # Record data
        self._data.appendData(userPer, sysPer, idlePer)
        self._data.flushData()

        # Save end times as the new start time
        self._start_values = self._end_values
        self._start_time = self._end_time

core.registerSubsystem('CPUMonitor', CPUMonitor)
