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

from sim.subsystems import Simulation
from sim.vision import IBuoy, IPipe, IBarbedWire, IBin, ITarget, ISafe

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

        # Grab the simulation object
        sim = core.Subsystem.getSubsystemOfType(Simulation, deps, 
                                                    nonNone = True)

        self._getObjectInstances(sim, "buoy", IBuoy)
        self._getObjectInstances(sim, "pipe", IPipe)
        self._getObjectInstances(sim, "bwire", IBarbedWire)
        self._getObjectInstances(sim, "bin", IBin)
        self._getObjectInstances(sim, "target", ITarget)
        self._getObjectInstances(sim, "safe", ISafe)

    def _getObjectInstances(self, sim, name, interface):
        robot = self._vehicle.getDevice('SimulationDevice').robot
        robotPos = robot._main_part._node.position

        objects = sim.scene.getObjectsByInterface(interface)
        if objects is None or len(objects) == 0:
            return
        if len(objects) == 1:
            position = objects[0].position
            self._objects[name] = math.Vector2(position.x - robotPos.x,
                                               position.y - robotPos.y)
        else:
            for i, obj in enumerate(objects):
                self._objects[name + str(i+1)] = \
                    math.Vector2(obj.position.x - robotPos.x,
                                 obj.position.y - robotPos.y)

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
