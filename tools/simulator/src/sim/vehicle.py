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
import sim.sonar as sonar
import ram.sim.scene as scene
import ram.sim.graphics as graphics

def convertToVector3(vType, vector):
    return vType(vector.x, vector.y, vector.z)

def convertToQuaternion(qType, quat):
    return qType(quat.x, quat.y, quat.z, quat.w)

class SimDevice(object):
    def getName(self):
        return self._name

    def setPriority(self, priority):
        pass
        
    def getPriority(self):
        return 0
    
    def setAffinity(self, core):
        pass
    
    def getAffinity(self):
        return 0

    def background(self, interval = -1):
        pass

    def backgrounded(self):
        return False
    
    def unbackground(self, join = False):
        pass

    def update(self, timestep):
        pass
    
class SimulationDevice(SimDevice, device.IDevice):
    
    def __init__(self, config, eventHub, vehicle):
        self._name = config['name']
        SimDevice.__init__(self)
        device.IDevice.__init__(self, eventHub, self._name)

        sim = subsystems.Simulation.SIMULATION
        self.robot = sim.scene._robots['Tortuga']
        self.scene = sim.scene
        subsystems.Simulation.SIMULATION = None

device.IDeviceMaker.registerDevice('SimulationDevice', SimulationDevice)


class SimDepthSensor(SimDevice, device.IDepthSensor):
    def __init__(self, config, eventHub, vehicle):
        self._name = config['name']
        SimDevice.__init__(self)
        device.IDepthSensor.__init__(self, eventHub, self._name)
        
        simDevice = vehicle.getDevice('SimulationDevice')
        self.robot = simDevice.robot        

    def update(self, time):
        event = math.NumericEvent()
        event.number = self.getDepth()
        self.publish(device.IDepthSensor.UPDATE, event)
        rawEvent = vehicle.RawDepthSensorDataEvent()
        rawEvent.rawDepth = self.getDepth()
        rawEvent.sensorLocation = self.getLocation()
        self.publish(device.IDepthSensor.RAW_UPDATE, rawEvent)
        
    def getDepth(self):
        # Down is positive for depth
        return -3.281 * self.robot._main_part._node.position.z
        
    def getLocation(self):
        return math.Vector3(0, 0, 0)

device.IDeviceMaker.registerDevice('SimDepthSensor', SimDepthSensor)


class SimVelocitySensor(SimDevice, device.IVelocitySensor):
    def __init__(self, config, eventHub, vehicle):
        self._name = config['name']
        SimDevice.__init__(self)
        device.IVelocitySensor.__init__(self, eventHub, self._name)
        
        simDevice = vehicle.getDevice('SimulationDevice')
        self.robot = simDevice.robot

        # This will keep track of the current velocity
        self.oldPos = math.Vector2(self.robot._main_part._node.position.x,
                                   -self.robot._main_part._node.position.y)
        self.velocity = math.Vector2(0, 0)

    def update(self, time):
        # On each update, we'll find the velocity using the old position
        currentPos = math.Vector2(self.robot._main_part._node.position.x,
                                  -self.robot._main_part._node.position.y)
        # Subtract by the old position and divide by time
        self.velocity = (currentPos - self.oldPos) / time

        # Update the position
        self.oldPos = currentPos

        # Publish an update event
        event = math.Vector2Event()
        event.vector2 = self.getVelocity()
        self.publish(device.IVelocitySensor.UPDATE, event)
        
    def getVelocity(self):
        return self.velocity
        
    def getLocation(self):
        return math.Vector3(0, 0, 0)

device.IDeviceMaker.registerDevice('SimVelocitySensor', SimVelocitySensor)


class IdealPositionSensor(SimDevice, device.IPositionSensor):
    def __init__(self, config, eventHub, vehicle):
        self._name = config['name']
        SimDevice.__init__(self)
        device.IPositionSensor.__init__(self, eventHub, self._name)
    
        simDevice = vehicle.getDevice('SimulationDevice')
        self.robot = simDevice.robot

        self.initialPos = math.Vector2(self.robot._main_part._node.position.x,
                                       -self.robot._main_part._node.position.y)

    def update(self, time):
        event = math.Vector2Event()
        event.vector2 = self.getPosition()
        self.publish(device.IPositionSensor.UPDATE, event)

    def getPosition(self):
        # Gets the exact position relative to the initial position
        #currentPos = math.Vector2(-self.robot._main_part._node.position.y,
        #                           self.robot._main_part._node.position.x)
        #return currentPos - self.initialPos
        return math.Vector2(self.robot._main_part._node.position.x,
                            -self.robot._main_part._node.position.y)
    def getLocation(self):
        return math.Vector3(0, 0, 0)

device.IDeviceMaker.registerDevice('IdealPositionSensor',
                                   IdealPositionSensor)

class SimIMU(SimDevice, device.IIMU):
    def __init__(self, config, eventHub, vehicle):
        self._name = config['name']
        SimDevice.__init__(self)
        device.IIMU.__init__(self, eventHub, self._name)
    
        simDevice = vehicle.getDevice('SimulationDevice')
        self.robot = simDevice.robot        

    def update(self, time):
        event = math.OrientationEvent()
        event.orientation = self.getOrientation()
        raw_event = vehicle.RawIMUDataEvent()
        raw_event.name = "simIMU"
        self.publish(device.IIMU.UPDATE, event)
        self.publish(device.IIMU.RAW_UPDATE, raw_event)
    
    def _getActualOrientation(self):
        return convertToQuaternion(math.Quaternion,
                                  self.robot._main_part._node.orientation)

    def getLinearAcceleration(self):
        baseAccel = convertToVector3(math.Vector3,
                                     self.robot._main_part.acceleration)
        # Add in gravity
        return baseAccel + math.Vector3(0, 0, -9.8)
    
    def getMagnetometer(self):
        return self._getActualOrientation() * math.Vector3(0.5, 0, -1);
    
    def getAngularRate(self):
        return convertToVector3(math.Vector3,
                                self.robot._main_part.angular_accel) 
        
    def getOrientation(self):
        return self._getActualOrientation()

device.IDeviceMaker.registerDevice('SimIMU', SimIMU)


class SimSonar(SimDevice, device.ISonar):
    def __init__(self, config, eventHub, vehicle):
        self._name = config['name']
        SimDevice.__init__(self)
        device.ISonar.__init__(self, eventHub, self._name)
        
        # Initialize variables
        self._direction = math.Vector3.ZERO
        self._range = 0
        
        # Subscribe to pings from the simulation
        eventHub.subscribeToType(sonar.Pinger.PING, self._onSonarUpdate)
        
    def getDirection(self):
        return self._direction

    def getRange(self):
        return self._range

    def _onSonarUpdate(self, event):
        self._direction = event.direction
        self._range = event.range
        
        self.publish(device.ISonar.UPDATE, event)

device.IDeviceMaker.registerDevice('SimSonar', SimSonar)


class SimThruster(SimDevice, device.IThruster):
    def __init__(self, config, eventHub, vehicle):
        self._name = config['name']
        SimDevice.__init__(self)
        device.IThruster.__init__(self, eventHub, self._name)
        
        simDevice = vehicle.getDevice('SimulationDevice')
        robot = simDevice.robot        
        self._simThruster = getattr(robot.parts, config['simName'])
        self._relAxis = config['relAxis']
        self._enabled = True
                
    @property
    def relativePosition(self):
        return convertToVector3(math.Vector3, self._simThruster._force_pos)
                
    @property
    def forceDirection(self):
        return convertToVector3(math.Vector3, self._simThruster.direction)
                
    def setForce(self, force):
        self._simThruster.force = force
        
        event = math.NumericEvent()
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
                
    def setEnabled(self, state):
        self._enabled = state
    
    def isEnabled(self):
        return self._enabled
    
    def getOffset(self):
        return pmath.fabs(getattr(self.relativePosition, self._relAxis))
    
device.IDeviceMaker.registerDevice('SimThruster', SimThruster)


class SimPayloadSet(SimDevice, device.IPayloadSet):
    def __init__(self, eventHub, name, count = 2, scene = None, robot = None,
                 payload = None):
        device.IPayloadSet.__init__(self, eventHub, name)
        
        self._scene = scene
        self._payload = payload
        self._robot = robot
        self._name = name
        self._initialCount = count
        self._count = count
        
    def initialObjectCount(self):
        return self._initialCount
    
    def objectCount(self):
        return self._count
    
    def releaseObject(self):
        if self._count != 0:
            
            event = core.Event()
            self.publish(device.IPayloadSet.OBJECT_RELEASED, event)
            
            # Bail out early if there is no scene
            if self._scene is None:
                self._count -= 1
                return
            
            if self._payload == 'marker':
                self._spawnMarker()
            elif self._payload == 'torpedo':
                self._spawnTorpedo()
            elif self._payload == 'grabber':
                self._spawnCube()
            
            self._count -= 1
            
    def _spawnMarker(self):
        # Now lets spawn an object
        obj = scene.SceneObject()
        position = self._robot._main_part._node.position
        robotOrient = self._robot._main_part._node.orientation

        # 30cm below robot
        offset = ogre.Vector3(0.15, 0.30 - (self._count * 0.2), -0.10)
        position = position + robotOrient * offset 
        orientation = ogre.Quaternion(ogre.Degree(90), ogre.Vector3.UNIT_X)

        cfg = {
            'name' : self._name + str(self._count),
            'position' : position,
            'Graphical' : {
                'mesh' : 'cylinder.mesh', 
                'scale' : [0.0762, 0.0127, 0.0127],
                'material' : 'Simple/Red',
                'orientation' : orientation * robotOrient
            },
            'Physical' : {
                'mass' : 0.01, 
                'center_of_mass' : [0, 0, 0.0127], # Top heavy
                'orientation' : orientation * robotOrient,
                'Shape' : {
                    'type' : 'cylinder',
                    'radius' : 0.0127,
                    'height' : 0.0762
                }
            }
        }
        obj.load((self._scene, None, cfg))
        self._scene._objects.append(obj)
                     
    def _spawnTorpedo(self):
        # Now lets spawn an object
        obj = scene.SceneObject()
        position = self._robot._main_part._node.position
        robotOrient = self._robot._main_part._node.orientation

        # 30cm below robot
        offset = ogre.Vector3(0.4, 0.30 - (self._count * 0.2), 0)
        position = position + robotOrient * offset 
        
        cfg = {
            'name' : self._name + str(self._count),
            'position' : position,
            'Graphical' : {
                'mesh' : 'cylinder.mesh', 
                'scale' : [0.127, 0.0127, 0.0127],
                'material' : 'Simple/Red',
                'orientation' : robotOrient 
            },
            'Physical' : {
                'mass' : 0.005,
                'orientation' : robotOrient,
                'Shape' : {
                    'type' : 'cylinder',
                    'radius' : 0.0127,
                    'height' : 0.0127
                }
            }
        }
        obj.load((self._scene, None, cfg))
        obj._body.setVelocity(robotOrient * ogre.Vector3(10, 0, 0))
        self._scene._objects.append(obj)

    def _spawnCube(self):
        # Now lets spawn an object
        obj = scene.SceneObject()
        position = self._robot._main_part._node.position
        robotOrient = self._robot._main_part._node.orientation

        # 30cm below robot
        offset = ogre.Vector3(0.4, 0.30 - (self._count * 0.2), 0)
        position = position + robotOrient * offset 
        
        cfg = {
            'name' : self._name + str(self._count),
            'position' : position,
            'Graphical' : {
                'mesh' : 'cylinder.mesh', 
                'scale' : [0.127, 0.0127, 0.0127],
                'material' : 'Simple/Yellow',
                'orientation' : robotOrient 
            },
            'Physical' : {
                'mass' : 0.005,
                'orientation' : robotOrient,
                'Shape' : {
                    'type' : 'cylinder',
                    'radius' : 0.0127,
                    'height' : 0.0127
                }
            }
        }
        obj.load((self._scene, None, cfg))
        self._scene._objects.append(obj)

class SimMarkerDropper(SimPayloadSet): # SimPayloadSet
    def __init__(self, config, eventHub, vehicle):
        self._name = config['name']
        simDevice = vehicle.getDevice('SimulationDevice')
        
        SimPayloadSet.__init__(self, eventHub, self._name, count = 2, 
                               scene = simDevice.scene, 
                               robot = simDevice.robot, payload = 'marker')

device.IDeviceMaker.registerDevice('SimMarkerDropper', SimMarkerDropper)

class SimTorpedoLauncher(SimPayloadSet): # SimPayloadSet
    def __init__(self, config, eventHub, vehicle):
        self._name = config['name']
        simDevice = vehicle.getDevice('SimulationDevice')
        
        SimPayloadSet.__init__(self, eventHub, self._name, count = 2, 
                               scene = simDevice.scene, 
                               robot = simDevice.robot, payload = 'torpedo')

device.IDeviceMaker.registerDevice('SimTorpedoLauncher', SimTorpedoLauncher)

class SimGrabber(SimPayloadSet): # SimPayloadSet
    def __init__(self, config, eventHub, vehicle):
        self._name = config['name']
        simDevice = vehicle.getDevice('SimulationDevice')

        SimPayloadSet.__init__(self, eventHub, self._name, count = 2,
                               scene = simDevice.scene,
                               robot = simDevice.robot, payload = 'grabber')

device.IDeviceMaker.registerDevice('SimGrabber', SimGrabber)

class TrailMarker(SimDevice, device.IDevice):
    def __init__(self, config, eventHub, vehicle):
        self._name = config['name']
        SimDevice.__init__(self)
        device.IDevice.__init__(self, eventHub, self._name)

        simDevice = vehicle.getDevice('SimulationDevice')
        self.robot = simDevice.robot
        self._scene = simDevice.scene     

        # Settings
        self._dropMarkers = config.get('markers', True)
        self._markerInterval = config.get('markerInterval', 1)
        
        # State variables
        self._markers = []
        self._markerCount = 0
        self._timeSinceLastMarker = self._markerInterval
        
    def update(self, timeSinceUpdate):   
        if self._dropMarkers:
            self._timeSinceLastMarker += timeSinceUpdate
            if self._timeSinceLastMarker >= self._markerInterval:
                self._spawnMarker()
                self._timeSinceLastMarker = 0

    def _spawnMarker(self):
        # Now lets spawn an object
        obj = graphics.Visual()
        position = self.robot._main_part._node.position

        cfg = {
            'name' : 'marker_' + str(self._markerCount),
            'position' : position,
            'Graphical' : {
                'mesh' : 'sphere.50cm.mesh', 
                'scale' : [0.075, 0.075, 0.075],
                'material' : 'Simple/Blue',
            }
        }
        obj.load((self._scene, None, cfg))
        #self._scene._objects.append(obj)
        self._markers.append(obj)
        self._markerCount += 1
        
    def setMarkerVisibility(self, value):
        for marker in self._markers:
            marker.visible = value
            
device.IDeviceMaker.registerDevice('TrailMarker', TrailMarker)            
