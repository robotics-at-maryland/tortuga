import ext.core as core
import ext.vehicle as vehicle
import ext.vehicle_device as vdev
import ext.math as math
from math import sin, pi,sqrt,degrees #degrees for testing only

class MockVehicle(vehicle.IVehicle,core.EventPublisher):
    def __init__(self,name):
        vehicle.IVehicle.__init__(self,name)
        """Sensor Data Variables"""
        self.depth = 0.0
        self.orientation = math.Quaternion(math.Quaternion.IDENTITY)
        self.linearAccel = math.Vector3(math.Vector3.ZERO)
        self.angularRate = math.Vector3(math.Vector3.ZERO)
        self.force = math.Vector3(math.Vector3.ZERO)
        self.torque = math.Vector3(math.Vector3.ZERO)
        #self.rotationList = []
        #self.thrusterList = []
    
    def applyForcesAndTorque(self, forceIn, torqueIn):
        self.force = forceIn
        self.torque = torqueIn
        
    def dropMarker(self):
        pass
    
    def safeThrusters(self):
        pass
    
    def unSafeThrusters(self):
        pass
    
    """Getter Methods""" 
    def getAngularRate(self):
        return self.angularRate
    
    def getDepth(self):
        return self.depth
    
    def getDevice(self):
        pass
    
    def getDeviceNames(self):
        pass
    
    def getLinearAcceleration(self):
        return self.linearAccel
    
    def getOrientation(self):
        return self.orientation
    
    def getTemperatures(self):
        pass