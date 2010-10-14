# Copyright (C) 2010 Maryland Robotics Club
# Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
# All rights reserved.
#
# Author: Jonathan Sternberg <jsternbe@umd.edu>
# File:  sandbox/statemachine/runner.py

import ext.core as core
import ram.ai.state as state

FINISH = core.declareEventType('FINISH')

import string
for letter in set(string.letters.upper()):
    globals()[letter] = core.declareEventType(letter)

def runMachine(start):
    eventHub = core.EventHub()
    qeventHub = core.QueuedEventHub(eventHub)

    subsystems = core.SubsystemList()
    subsystems.append(qeventHub)
    machine = state.Machine(deps = subsystems)

    sequence = raw_input('Enter sequence: ')

    machine.start(start)
    assert machine.currentState() is not None, 'State machine not started'
    for x in sequence:
        eventType = globals()[x.upper()]
        qeventHub.publish(eventType, core.Event())
        qeventHub.publishEvents()

        assert machine.currentState() is not None, 'State machine ended prematurely'

    # State machine finished, send finish event
    qeventHub.publish(FINISH, core.Event())
    qeventHub.publishEvents()

    assert machine.currentState() is None, 'State machine has not ended'
    machine.stop()
