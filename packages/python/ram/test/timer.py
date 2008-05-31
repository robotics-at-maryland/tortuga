# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/time.py

# STD Imports
import unittest
import time as time
import math

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
    def replaceNanosleep(self):
        if not self.nanoSleepMocked:
            self.origNanosleep = timer.nanosleep
            timer.nanosleep = self.mockNanosleep
            self.nanoSleepMocked = True
        else:
            timer.nanosleep = self.origNanosleep
            self.nanoSleepMocked = False
    
    def mockNanosleep(self, sec, nsec):
        self.sec = sec
        self.nsec = nsec

        self.secs.append(sec)
        self.nsecs.append(nsec)
        
        # Busy loop if desired
        while self.timerBlock:
            pass

    def checkRepeat(self, count, interval):
        sec = math.floor(interval)
        secFloat = interval - sec
        
        for i in xrange(0, count):
            while self.count == i:
                self.origNanosleep(0, int(1e6))
            
            self.assertEquals(sec, self.secs[i])
            self.assertEquals(secFloat * 1e9, self.nsecs[i])

    def setUp(self):
        self.nanoSleepMocked = False
        # Replace nanosleep with out Mock sleep function
        self.replaceNanosleep()
        self.timerBlock = False

        self.sec = None
        self.nsec = None
        self.secs = []
        self.nsecs = []
        
    def tearDown(self):
        # Put back the original function
        self.replaceNanosleep()
        self.timerBlock = False


class TestTimer(TimerTester):
    TIMER_EVENT = core.declareEventType('TIMER_EVENT')
    
    def setUp(self):
        TimerTester.setUp(self)
        # Set up publisher and event handler
        self.epub = core.EventPublisher()
        self.epub.subscribe(TestTimer.TIMER_EVENT, self.handleTimer)
        self.event = None
        self.count = 0
        
    def tearDown(self):
        TimerTester.tearDown(self)
        self.epub = None
        self.event = None
        self.count = None
    
    def handleTimer(self, event):
        self.count += 1
        self.event = event
        self.endTime = timer.time()
    
    def checkTimer(self, seconds, eventType = None):
        sec = math.floor(seconds)
        secFloat = seconds - sec
    
        self.assertEquals(sec, self.sec)
        self.assertEquals(secFloat * 1e9, self.nsec)
        
        if eventType is None:
            self.assertEquals(None, self.event)
        else:
            self.assertEquals(eventType, self.event.type)
    
    def testStart(self):
        # Create time and record the current time
        newTimer = timer.Timer(self.epub, TestTimer.TIMER_EVENT, 0.25)
        
        # Start the timer and sleep to wait for it to complete, then check
        newTimer.start()
        newTimer.join()
        
        self.checkTimer(0.25, TestTimer.TIMER_EVENT)
        self.assertEqual(1, self.count)
    
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
        
        self.checkTimer(0.25)
        
    def testRepeat(self):
        newTimer = timer.Timer(self.epub, TestTimer.TIMER_EVENT, 0.1,
                               repeat = True)
        newTimer.start()
        
        self.checkRepeat(5, 0.1)

        newTimer.stop()
        newTimer.join()
        
        self.assertEqual(TestTimer.TIMER_EVENT, self.event.type)

                
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
