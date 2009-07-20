import ram.ai.light as light
import ram.ai.pipe as pipe
import ram.ai.bin as bin
import ram.ai.barbedwire as bwire
import ram.ai.target as target
import ram.ai.course as course
import ram.motion as motion
import ram.motion.basic as basic
import ram.motion.search as search

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

def yaw2(yawChange, speed = 30):
    yaw(yawChange, speed, absolute = True)

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

# End of file (this line is required)
