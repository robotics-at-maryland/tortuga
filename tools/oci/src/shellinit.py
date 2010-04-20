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
import ram.ai.gen2.light as light2
import ram.ai.pipe as pipe
import ram.ai.bin as bin
import ram.ai.barbedwire as bwire
import ram.ai.target as target
import ram.ai.hedge as hedge
import ram.ai.course as course
import ram.ai.gen2.course as course2
import ram.motion as motion
import ram.motion.basic as basic
import ram.motion.search as search
import ram.timer as timer
from datetime import datetime
from pprint import pprint

vars = dir()
# this will print an error message for any function fname
# that requires the component cname that has not been loaded
def error_function(fname, cname):
    def error():
        raise Exception (fname + " is not defined because " + cname + " is not loaded.")
    return error

# Helper methods
diveTo = error_function("diveTo", "motionManager")
if('motionManager' in vars):
    def diveTo_helper(depth, speed = 0.3):
        motionManager.setMotion(basic.RateChangeDepth(depth, speed))
    diveTo = diveTo_helper

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

yaw = error_function("yaw", "motionManager")
if('motionManager' in vars):
    def yaw_helper(yawChange, speed = 30, absolute = False):
        motionManager.setMotion(basic.RateChangeHeading(yawChange, speed, absolute = absolute))
    yaw = yaw_helper

yawTo = error_function("yawTo", "motionManager")
if('motionManager' in vars):
    def yawTo_helper(yawChange, speed = 30):
        yaw(yawChange, speed, absolute = True)
    yawTo = yawTo_helper

yaw2 = yawTo

allStop = error_function("allStop", "motionManager/controller/stateMachine")
if(('motionManager' in vars) and ('controller' in vars) and ('stateMachine' in vars)):
    def allStop_helper():
        stateMachine.stop()
        motionManager.stopCurrentMotion()
        controller.setSpeed(0)
        controller.setSidewaysSpeed(0)
        controller.holdCurrentDepth()
        controller.holdCurrentHeading()
    allStop = allStop_helper

s = allStop
stop = allStop

start = error_function("start", "stateMachine")
if('stateMachine' in vars):
    def start_helper(state):
        allStop()
        stateMachine.start(state)
    start = start_helper

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

# make error functions for all of them
lightOn = error_function("lightOn", "visionSystem")
lightOff = error_function("lightOff", "visionSystem")
pipeOn = error_function("lightOn", "visionSystem")
pipeOff = error_function("pipeOff", "visionSystem")
bwireOn = error_function("bwireOn", "visionSystem")
bwireOff = error_function("bwireOff", "visionSystem")
binOn = error_function("binOn", "visionSystem")
binOff = error_function("binOff", "visionSystem")
targetOn = error_function("targetOn", "visionSystem")
targetOff = error_function("targetOff", "visionSystem")

# set them all to the real thing if the vision system is present
if('visionSystem' in vars):
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

def recordClipError(*args, **kwargs):
    raise Exception ("Record clip cannot be used with IdealSimVision")

# This is the helper function for takeXClip. Don't use it.
def recordClipImpl(addRecorder, removeRecorder, seconds, name, extension, rate):
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

recordClip = error_function("recordClip", "visionSystem")

if "visionSystem" in vars:
    if 'IdealSimVision' in str(visionSystem):
        recordClip = recordClipError
    else:
        recordClip = recordClipImpl

def takeFClip(seconds, name = None, extension = ".rmv", rate = 5):
    recordClip(visionSystem.addForwardRecorder, visionSystem.removeForwardRecorder, seconds, name, extension, rate)

def takeDClip(seconds, name = None, extension = ".rmv", rate = 5):
    recordClip(visionSystem.addDownwardRecorder, visionSystem.removeDownwardRecorder, seconds, name, extension, rate)

del vars
del error_function

# End of file (REQUIRED! DO NOT DELETE!)
