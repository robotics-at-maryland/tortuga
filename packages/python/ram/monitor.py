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

        self._connections = []

        # Get essential subsystems
        self._qeventHub = core.Subsystem.getSubsystemOfType(core.QueuedEventHub,
                                                            deps,
                                                            nonNone = True)

        self._signals = {}

        if cfg.has_key('type'):
            cfg.pop('type')
        if cfg.has_key('depends_on'):
            cfg.pop('depends_on')
        cfg.pop('name')

        # Iterate through the config file subscribing to any events it lists
        for name, data in cfg.iteritems():
            publisher = self._lookupByName(data['publisher'], deps)
            self._signals[name] = Signal(self._qeventHub,
                                         data['eventType'],
                                         publisher,
                                         data['property'],
                                         data['name'],
                                         data['warning'],
                                         data['critical'])

    def _lookupByName(self, publisher, deps):
        # Check if the publisher is a device
        # Something different is done if it is
        if publisher.startswith('Vehicle.Device.'):
            deviceName = publisher[len('Vehicle.Device.'):]
            vehicle = core.Subsystem.getSubsystemOfType(
                ext.vehicle.IVehicle, deps)
            device = vehicle.getDevice(deviceName)
            return device
        else:
            # Normal operations for any other publisher
            # Parse the subsystem out of the publisher name
            systemName = publisher.split('.')[0]
            for d in deps:
                # Parse the subsystem name
                depName = d.getPublisherName().split('.')[-1]
                
                # Check if this is the subsystem needed
                if depName == systemName:
                    # Return the publisher
                    return d

    # Not used. May be used in the future.
    def _acquireDependencies(self, cfg, deps):
        """
        This is supposed to look into the systems that it monitors and
        see which subsystems they depend on. Then it would add those
        subsystems to its own list.

        It does not do that yet. I do not believe there is a possible way
        to do this yet.
        """
        # Create a set so that there are no duplicates
        systems = set()
        
        # Load the dependencies that were passed in
        if deps is not None:
            for d in deps:
                systems.add(d)

        # Iterate through the inner subsystems
        #for subsystem, config in cfg.iteritems():
            

        # Reload the dependencies into a list
        newDeps = []
        for d in systems:
            newDeps.append(d)

        # Return the new list
        return newDeps

class Signal(object):
    def __init__(self, qeventHub, eventType, publisher, propName, name,
                 warning, critical):
        self._qeventHub = qeventHub
        self._eventType = eventType
        self._publisher = publisher
        self._property = propName
        self._name = name
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
                                        self._property, self._name)

        if self._critical > self._warning:
            self._checkMaximum(value, newEvent)
        else:
            self._checkMinimum(value, newEvent)

        self._lastValue = value

core.registerSubsystem('Monitor', Monitor)
