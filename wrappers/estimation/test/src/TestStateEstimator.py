# Copyright (C) 2010 Maryland Robotics Club
# Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
# All rights reserved.
#
# Author: Jonathan Sternberg <jsternbe@umd.edu>
# File: wrapper/estimation/test/src/TestIStateEstimator.py

# STD Imports
import os
import unittest

# Project Imports
import ext.core as core
import ext.math as math
import ext.estimation as estimation

class MockStateEstimator(estimation.IStateEstimator):
    def __init__(self, cfg, deps = None):
        if deps is None:
            deps = core.SubsystemList()
        estimation.IStateEstimator.__init__(self, 'StateEstimator',
            core.Subsystem.getSubsystemOfType(core.EventHub, deps))
        self._obstacles = {}
    def getEstimatedPosition(self):
        return math.Vector2(0, 0)
    def getEstimatedVelocity(self):
        return math.Vector2(0, 0)
    def getEstimatedLinearAcceleration(self):
        return math.Vector3(0, 0, 0)
    def getEstimatedAngularRate(self):
        return math.Vector3(0, 0, 0)
    def getEstimatedOrientation(self):
        return math.Quaternion.IDENTITY
    def getEstimatedDepth(self):
        return 0
    def getEstimatedDepthDot(self):
        return 0
    def addObstacle(self, name, obstacle):
        assert isinstance(obstacle, estimation.Obstacle),\
            'Only obstacles can be added to a state estimator'
        assert not self._obstacles.has_key(name), 'Duplicate entry'
        self._obstacles[name] = obstacle
    def getObstaclePosition(self, name):
        return self._obstacles[name].getPosition()
    def getObstacleDepth(self, name):
        return self._obstacles[name].getDepth()

core.SubsystemMaker.registerSubsystem('MockStateEstimator', MockStateEstimator)

class TestIStateEstimator(unittest.TestCase):
    def testMaker(self):
        cfg = {
            'name' : 'TestEstimator',
            'type' : 'MockStateEstimator'
            }
        cfg = core.ConfigNode.fromString(str(cfg))
        estimator = core.SubsystemMaker.newObject(cfg, core.SubsystemList())

        self.assert_(isinstance(estimator, estimation.IStateEstimator))

    def testObstacle(self):
        estimator = MockStateEstimator({})
        obstacle = estimation.Obstacle()
        desiredPos = math.Vector2(5, -5)
        obstacle.setPosition(desiredPos)
        estimator.addObstacle(estimation.Obstacle.RED_BUOY, obstacle)

        self.assertEquals(desiredPos, estimator.getObstaclePosition(estimation.Obstacle.RED_BUOY))

        depth = estimator.getObstacleDepth(estimation.Obstacle.RED_BUOY)

if __name__ == '__main__':
    unittest.main()
