# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  tools/simulator/src/sim/vehicle.py

"""
This module implements the ext.core.IVehicle interface in python using a 
simulation robot.
"""

# STD Imports
import math as pmath

# Library Imports
import ogre.renderer.OGRE as ogre
HAVE_NUMPY = True
try:
    import numpy
except ImportError:
    HAVE_NUMPY = False

# Project Imports
import ext.core as core
import ext.vehicle as vehicle
import ext.vehicle.device as device
import ext.math as math
import sim.subsystems as subsystems


def convertToVector3(vType, vector):
    return vType(vector.x, vector.y, vector.z)

def convertToQuaternion(qType, quat):
    return qType(quat.x, quat.y, quat.z, quat.w)

class SimThruster(device.IThruster):
    def __init__(self, eventHub, name, simThruster):
        device.IThruster.__init__(self, eventHub)
        
        self._simThruster = simThruster
        self._name = name
        self._enabled = True
                
    @property
    def relativePosition(self):
        return convertToVector3(math.Vector3, self._simThruster._force_pos)
                
    @property
    def forceDirection(self):
        return convertToVector3(math.Vector3, self._simThruster.direction)
                
    def getName(self):
        return self._name
    
    def setForce(self, force):
        self._simThruster.force = force
        
        event = core.Event()
        event.number = self._simThruster.force
        self.publish(device.IThruster.FORCE_UPDATE, event)
    
    def getForce(self):
        if self._enabled:
            return self._simThruster.force
        else:
            return ogre.Vector3.ZERO
    
    def getMaxForce(self):
        return self._simThruster.max_force
    
    def getMinForce(self):
        return self._simThruster.min_force
                
    def update(self, timestep):
        pass
    
    def setEnabled(self, state):
        self._enabled = state
    
    def isEnabled(self):
        return self._enabled

class SimPayloadSet(device.IPayloadSet):
    def __init__(self, eventHub, name, count = 2, simSpawner = None):
        device.IPayloadSet.__init__(self, eventHub)
        
        self._spawner = simSpawner
        self._name = name
        self._initialCount = count
        self._count = count
        
    def initialObjectCount(self):
        return self._initialCount
    
    def objectCount(self):
        return self._count
    
    def releaseObject(self):
        if self._count != 0:
            self._count -= 1
            event = core.Event()
            self.publish(device.IPayloadSet.OBJECT_RELEASED, event)

class SimVehicle(vehicle.IVehicle):
    def __init__(self, config, deps):
        eventHub = core.Subsystem.getSubsystemOfExactType(core.EventHub, deps)
        vehicle.IVehicle.__init__(self, config.get('name', 'SimVehicle'),
                                  eventHub)
        
        sim = core.Subsystem.getSubsystemOfType(subsystems.Simulation, deps)
        self.robot = sim.scene._robots['Tortuga']
        self._devices = {}
    
        # Add Sim Thruster objects
        self._addThruster(eventHub, 'PortThruster', self.robot.parts.left_thruster)
        self._addThruster(eventHub, 'StarboardThruster', self.robot.parts.right_thruster)
        self._addThruster(eventHub, 'AftThruster', self.robot.parts.aft_thruster)
        self._addThruster(eventHub, 'ForeThruster', self.robot.parts.front_thruster)
        self._addThruster(eventHub, 'TopThruster', self.robot.parts.top_thruster)
        self._addThruster(eventHub, 'BotThruster', self.robot.parts.bot_thruster)

    def _addThruster(self, eventHub, name, simThruster):
        thruster = SimThruster(eventHub, name, simThruster)
        self._devices[name] = thruster
        setattr(self, name[0].lower() + name[1:], thruster)
    
    def getThrusters(self):
        thrusters = []
        for name in self.getDeviceNames():
            device = self.getDevice(name)
            if isinstance(device, vehicle.device.IThruster):
                thrusters.append(device)
        return thrusters
    
    def dropMarker(self):
        # TODO: Add object here and have it throw the proper event
        pass
    
    def fireTorpedo(self):
        pass
    
    def getDevice(self, name):
        return self._devices[name]
    
    def getDeviceNames(self):
        return self._devices.keys()
    
    def getDepth(self):
        # Down is positive for depth
        return -3.281 * self.robot._main_part._node.position.z 
    
    def quaternionFromMagAccel(self, mag, accel):
        """
        Just here for reference, will be moved in the future
        """
        if accel == math.Vector3(0,0,0):
            accel = math.Vector3(0, 0, 0.084214)
        accel = accel + math.Vector3(0,0,-9.8);
        mag.normalise();
        
        n3 = accel * -1;
        n3.normalise();
        n2 = mag.crossProduct(accel);
        n2.normalise();
        n1 = n2.crossProduct(n3);
        n1.normalise();
        
        return math.Quaternion(n1,n2,n3);

    def getOrientation(self):
        return self._getActualOrientation()
        #return self.quaternionFromMagAccel(self.getMag(), self.getLinearAcceleration())
    
    def _getActualOrientation(self):
        return convertToQuaternion(math.Quaternion,
                                  self.robot._main_part._node.orientation)

    def getLinearAcceleration(self):
        baseAccel = convertToVector3(math.Vector3,
                                     self.robot._main_part.acceleration)
        # Add in gravity
        return baseAccel + math.Vector3(0, 0, -9.8)
    
    def getMag(self):
        return self._getActualOrientation() * math.Vector3(0.5, 0, -1);
    
    def getAngularRate(self):
        return convertToVector3(math.Vector3,
                                self.robot._main_part.angular_accel)   
    
    def _vectorToNumpyArray(self, vec):
        return numpy.array([vec.x, vec.y, vec.z])
    
    def applyForcesAndTorques(self, force, torque):
        if HAVE_NUMPY:
            force.y = force.y * -1
            thrusters = self.getThrusters()
            m = len(thrusters)
            A = numpy.zeros([6, m])
        
            for i in range(m):
                thruster = thrusters[i]
                maxThrusterForce = thruster.forceDirection * thruster.getMaxForce()
                A[0:3,i] = self._vectorToNumpyArray(maxThrusterForce)
                A[3:6,i] = self._vectorToNumpyArray(thruster.relativePosition.crossProduct(maxThrusterForce))
        
            b = numpy.array([force.x, force.y, force.z, torque.x, torque.y, torque.z])
            (p, residuals, rank, s) = numpy.linalg.lstsq(A, b)
        
            for i in range(m):
                thruster = thrusters[i]
                thruster.setForce(thruster.getMaxForce() * p[i])
        else:
            # Determine Thruster forces based on thruster position
            star = (force.x / 2) - (0.5 * torque.z / self.starboardThruster.relativePosition.y)
            port = (force.x / 2) - (0.5 * torque.z / self.portThruster.relativePosition.y)
            fore = (force.z / 2) - (0.5 * torque.y / self.foreThruster.relativePosition.x)
            aft = (force.z / 2) - (0.5 * torque.y / self.aftThruster.relativePosition.x)
            top = (force.y / 2) - (0.5 * torque.x / self.topThruster.relativePosition.z)
            bot = (force.y / 2) + (0.5 * torque.x / self.botThruster.relativePosition.z)

            self.starboardThruster.setForce(star)
            self.portThruster.setForce(port)
            self.foreThruster.setForce(fore)
            self.aftThruster.setForce(aft)
            self.topThruster.setForce(top)
            self.botThruster.setForce(bot)

            # Set forces exactly
            #self.robot._main_part.set_local_force(
            #    convertToVector3(ogre.Vector3, force), (0,0,0))
            #self.robot._main_part.torque = convertToVector3(ogre.Vector3, torque)            
    
    def backgrounded(self):
        return False
    
    def unbackground(self, join = True):
        pass
    
    def update(self, timeSinceUpdate):
        # Send Orientation Event
        event = core.Event()
        event.orientation = self.getOrientation()
        self.publish(vehicle.IVehicle.ORIENTATION_UPDATE, event)
        
        # Send Depth Event
        event = core.Event()
        event.number = self.getDepth()
        self.publish(vehicle.IVehicle.DEPTH_UPDATE, event)

core.SubsystemMaker.registerSubsystem('SimVehicle', SimVehicle)
