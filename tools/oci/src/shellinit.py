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
# in the OCI. Only use comments with the '#' symbol. Docstrings within functions
# can, and are encouraged, to be used.

import math as pmath
import ext.core as core
import ext.math as math
import ram.ai.light as light
import ram.ai.buoy as buoy
import ram.ai.gen2.light as light2
import ram.ai.pipe as pipe
import ram.ai.bin as bin
import ram.ai.barbedwire as bwire
import ram.ai.target as target
import ram.ai.window as window
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

def error_function(fname, fmsg):
    def error(*args):
        raise Exception("%s is not defined because %s "
                                    "is not loaded" % (fname, fmsg));
    return error

# Function decorator
# If the requirements are not met, replaces the function with an error message
class requires(object):
    def __init__(self, *args):
        self._systemList = args
    def __call__(self, f):
        for sys in self._systemList:
            if sys not in vars:
                print "Disabling quick function %s" % f.__name__
                return error_function(f.__name__, sys)
        return f

# Helper methods
@requires('motionManager')
def diveTo(depth, speed = 0.3):
    """
    Dive to the specified depth.

    @param depth Desired depth
    @param speed Speed of the dive
    @requires motionManager
    """
    motionManager.setMotion(basic.RateChangeDepth(depth, speed))

# dive and diveTo are the same function.
dive = diveTo

@requires('vehicle', 'motionManager')
def up(depthChange, speed = 0.3):
    """
    Move the vehicle upward by the specified value.

    @param depthChange Distance upwards to move.
    @param speed Speed of the dive
    @requires vehicle, motionManager
    """
    diveTo(vehicle.getDepth() - depthChange, speed = speed)

@requires('vehicle', 'motionManager')
def down(depthChange, speed = 0.3):
    """
    Move the vehicle downward by the specified value.

    @param depthChange Distance downwards to move.
    @param speed Speed of the dive
    @requires vehicle, motionManager
    """
    diveTo(vehicle.getDepth() + depthChange, speed = speed)

# Surface depth to 0.3 because 0.0 causes the vehicle to always try and get
# above the water
def surface(speed = 0.3):
    """
    Dives the vehicle to the surface.
    The surface is an approximate value, and the vehicle may
    be slightly underwater or try to force itself above water at
    the end.

    @param speed Speed of the dive
    @requires motionManager
    """
    diveTo(depth = 0.3, speed = speed)

@requires('motionManager')
def yaw(yawChange, speed = 30, absolute = False):
    """
    Yaws the vehicle the specified number of degrees.
    Positive values rotate counter-clockwise, negative values rotate
    clockwise. Use the right-hand rule.

    @param yawChange Distance to yaw in degrees. A value of greater than 180
                     will just loop around.
    @param speed Speed to yaw the vehicle
    @param absolute If True, the vehicle will yaw to the heading given.
                    Otherwise, it will figure out a relative number of
                    degrees to yaw.
    @requires motionManager
    """
    motionManager.setMotion(basic.RateChangeHeading(yawChange, speed, absolute = absolute))

@requires('motionManager')
def yawTo(angle, speed = 30):
    """
    This yaws the vehicle to the specified absolute angle.

    @param angle Angle to yaw to in degrees
    @param speed Speed to yaw the vehicle
    @see yaw
    @requires motionManager
    """
    yaw(yawChange, speed, absolute = True)

yaw2 = yawTo

@requires('motionManager', 'controller', 'stateMachine')
def allStop():
    """
    Stops every possible system that could move the vehicle.

    @requires motionManager, controller, stateMachine
    """
    stateMachine.stop()
    motionManager.stopCurrentMotion()
    controller.setSpeed(0)
    controller.setSidewaysSpeed(0)
    controller.holdCurrentDepth()
    controller.holdCurrentHeading()

s = allStop
stop = allStop

@requires('vehicle')
def safe():
    """
    Safes the thrusters on the vehicle.

    @requires vehicle
    """
    vehicle.safeThrusters()

@requires('vehicle')
def unsafe():
    """
    Unsafes the thrusters on the vehicle, stops every motion the
    vehicle may be trying to do, and resets the depth to avoid any
    complications while unsafing the thrusters.

    @requires vehicle
    """
    controller.setDepth(vehicle.getDepth())
    allStop()
    # This is unsafe, since this doesn't always work
    vehicle.unsafeThrusters()
    controller.setDepth(vehicle.getDepth())

@requires('stateMachine')
def start(state):
    """
    Starts the state machine using the specified state.

    @param state The state to start
    """
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
    """
    Starts forward video streaming.

    @param port The port to stream video over
    @param size The size to send video at
    @param rate The rate of the video
    """
    RecorderManager.fstream(port, size, rate)

def dstream(port = 50001, size = (320, 240), rate = 5):
    """
    Starts downward video streaming.

    @param port The port to stream video over
    @param size The size to send video at
    @param rate The rate of the video
    """
    RecorderManager.dstream(port, size, rate)

def removefs(port = 50000):
    """
    Removes the forward recorder associated with the given port.

    @param port The port to remove forward streaming from.
    """
    RecorderManager.removefs(port)

def removeds(port = 50001):
    """
    Removes the downward recorder associated iwth the given port.

    @param port The port to remove downward streaming from.
    """
    RecorderManager.removeds(port)

fsremove = removefs
dsremove = removeds

# Quick detector functions

# make error functions for all of them
lightOn = error_function("lightOn", "visionSystem")
lightOff = error_function("lightOff", "visionSystem")
buoyOn = error_function("buoyOn", "visionSystem")
buoyOff = error_function("buoyOff", "visionSystem")
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
    buoyOn = visionSystem.buoyDetectorOn
    buoyOff = visionSystem.buoyDetectorOff
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
    """
    Takes a forward video and saves it to disk.

    @param seconds Time to record video
    @param name The name to save the video as. If None, this will be
                auto-generated based on the current date/time.
    @param extension Extension to save the video as. .rmv or .avi are supported.
    @param rate The rate to record video at
    """
    recordClip(visionSystem.addForwardRecorder, visionSystem.removeForwardRecorder, seconds, name, extension, rate)

def takeDClip(seconds, name = None, extension = ".rmv", rate = 5):
    """
    Takes a downward video and saves it to disk.

    @param seconds Time to record video
    @param name The name to save the video as. If None, this will be
                auto-generated based on the current date/time.
    @param extension Extension to save the video as. .rmv or .avi are supported.
    @param rate The rate to record video at
    """
    recordClip(visionSystem.addDownwardRecorder, visionSystem.removeDownwardRecorder, seconds, name, extension, rate)

del vars
del error_function

# End of file (REQUIRED! DO NOT DELETE!)
