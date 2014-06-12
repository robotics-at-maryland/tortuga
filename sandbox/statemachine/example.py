# Copyright (C) 2010 Maryland Robotics Club
# Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
# All rights reserved.
#
# Author: Jonathan Sternberg <jsternbe@umd.edu>
# File:  sandbox/statemachine/example.py

from runner import *
import ram.ai.state as state

"""
Note: Events for every letter in the english language A-Z are automatically
created by calling:

from runner import *
"""

class Start(state.State):
    @staticmethod
    def transitions():
        """
        I have specified no transition for event B. If no transition
        is specified, the event will be ignored and no transitions will
        take place.
        """
        return { A : Middle }

    def enter(self):
        print 'start of the state machine'

    def exit(self):
        print "received event 'A' and exiting the starting state"

class Middle(state.State):
    @staticmethod
    def transitions():
        return { A : Finish,
                 B : Middle }

    def B(self, event):
        print 'this is an example of a loopback'

class Finish(state.State):
    """
    This isn't the real finish, but we're going to play pretend.

    In these examples, a finish state should contain a transition for
    runner.FINISH. The runner code will automatically publish this event
    after it's processed all other tokens.
    """
    @staticmethod
    def transitions():
        return { FINISH : End }

class End(state.State):
    """
    In our implementation, we automatically stop the state machine when it
    reaches a state that has no transitions.

    No states in your implementation should ever lead to this state unless
    runner.FINISH is the event (as in the Finish state above).
    """
    def enter(self):
        print 'done'

if __name__ == '__main__':
    """
    Main functions don't exist in python, but this is the proper way
    to write one.

    All this has to do is call runMachine with the name of the state
    you want to start at.
    """
    runMachine(Start)
