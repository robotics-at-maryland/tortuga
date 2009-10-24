# Copyright (C) 2009 Maryland Robotics Club
# Copyright (C) 2009 Jonathan Sternberg <jsternbe@umd.edu>
# All rights reserved.
#
# Author: Jonathan Sternberg <jsternbe@umd.edu>
# File:   tools/simulator/src/sim/slam.py

# Library Imports
import ogre.renderer.OGRE as ogre

# Project Imports
import ext.core as core
import ext.control as control
import ext.math as math
import ext.slam as slam
import ext.vehicle as vehicle

class PerfectSlam(slam.ISlam):
    def __init__(self, config, deps):
        slam.ISlam.__init__(self, config.get('name', 'Slam'),
                            core.Subsystem.getSubsystemOfType(
                core.QueuedEventHub, deps))

        # Grab the vehicle and the controller
        self._vehicle = core.Subsystem.getSubsystemOfType(vehicle.IVehicle,
                                                          deps,
                                                          nonNone = True)
        self._controller = core.Subsystem.getSubsystemOfType(
            control.IController, deps, nonNone = True)

        # Create the dictionary that stores the values
        self._objects = {}

        objectList = config.get("Objects", {})
        for obj, loc in objectList.iteritems():
            # These need to be excluded if an INCLUDE is there
            if obj != "INCLUDE_LOADED" and obj != "INCLUDE":
                self._objects[obj] = math.Vector2(loc[0], loc[1])

    def getObjectPosition(self, name):
        return self._objects[name]

    def getRelativePosition(self, name):
        return self._vehicle.getPosition() - self._objects[name]

    def hasObject(self, name):
        return self._objects.has_key(name)

    def backgrounded(self):
        return True

    def unbackground(self, join = False):
        pass

    def background(self, interval = -1):
        pass

core.SubsystemMaker.registerSubsystem('PerfectSlam', PerfectSlam)
