import wx
import ext.core as core
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

dive = diveTo

def up(depthChange, speed = 0.3):
    diveTo(vehicle.getDepth() - depthChange, speed = speed)

def down(depthChange, speed = 0.3):
    diveTo(vehicle.getDepth() + depthChange, speed = speed)

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

def fstream(port = 50000, size = (320, 240), rate = 5):
    path = str(port) + "(" + str(size[0]) + "," + str(size[1]) + ")"
    visionSystem.addForwardRecorder(path, rate)

def dstream(port = 50001, size = (320, 240), rate = 5):
    path = str(port) + "(" + str(size[0]) + "," + str(size[1]) + ")"
    visionSystem.addDownwardRecorder(path, rate)

def fsremove(name = '50000(320,240)'):
    visionSystem.removeForwardRecorder(name)

def dsremove(name = '50001(320,240)'):
    visionSystem.removeDownwardRecorder(name)

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

# End of file (this line is required)
