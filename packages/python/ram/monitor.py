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
        # TODO: I hate you Joe
        # In the future, wrap the lookupByName function in
        # EventPublisher.h
        core.Subsystem.__init__(self, cfg.get('name', 'Monitor'), deps)

        self._connections = []

        # Get essential subsystems
        self._qeventHub = core.Subsystem.getSubsystemOfType(core.QueuedEventHub,
                                                            deps,
                                                            nonNone = True)

        self._signals = {}

        # Iterate through the config file subscribing to any events it lists
        for name, data in cfg.iteritems():
            if name != "type" and name != "name" and name != "depends_on":
                publisher = self._lookupByName(data['publisher'], deps)
                self._signals[name] = Signal(self._qeventHub,
                                             data['eventType'],
                                             publisher,
                                             data['property'],
                                             data['display'],
                                             data['warning'],
                                             data['critical'])

    def _lookupByName(self, publisher, deps):
        for d in deps:
            if publisher.startswith('Vehicle.Device.'):
                deviceName = publisher[len('Vehicle.Device.'):]
                vehicle = core.Subsystem.getSubsystemOfType(
                    ext.vehicle.IVehicle, deps)
                device = vehicle.getDevice(deviceName)
                return device
            else:
                if d.getPublisherName() == publisher:
                    print d, type(d)
                    return d

class Signal(object):
    def __init__(self, qeventHub, eventType, publisher, propName, display,
                 warning, critical):
        self._qeventHub = qeventHub
        self._eventType = eventType
        self._publisher = publisher
        self._property = propName
        self._display = display
        self._warning = warning
        self._critical = critical
        self._lastValue = 0

        eventType = resolve(self._eventType)

        # publisher = core.lookupByName(self._publisher)

        self._qeventHub.subscribe(eventType, publisher, self._handler)

    def _checkMaximum(self, event):
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
        newEvent.string = '%s:%s:%s:%s' % (self._eventType, self._publisher,
                                        self._property, self._display)

        if self._critical > self._warning:
            self._checkMaximum(value, newEvent)
        else:
            self._checkMinimum(value, newEvent)

        self._lastValue = value

core.registerSubsystem('Monitor', Monitor)
