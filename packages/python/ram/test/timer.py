# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/time.py

# STD Imports
import unittest
import time as time

# Project Imports
import ext.core as core

# Module to Test
import ram.timer as timer

class TestSleepFuncs(unittest.TestCase):
    def testNanosleep(self):
        startTime = timer.time()
        timer.nanosleep(0, long(0.25 * 1e9))
        endTime = timer.time()
        
        self.assertAlmostEquals(0.25, endTime - startTime, 1)
        
    def mockNanosleep(self, sec, nsec):
        self.sec = sec
        self.nsec = nsec
        
    def testSleep(self):
        # Mock Nanosleep so we don't really have to sleep
        originalNanosleep = timer.nanosleep
        timer.nanosleep = self.mockNanosleep
        
        startTime = timer.time()
        timer.sleep(0.25)
        endTime = timer.time()
        
        timer.nanosleep = originalNanosleep
        self.assertEquals(0, self.sec)
        self.assertEquals(0.25 * 1e9, self.nsec)
        

class TimerTester(unittest.TestCase):
    def mockNanosleep(self, sec, nsec):
        self.sec = sec
        self.nsec = nsec
        
        # Busy loop if desired
        while self.timerBlock:
            pass

    def setUp(self):
        # Replace nanosleep with out Mock sleep function
        self.origNanosleep = timer.nanosleep
        timer.nanosleep = self.mockNanosleep
        self.timerBlock = False
        
    def tearDown(self):
        # Put back the original function
        timer.nanosleep = self.origNanosleep
        self.timerBlock = False


class TestTimer(TimerTester):
    TIMER_EVENT = core.declareEventType('TIMER_EVENT')
    
    def setUp(self):
        TimerTester.setUp(self)
        # Set up publisher and event handler
        self.epub = core.EventPublisher()
        self.epub.subscribe(TestTimer.TIMER_EVENT, self.handleTimer)
        self.event = None
        
    def tearDown(self):
        TimerTester.tearDown(self)
        self.epub = None
        self.event = None
    
    def handleTimer(self, event):
        self.event = event
        self.endTime = timer.time()
    
    def testStart(self):
        # Create time and record the current time
        newTimer = timer.Timer(self.epub, TestTimer.TIMER_EVENT, 0.25)
        
        # Start the timer and sleep to wait for it to complete, then check
        newTimer.start()
        newTimer.join()
        
        self.assertEquals(0, self.sec)
        self.assertEquals(0.25 * 1e9, self.nsec)
        self.assertEquals(TestTimer.TIMER_EVENT, self.event.type)
    
    def testStop(self):        
        # Create time and record the current time
        newTimer = timer.Timer(self.epub, TestTimer.TIMER_EVENT, 0.25)
        
        # Start the timer in a busy background loop
        self.timerBlock = True
        newTimer.start()
        
        # Stop then kill background loop
        newTimer.stop()
        self.timerBlock = False
        newTimer.join()
        
        self.assertEquals(0, self.sec)
        self.assertEquals(0.25 * 1e9, self.nsec)
        self.assertEquals(None, self.event)
        
class TestTimerManager(TimerTester):
    def handleTimer(self, event):
        self.event = event
        self.endTime = timer.time()
    
    def testNewTimer(self):
        # Set up publisher and event handler
        timerManager = timer.TimerManager(deps = [core.EventHub()])
        timerManager.subscribe(TestTimer.TIMER_EVENT, self.handleTimer)
        
        # Create times
        newTimer = timerManager.newTimer(TestTimer.TIMER_EVENT, 0.25)
        
        # Start the timer and sleep to wait for it to complete, then check
        newTimer.start()
        time.sleep(0.1)
        self.assertEquals(0, self.sec)
        self.assertEquals(0.25 * 1e9, self.nsec)
        self.assertEquals(TestTimer.TIMER_EVENT, self.event.type)
        
    def testQueuedEvents(self):
        self.event = None
        eventHub = core.EventHub()
        qeventHub = core.QueuedEventHub(eventHub)
        timerManager = timer.TimerManager(deps = [eventHub])
        
        # Subscribe event queue
        qeventHub.subscribeToType(TestTimer.TIMER_EVENT, self.handleTimer)
        
        # Create times
        newTimer = timerManager.newTimer(TestTimer.TIMER_EVENT, 0.25)
        
        # Start the timer and sleep to wait for it to complete, then check
        newTimer.start()
        time.sleep(0.1)
        self.assertEquals(0, self.sec)
        self.assertEquals(0.25 * 1e9, self.nsec)
        self.assertEquals(None, self.event)
        
        # Now release the events
        qeventHub.publishEvents()
        self.assertEquals(TestTimer.TIMER_EVENT, self.event.type)
        

if __name__ == '__main__':
    unittest.main()