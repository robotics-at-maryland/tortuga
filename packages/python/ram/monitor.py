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
from types import *

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
        if cfg is None:
            cfg = {}
        if deps is None:
            deps = []

        core.Subsystem.__init__(self, cfg.get('name', 'Monitor'), deps)

        # Get essential subsystems
        self._qeventHub = \
            core.Subsystem.getSubsystemOfType(core.QueuedEventHub,
                                              deps, nonNone = True)

        self._signals = {}

        sysCfg = cfg.get('Systems', {})

        # Iterate through the config file subscribing to any events it lists
        for name, data in sysCfg.iteritems():
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
            if value >= self._critical and self._lastValue < self._critical:
                # Publish a critical event
                self._qeventHub.publish(Monitor.CRITICAL, event)
            elif value >= self._warning and self._lastValue < self._warning:
                # Publish a warning event
                self._qeventHub.publish(Monitor.WARNING, event)
        else:
            if value < self._warning and self._lastValue >= self._warning:
                # Publish a nominal event
                self._qeventHub.publish(Monitor.NOMINAL, event)
            elif value < self._critical and self._lastValue >= self._critical:
                # Publish a warning event
                self._qeventHub.publish(Monitor.WARNING, event)

    def _checkMinimum(self, value, event):
        if value <= self._lastValue:
            if value <= self._critical and self._lastValue > self._critical:
                # Publish a critical event
                self._qeventHub.publish(Monitor.CRITICAL, event)
            elif value <= self._warning and self._lastValue > self._warning:
                # Publish a warning event
                self._qeventHub.publish(Monitor.WARNING, event)
        else:
            if value > self._warning and self._lastValue <= self._warning:
                # Publish a nominal event
                self._qeventHub.publish(Monitor.NOMINAL, event)
            elif value > self._critical and self._lastValue <= self._critical:
                # Publish a warning event
                self._qeventHub.publish(Monitor.WARNING, event)

    def _handler(self, event):
        value = getattr(event, self._property)

        newEvent = core.StringEvent()
        newEvent.string = '%s:%s:%s:%s' % (self._eventType,
                                           self._property,
                                           self._name,
                                           value)

        if self._critical > self._warning:
            self._checkMaximum(value, newEvent)
        else:
            self._checkMinimum(value, newEvent)

        self._lastValue = value

core.registerSubsystem('Monitor', Monitor)

class CpuData(object):
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

    def _appendData(self, arr):
        self._data.append((arr[0], arr[1], arr[2], arr[3],
                           arr[4], arr[5], arr[6]))

    def _averageStats(self):
        userAvg, niceAvg, sysAvg, idleAvg, iowtAvg, irqAvg, sirqAvg = \
            0, 0, 0, 0, 0, 0, 0
        size = len(self._data)
        if size == 0:
            return userAvg, niceAvg, sysAvg, idleAvg, iowtAvg, irqAvg, sirqAvg

        for (u, n, s, i, wt, irq, sirq) in self._data:
            userAvg, niceAvg, sysAvg, idleAvg, iowtAvg, irqAvg, sirqAvg = \
                userAvg + u, niceAvg + n, sysAvg + s, idleAvg + i, \
                iowtAvg + wt, irqAvg + irq, sirqAvg + sirq
        return (userAvg / size, niceAvg / size, sysAvg / size, idleAvg / size,
                iowtAvg / size, irqAvg / size, sirqAvg / size)

    # Flushes data to disk if the amount of data is greater than the size
    def _flushData(self, force = False):
        if len(self._data) != 0 and (len(self._data) >= self._size or force):
            self._file.write('User: %%%5.2f Nice: %%%5.2f Syst: %%%5.2f '
                             'Idle: %%%5.2f IOWt: %%%5.2f IRQ: %%%5.2f '
                             'SIRQ: %%%5.2f\n' % self._averageStats())
            self._data = []

class CpuMonitor(core.Subsystem):

    USER_UPDATE = core.declareEventType('USER_UPDATE')
    NICE_UPDATE = core.declareEventType('NICE_UPDATE')
    SYS_UPDATE  = core.declareEventType('SYS_UPDATE' )
    IDLE_UPDATE = core.declareEventType('IDLE_UPDATE')
    IOWT_UPDATE = core.declareEventType('IOWT_UPDATE')
    IRQ_UPDATE  = core.declareEventType('IRQ_UPDATE' )
    SIRQ_UPDATE = core.declareEventType('SIRQ_UPDATE')

    def __init__(self, cfg = None, deps = None):
        if deps is None:
            deps = []
        if cfg is None:
            cfg = {}

        core.Subsystem.__init__(self, cfg.get('name', 'CpuMonitor'), deps)

        self._qeventHub = \
            core.Subsystem.getSubsystemOfType(core.QueuedEventHub,
                                              deps, nonNone = True)

        self._config = cfg

        self._bufSize = cfg.get('bufferSize', 1)
        logName = cfg.get('log_name', None)
        self._log = cfg.get('log_results', True)

        if self._log:
            if logName is None:
                # Generate log name
                timeStamp = datetime.fromtimestamp(timer.time())
                directory = timeStamp.strftime("%Y%m%d%H%M%S")
                
                # Create the directory, the directory name
                # should never conflict. If it does, this should crash
                os.mkdir(directory)
                self._file = open(directory + '/cpu.log', 'w')
            else:
                # A specified file must have the directory exist
                self._file = open(logName, 'w')
        else:
            self._file = None

        # Setup logfile
        self._data = CpuData(self._file, self._bufSize)

        # Initial values
        f = open('/proc/stat')
        self._start_values = f.readline().split()[1:]
        self._start_time = time.time()
        f.close()

        # Pack the update types into an array
        self._updateTypes = [ CpuMonitor.USER_UPDATE,
                              CpuMonitor.NICE_UPDATE,
                              CpuMonitor.SYS_UPDATE,
                              CpuMonitor.IDLE_UPDATE,
                              CpuMonitor.IOWT_UPDATE,
                              CpuMonitor.IRQ_UPDATE,
                              CpuMonitor.SIRQ_UPDATE ]

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
        
        perc = range(7)
        # Generate percentages and publish them
        for x in xrange(7):
            diff = (int(self._end_values[x]) - \
                        int(self._start_values[x])) * 100
            perc[x] = diff / total_diff
            self._qeventHub.publish(self._updateTypes[x],
                                    self._createEvent(perc[x]))

        # Record data
        self._data.appendData(perc)
        self._data.flushData()

        # Save end times as the new start time
        self._start_values = self._end_values
        self._start_time = self._end_time

core.registerSubsystem('CpuMonitor', CpuMonitor)
