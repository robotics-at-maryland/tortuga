# This file is for initializing the shell in the OCI with default commands.
#
# All of the commands in this file are run as if they were typed into an
# interpreter by a human. This is for generic code that should be initialized
# in the shell everytime the OCI is run.
#
# When entering new code, remember that it is run as if the code was typed
# directly into the interpreter. This code will cause an error:
#
# def HelloWorld():
#     print "hello,",
#
#     print "world!"
#
# Instead, remove any spaces.
#
# def HelloWorld():
#     print "hello,",
#     print "world!"
#
# If you want to add a blankline for clarity, use a comment:
#
# def HelloWorld():
#     print "hello,",
#     # We're saying hello to the world right now
#     print "world!"
#
# This block is not in """ Text Here """ comments because these are printed out
# in the OCI. Only use comments with the '#' symbol.

import math as pmath
import ext.core as core
import ext.math as math
import ram.ai.light as light
import ram.ai.pipe as pipe
import ram.ai.bin as bin
import ram.ai.barbedwire as bwire
import ram.ai.target as target
import ram.ai.course as course
import ram.motion as motion
import ram.motion.basic as basic
import ram.motion.search as search
import ram.timer as timer
from datetime import datetime
from pprint import pprint

# Helper methods
def diveTo(depth, speed = 0.3):
    motionManager.setMotion(basic.RateChangeDepth(depth, speed))

# dive and diveTo are the same function.
dive = diveTo

def up(depthChange, speed = 0.3):
    diveTo(vehicle.getDepth() - depthChange, speed = speed)

def down(depthChange, speed = 0.3):
    diveTo(vehicle.getDepth() + depthChange, speed = speed)

# Surface depth to 0.3 because 0.0 causes the vehicle to always try and get
# above the water
def surface(speed = 0.3):
    diveTo(depth = 0.3, speed = speed)

def yaw(yawChange, speed = 30, absolute = False):
    motionManager.setMotion(basic.RateChangeHeading(yawChange, speed, absolute = absolute))

def yawTo(yawChange, speed = 30):
    yaw(yawChange, speed, absolute = True)

yaw2 = yawTo

def allStop():
    stateMachine.stop()
    motionManager.stopCurrentMotion()
    controller.setSpeed(0)
    controller.setSidewaysSpeed(0)
    controller.holdCurrentDepth()
    controller.holdCurrentHeading()

s = allStop
stop = allStop

def start(state):
    allStop()
    stateMachine.start(state)

# Keeps track of the forward and downward streams.
# You should never be required to access the RecorderManager.
# Instead, use the utility functions included after the RecorderManager.
class RecorderManager(object):
    recorders = {}
    # Generic add function
    @staticmethod
    def _addstream(func, port, size = (320, 240), rate = 5):
        # Check if the port is being used
        for name in RecorderManager.recorders.iterkeys():
            if name.find(str(port)) != -1:
                # Do nothing if we find the port in use
                return
        path = str(port) + "(" + str(size[0]) + "," + str(size[1]) + ")"
        func(path, rate)
        RecorderManager.recorders[path] = path
    # Generic function to remove a stream
    @staticmethod
    def _removestream(func, port):
        # Check all the current streams to find the port in any names
        for name in RecorderManager.recorders.iterkeys():
            if name.find(str(port)) != -1:
                # When found, remove it
                func(name)
                return
    @staticmethod
    def fstream(port = 50000, size = (320, 240), rate = 5):
        RecorderManager._addstream(visionSystem.addForwardRecorder,
                                   port, size, rate)
    @staticmethod
    def dstream(port = 50001, size = (320, 240), rate = 5):
        RecorderManager._addstream(visionSystem.addDownwardRecorder,
                                   port, size, rate)
    @staticmethod
    def removefs(port = 50000):
        RecorderManager._removestream(visionSystem.removeForwardRecorder,
                                      port)
    @staticmethod
    def removeds(port = 50001):
        RecorderManager._removestream(visionSystem.removeDownwardRecorder,
                                      port)

# Use these functions to interact with the RecorderManager.
def fstream(port = 50000, size = (320, 240), rate = 5):
    RecorderManager.fstream(port, size, rate)

def dstream(port = 50001, size = (320, 240), rate = 5):
    RecorderManager.dstream(port, size, rate)

def removefs(port = 50000):
    RecorderManager.removefs(port)

def removeds(port = 50001):
    RecorderManager.removeds(port)

fsremove = removefs
dsremove = removeds

# Quick detector functions
lightOn = visionSystem.redLightDetectorOn
lightOff = visionSystem.redLightDetectorOff
pipeOn = visionSystem.pipeLineDetectorOn
pipeOff = visionSystem.pipeLineDetectorOff
bwireOn = visionSystem.barbedWireDetectorOn
bwireOff = visionSystem.barbedWireDetectorOff
binOn = visionSystem.binDetectorOn
binOff = visionSystem.binDetectorOff
targetOn = visionSystem.targetDetectorOn
targetOff = visionSystem.targetDetectorOff

# This is the helper function for takeXClip. Don't use it.
def recordClip(addRecorder, removeRecorder, seconds, name, extension, rate):
    # All comments are needed. Do not add blank lines!
    TIMEOUT = core.declareEventType('TIMEOUT')
    conn = None
    def stop(event):
        removeRecorder(name + extension)
        conn.disconnect()
    conn = queuedEventHub.subscribeToType(TIMEOUT, stop)
    # If no name was given, create one out of the time
    if name is None:
        timeStamp = datetime.fromtimestamp(timer.time())
        name = timeStamp.strftime("%Y%m%d%H%M%S")
    # Add the forward recorder
    addRecorder(name + extension, rate)
    # Create the timer
    clipTimer = timerManager.newTimer(TIMEOUT, seconds)
    clipTimer.start()

def takeFClip(seconds, name = None, extension = ".rmv", rate = 5):
    recordClip(visionSystem.addForwardRecorder, visionSystem.removeForwardRecorder, seconds, name, extension, rate)

def takeDClip(seconds, name = None, extension = ".rmv", rate = 5):
    recordClip(visionSystem.addDownwardRecorder, visionSystem.removeDownwardRecorder, seconds, name, extension, rate)

# End of file (REQUIRED! DO NOT DELETE!)
