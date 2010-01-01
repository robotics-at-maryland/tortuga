# Copyright (C) 2009 Maryland Robotics Club
# Copyright (C) 2009 Jonathan Sternberg <jsternbe@umd.edu>
# All rights reserved.
#
# Author: Jonathan Sternberg <jsternbe@umd.edu>
# File: packages/python/ram/monitor.py

# Project imports
import ext.core as core
import ext.vehicle
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
        self._qeventHub = core.Subsystem.getSubsystemOfType(core.QueuedEventHub,
                                                            deps,
                                                            nonNone = True)

        self._signals = {}

        # Clear config file of extra values
        if cfg.has_key('type'):
            cfg.pop('type')
        if cfg.has_key('depends_on'):
            cfg.pop('depends_on')
        cfg.pop('name')

        # Iterate through the config file subscribing to any events it lists
        for name, data in cfg.iteritems():
            self._signals[name] = Signal(self._qeventHub,
                                         data['eventType'],
                                         data['property'],
                                         data['name'],
                                         data['warning'],
                                         data['critical'])

    def __del__(self):
        # Delete all signals
        for signal in self._signals.itervalues():
            del signal

    #def backgrounded(self):
    #    return True

class Signal(object):
    def __init__(self, qeventHub, eventType, propName, name,
                 warning, critical):
        self._qeventHub = qeventHub
        self._eventType = eventType
        self._property = propName
        self._name = name
        self._warning = warning
        self._critical = critical
        self._lastValue = 0

        eventType = resolve(self._eventType)

        self._conn = self._qeventHub.subscribeToType(eventType, self._handler)

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
