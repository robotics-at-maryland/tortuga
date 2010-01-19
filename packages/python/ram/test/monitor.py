# Copyright (C) 2010 Maryland Robotics Club
# Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
# All rights reserved.
#
# Author: Jonathan Sternberg <jsternbe@umd.edu>
# File: packages/python/ram/test/monitor.py

# STD Imports
import unittest

# Project Imports
import ext.core as core
import ext.math as math
import ram.monitor as monitor

class TestMonitor(unittest.TestCase):

    # Test event
    TESTEVENT = core.declareEventType('TESTEVENT')

    def setUp(self):
        # Need a QueuedEventHub
        self.eventHub = core.EventHub()
        self.qeventHub = core.QueuedEventHub(self.eventHub)

        cfg = {
            'Monitor' : {
                'Systems' : {
                    'Test' : {
                        'eventType' : 'ram.test.monitor.TestMonitor.TESTEVENT',
                        'property' : 'number',
                        'name' : 'testing123',
                        'warning' : 5,
                        'critical' : 10
                        }
                    }
                }
            }
        self.monitor = monitor.Monitor(cfg = cfg.get('Monitor', {}),
                                       deps = [self.qeventHub])

    def testConfig(self):
        # Check if the signal is there
        self.assert_(self.monitor._signals.has_key('Test'))
        
        # Make sure that all of the fields got filled out correctly
        self.assertEqual('ram.test.monitor.TestMonitor.TESTEVENT',
                         self.monitor._signals['Test']._eventType)
        self.assertEqual('number', self.monitor._signals['Test']._property)
        self.assertEqual('testing123', self.monitor._signals['Test']._name)
        self.assertEqual(5, self.monitor._signals['Test']._warning)
        self.assertEqual(10, self.monitor._signals['Test']._critical)

    def testWarning(self):
        # Subscribe to the warning event
        self._string = ""
        def _handler(event):
            self._string = event.string

        self.qeventHub.subscribeToType(monitor.Monitor.WARNING, _handler)

        # Publish an event that does not reach the warning value
        event = math.NumericEvent()
        event.number = 4
        self.qeventHub.publish(TestMonitor.TESTEVENT, event)
        self.qeventHub.publishEvents()

        self.assertEqual("", self._string)

        # Publish the event with the warning value
        event = math.NumericEvent()
        event.number = 5
        self.qeventHub.publish(TestMonitor.TESTEVENT, event)
        self.qeventHub.publishEvents()

        self.assertEqual("ram.test.monitor.TestMonitor.TESTEVENT:"
                         "number:testing123", self._string)

    def testCritical(self):
        # Subscribe to the warning event
        self._string = ""
        def _critical_handler(event):
            self._string = event.string

        self._called = False
        def _warning_handler(event):
            self._called = True

        self.qeventHub.subscribeToType(monitor.Monitor.WARNING,
                                       _warning_handler)
        self.qeventHub.subscribeToType(monitor.Monitor.CRITICAL,
                                       _critical_handler)

        # Publish an event that does not reach the warning value
        event = math.NumericEvent()
        event.number = 2
        self.qeventHub.publish(TestMonitor.TESTEVENT, event)
        self.qeventHub.publishEvents()

        self.assertEqual("", self._string)

        # Publish the event with the critical value
        # Critical value should outprioritize the warning value
        event = math.NumericEvent()
        event.number = 10
        self.qeventHub.publish(TestMonitor.TESTEVENT, event)
        self.qeventHub.publishEvents()

        self.assertEqual("ram.test.monitor.TestMonitor.TESTEVENT:"
                         "number:testing123", self._string)

    def testNominal(self):
        # Set the monitor to be at critical value
        self.monitor._signals['Test']._lastValue = 10
        
        self._string = ""
        def _nominal_handler(event):
            self._string = event.string

        self.qeventHub.subscribeToType(monitor.Monitor.NOMINAL,
                                       _nominal_handler)

        # Set the system back to a non-warning value
        event = math.NumericEvent()
        event.number = 3
        self.qeventHub.publish(TestMonitor.TESTEVENT, event)
        self.qeventHub.publishEvents()

        self.assertEqual("ram.test.monitor.TestMonitor.TESTEVENT:"
                         "number:testing123", self._string)
