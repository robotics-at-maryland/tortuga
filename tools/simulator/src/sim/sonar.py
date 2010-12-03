# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   tools/simulator/src/sim/sonar


# Project Imports
import ext.core
import ext.math
import ext.vehicle
from ext.vehicle import IVehicle
from sim.subsystems import Simulation

import ram.timer
import ram.core as core
from ram.sim.serialization import two_step_init
from ram.sim.object import IObject
from ram.sim.graphics import IVisual, Visual


class IPinger(IObject):
    """The pinger which produces pulses"""
    
class Pinger(Visual):
    core.implements(IVisual, IPinger)
    
    PING = ext.core.declareEventType('PING')

    PING_COUNT = 0
    
    @two_step_init
    def __init__(self):
        Visual.__init__(self)
        self._robot = None
        self._sonarSys = None
        self._timeSinceLastPing = 0
        self._pingInterval = 0

    def load(self, data_object):
        scene, parent, node = data_object
        if not node.has_key('Graphical'):
            # Default mesh and scale info
            gfxNode = {'mesh' : 'sphere.50cm.mesh', 
                       'scale' : [0.15, 0.15, 0.23],
                       'material' : 'Simple/Orange' }
            node['Graphical'] = gfxNode
        Visual.load(self, (scene, parent, node))

        self._pingerID = node.get('ID', 0)
        self._pingInterval = node.get('pingInterval', 2)
        self._timeSinceLastPing = -1 * node.get('delay', 0)

    def save(self, data_object):
        raise "Not yet implemented"
        
    def setup(self, robot, sonarSys):
        self._robot = robot
        self._sonarSys = sonarSys

    def destroy(self):
        self._robot = None
        self._sonarSys = None

    def update(self, timeSinceLastUpdate):
        self._timeSinceLastPing += timeSinceLastUpdate
        if self._timeSinceLastPing >= self._pingInterval:
            if self._robot is not None:
                self._doPing()
            self._timeSinceLastPing = 0
            
    def _doPing(self):
        event = ext.vehicle.SonarEvent()

        relativePos = self.position - self._robot.position
        relativePos = self._robot.orientation.Inverse() * relativePos 
        relativePos.normalise()
        event.direction = ext.math.Vector3(relativePos.x, relativePos.y,
            relativePos.z)

        Pinger.PING_COUNT += 1
        event.pingCount = Pinger.PING_COUNT

        event.pingerID = self._pingerID

        self._sonarSys.publish(Pinger.PING, event)
    
class SimSonar(ext.core.Subsystem):
    def __init__(self, config, deps):
        ext.core.Subsystem.__init__(self, config.get('name', 'Simulation'), 
                                    deps)

        # Grab the vehicle
        sim = ext.core.Subsystem.getSubsystemOfType(Simulation, deps, 
                                                    nonNone = True)
        vehicle = ext.core.Subsystem.getSubsystemOfType(IVehicle, deps, 
                                                        nonNone = True)
        self._robot = vehicle.getDevice('SimulationDevice').robot

        # Grab the pinger object
        self._pingers = sim.scene.getObjectsByInterface(IPinger)
        assert len(self._pingers) <= 2
        self._setup()

    def _setup(self):
        for pinger in self._pingers:
            pinger.setup(robot = self._robot, sonarSys = self)

    def _destroy(self):
        for pinger in self._pingers:
            pinger.destroy()

    def unbackground(self, join = True):
        self._destroy()

    def background(self):
        self._setup()

    def backgrounded(self):
        return True
        
    def update(self, timeSinceLastUpdate):
        print "ERROR, should not be updating"
      
ext.core.SubsystemMaker.registerSubsystem('SimSonar', SimSonar)      
