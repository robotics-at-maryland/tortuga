# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Jon Speiser <jspeiser@umd.edu>
# All rights reserved.
#
# Author: Jon Speiser <jspeiser@umd.edu>
import ext.core as core
import ext.math as math
from math import sqrt,pi,degrees

class Orientation(core.EventPublisher):
        SQRTFIVE = sqrt(5)
        def __init__(self,angleIn=15,nameIn="Roll"):
            core.EventPublisher.__init__(self)
            self.name = nameIn
            self.quat = math.Quaternion(math.Quaternion.IDENTITY) #Yaw
            self.PYORIENTATIONUPDATE = "ORIENTATION_UPDATE"
            
        def getName(self):
            return self.name
        
        def getForce(self):
            return self.force
            
        def setAngle(self, val):
            self.angle = val
            
        def step(self):
            #self.qVal *= math.Quaternion(math.Radian(pi/12), math.Vector3.UNIT_Y) #Pitch
            self.quat *= math.Quaternion(math.Radian(pi/6), math.Vector3.UNIT_X) #Roll
                       
        def update(self):
            self.step()     
            pyEvent = core.Event()
            pyEvent.quat = self.quat
            self.publish(self.PYORIENTATIONUPDATE, pyEvent)