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

# Library Imports
import ogre.renderer.OGRE as ogre

# Project Imports
import ext.core as core
import ext.vehicle as vehicle
import ext.math as math

def convertToVector3(vType, vector):
    return vType(vector.x, vector.y, vector.z)

def convertToQuaternion(qType, quat):
    return qType(quat.x, quat.y, quat.z, quat.w)


class Vehicle(vehicle.IVehicle):
    def __init__(self, robot):
        vehicle.IVehicle.__init__(self, "SimVehicle")
        self.robot = robot
    
    def getDepth(self):
        #print 'Depth',self.robot._main_part._node.position.z
        return -self.robot._main_part._node.position.z
    
    def getOrientation(self):
        return convertToQuaternion(math.Quaternion,
                                   self.robot._main_part._node.orientation)
    
    def getLinearAcceleration(self):
        return convertToVector3(math.Vector3,
                                self.robot._main_part.acceleration)
    
    def getAngularRate(self):
        return convertToVector3(math.Vector3,
                                self.robot._main_part.angular_accel)
    
    def applyForcesAndTorques(self, force, torque):
        self.robot._main_part.set_local_force(
            convertToVector3(ogre.Vector3, force), (0,0,0))
        self.robot._main_part.torque = convertToVector3(ogre.Vector3, torque)
        
        # TODO: Fix and check me
#        star = force[2] / 2 + 0.5 * torque[1] #/ 0.1905
#        port = force[2] / 2 - 0.5 * torque[1] #/ 0.1905
#        fore = force[1] / 2 + 0.5 * torque[0] #/ 0.3366
#        aft = force[1] / 2 - 0.5 * torque[0] #/ 0.3366        
#            
#        self.robot.parts.right_thruster.force = star
#        self.robot.parts.left_thruster.force = port
#        self.robot.parts.front_thruster.force = fore
#        self.robot.parts.aft_thruster.force = aft

core.SubsystemMaker.registerSubsystem('SimVehicle', Vehicle)