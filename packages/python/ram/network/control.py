# Copyright (C) 2010 Maryland Robotics Club
# Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
# All rights reserved.
#
# Author: Jonathan Sternberg <jsternbe@umd.edu>
# File:  packages/python/ram/network/vision.py

# Library Imports
import Ice

# Project Imports
import ext.core
import ext.math
import ext.control

import proxy.math
import proxy.control

class ControllerProxy(ext.control.IController):
    def __init__(self, config, deps = None):
        cfg = ext.core.ConfigNode.fromString(str(config))
        if deps is None:
            deps = []
        eventHub = ext.core.Subsystem.getSubsystemOfType(
            ext.control.IController, deps)

        ext.control.IController.__init__(self, config.get('name', 'Controller'),
                                         eventHub)

        self._ic = Ice.initialize()
        host = config.get('host', 'localhost')
        port = config.get('port', '10000')
        base = self._ic.stringToProxy(
            'Controller:default -h %s -p %s' % (host, port))
        self._impl = proxy.control.IControllerPrx.checkedCast(base)

    def __del__(self):
        self._ic.destroy()

    def setVelocity(self, velocity):
        self._impl.setVelocity(proxy.math.Vector2(x = velocity.x,
                                                  y = velocity.y))
    def getVelocity(self):
        velocity = self._impl.getVelocity()
        return ext.math.Vector2(velocity.x, velocity.y)

    def setSpeed(self, speed):
        self._impl.setSpeed(float(speed))
    def setSidewaysSpeed(self, speed):
        self._impl.setSidewaysSpeed(float(speed))

    def getSpeed(self):
        return self._impl.getSpeed()
    def getSidewaysSpeed(self):
        return self._impl.getSidewaysSpeed()

    def holdCurrentPosition(self):
        return self._impl.holdCurrentPosition()

    def atPosition(self):
        return self._impl.atPosition()
    def atVelocity(self):
        return self._impl.atVelocity()

    def yawVehicle(self, degrees):
        self._impl.yawVehicle(float(degrees))
    def pitchVehicle(self, degrees):
        self._impl.pitchVehicle(float(degrees))
    def rollVehicle(self, degrees):
        self._impl.rollVehicle(float(degrees))

    def getDesiredOrientation(self):
        quat = self._impl.getDesiredOrientation()
        # Convert from ICE quaternion to math Quaternion
        return ext.math.Quaternion(quat.x, quat.y, quat.z, quat.w)
    def setDesiredOrientation(self, quat):
        self._impl.setDesiredOrienation(
            proxy.math.Quaternion(x = quat.x, y = quat.y,
                                  z = quat.z, w = quat.w))
    def atOrientation(self):
        return self._impl.atOrientation()

    def setDepth(self, depth):
        self._impl.setDepth(float(depth))
    def getDepth(self):
        return self._impl.getDepth()

    def getEstimatedDepth(self):
        return self._impl.getEstimatedDepth()
    def getEstimatedDepthDot(self):
        return self._impl.getEstimatedDepthDot()
    def atDepth(self):
        return self._impl.atDepth()
    def holdCurrentDepth(self):
        self._impl.holdCurrentDepth()

    def holdCurrentHeading(self):
        self._impl.holdCurrentHeading()

    def backgrounded(self):
        return True
    def background(self, interval = -1):
        pass
    def unbackground(self, join = False):
        pass

ext.core.SubsystemMaker.registerSubsystem('ControllerProxy',
                                          ControllerProxy)
