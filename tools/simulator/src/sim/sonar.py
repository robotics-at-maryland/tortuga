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
from sim.subsystems import Simulation
from sim.vehicle import SimVehicle

import ram.timer
import ram.core as core
from ram.sim.serialization import two_step_init
from ram.sim.object import IObject
from ram.sim.graphics import IVisual, Visual


class IPinger(IObject):
    """The pinger which produces pulses"""
    
class Pinger(Visual):
    core.implements(IVisual, IPinger)
    
    @two_step_init
    def __init__(self):
        Visual.__init__(self)

    def load(self, data_object):
        scene, parent, node = data_object
        if not node.has_key('Graphical'):
            # Default mesh and scale info
            gfxNode = {'mesh' : 'sphere.50cm.mesh', 
                       'scale' : [0.15, 0.15, 0.23],
                       'material' : 'Simple/Orange' }
            node['Graphical'] = gfxNode
        Visual.load(self, (scene, parent, node))
        
    def save(self, data_object):
        raise "Not yet implemented"
    
class SimSonar(ext.core.Subsystem):
    def __init__(self, config, deps):
        ext.core.Subsystem.__init__(self, config.get('name', 'Simulation'), 
                                    deps)

        # Grab the vehicle
        sim = ext.core.Subsystem.getSubsystemOfType(Simulation, deps, 
                                                    nonNone = True)
        self.vehicle = ext.core.Subsystem.getSubsystemOfType(SimVehicle, deps, 
                                                             nonNone = True)
        
        # How long since its been since a ping
        self._pingInterval = config.get('pingInterval', 2)
        self._timeSinceLastPing = 10

        # Grab the pinger object
        self._pingers = sim.scene.getObjectsByInterface(IPinger)
        assert len(self._pingers) <= 1
        if len(self._pingers) >= 1:
            self._pinger = self._pingers[0]
        else:
            self._pinger = None

    def backgrounded(self):
        return False
        
    def update(self, timeSinceLastUpdate):
        self._timeSinceLastPing += timeSinceLastUpdate
        if self._timeSinceLastPing > self._pingInterval:
            if self._pinger is not None:
                self._doPing()
                self._timeSinceLastPing = 0
            
    def _doPing(self):
        event = ext.vehicle.SonarEvent()
        relativePos = self._pinger.position - self.vehicle.robot.position
        relativePos = self.vehicle.robot.orientation.Inverse() * relativePos 
        relativePos.normalise()
        event.direction = ext.math.Vector3(relativePos.x, relativePos.y,
            relativePos.z)
        self.publish(ext.vehicle.device.ISonar.UPDATE, event)
      
ext.core.SubsystemMaker.registerSubsystem('SimSonar', SimSonar)      