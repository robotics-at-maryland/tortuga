# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# nanosleep code: Copyright (c) 2006 Lovely Systems and Contributors. (ZPL)
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/time.py

"""
This module contains class for Timers, a TimerManager and wrappers for sleeps
and getting the current time.
"""

# STD Imports
import os
import ctypes
import threading
import math
import time as _time

# Project Imports
import ext.core

# Begin nanosleep code 
# Under the ZPL, Please see the license/ZPL.txt file for more information.
# Changes: by Joseph Lisee on Jan 20, 2008

try:    
    # Linux
    try:
        _libc = ctypes.CDLL("libc.so.6")
    except OSError:
        _libc = None
    if _libc is None:
        # MAC OS-X
        try:
            _libc = ctypes.CDLL("libc.dylib", ctypes.RTLD_GLOBAL)
        except OSError:
            raise ImportError

    # Define the timespec structure in python
    class _TIMESPEC(ctypes.Structure):
        _fields_ = [('secs', ctypes.c_long),
                    ('nsecs', ctypes.c_long),
                   ]

    _libc.nanosleep.argtypes = \
            [ctypes.POINTER(_TIMESPEC), ctypes.POINTER(_TIMESPEC)]


    def nanosleep(sec, nsec):
        sleeptime = _TIMESPEC()
        sleeptime.secs = sec
        sleeptime.nsecs = nsec
        remaining = _TIMESPEC()
        _libc.nanosleep(sleeptime, remaining)
        return (remaining.secs, remaining.nsecs)

except ImportError:
    # if ctypes is not available or no reasonable library is found we provide
    # a dummy which uses time.sleep

    def nanosleep(sec, nsec):
        _time.sleep(sec + (nsec * 0.000000001))
        
# End nanosleep code

def sleep(seconds):
    """
    Sleeps the current thread the given number of seconds useing nanosleep
    
    @type  seconds: float 
    @param seconds: The number of seconds to sleep
    """
    
    # Round down to our seconds
    secs = math.floor(float(seconds))
    # Convert the remainder to nano seconds
    nsecs = (seconds - secs) * 1e9;
    
    nanosleep(long(secs), long(nsecs))

def time():
    """
    Returns the time since program start
    
    Due to some odd platform differences different time module functions 
    have different accuracies, on different platforms.  The function takes
    that into account.
    
    @rtype:  double
    @return: Seconds since program start
    """
    # This is most accuracte on Linux and Mac
    if 'posix' == os.name:
        return _time.time()
    # This on on Windows
    else:
        return _time.clock()

class Timer(threading.Thread):
    """
    Throws event after given duration sleep in a background thread
    """
    
    def __init__(self, eventPublisher, eventType, duration):
        """
        @type  eventPublisher: ext.core.EventPublisher
        @param eventPublisher: Publisher to publish the event with
        
        @type  eventType: str
        @param eventType: The type of event to publish
        
        @type  duration: float
        @param duration: The seconds to sleep
        """
        threading.Thread.__init__(self)
        
        self._eventPublisher = eventPublisher
        self._eventType = eventType
        self._sleepTime = duration
        self._running = True
        
    def run(self):
        """
        Fires of the event from a background thread after the needed sleep

        This is implements the standard python threading.Thread method.
        """
        # Sleep for that time period
        sleep(self._sleepTime)

        # Publish event
        self._complete()

        # Set running to false
        self.stop()
 
    def stop(self):
        """
        Stops the backgruond thread from publishing its event when it wakes up
        """
        self._running = False
        
    def _complete(self):
        """
        Publishes the desired event, called in background thread after the sleep
        """
        # Publish event
        if self._running:
            self._eventPublisher.publish(self._eventType, ext.core.Event())
        
class TimerManager(ext.core.Subsystem):
    def __init__(self, config = None, deps = None):
        if config is None:
            config = {}
        
        # Require EventHub, because the main client of this class, the AI, only
        # gets events if the hub is present
        ext.core.Subsystem.getSubsystemOfExactType(ext.core.EventHub, deps, 
                                                   nonNone = True)
        ext.core.Subsystem.__init__(self, config.get('name', 'TimerManager'), 
                                    deps)
        
    def newTimer(self, eventType, duration):
        return Timer(self, eventType, duration)


ext.core.registerSubsystem('TimerManager', TimerManager)
