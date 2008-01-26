# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/motion.py

# STD Imports
import unittest

# Project Imports
import ram.motion as motion
import ext.control as control
import ext.vehicle as vehicle
import ext.core as core
import ext.math

# Mock subsystems
class MockController(control.IController):
    def __init__(self, eventHub):
        control.IController.__init__(self, "A", eventHub)
        self.depth = 0
        self.yawChange = 0
        
    def setDepth(self, depth):
        self.depth = depth
        
    def yawVehicle(self, yawChange):
        self.yawChange = yawChange
        
    def publishAtDepth(self, vehicleDepth):
        event = ext.math.NumericEvent()
        event.number = vehicleDepth
        self.publish(control.IController.AT_DEPTH, event)
        
    def publishAtOrientation(self, vehicleOrientation):
        event = ext.math.OrientationEvent()
        event.orientation = vehicleOrientation
        self.publish(control.IController.AT_ORIENTATION, event)
        
        
class MockVehicle(vehicle.IVehicle):
    def __init__(self):
        vehicle.IVehicle.__init__(self, "B")
        self.depth = 0
        self.orientation = ext.math.Quaternion.IDENTITY
        
    def getDepth(self):
        return self.depth
    
    def getOrientation(self):
        return self.orientation

# Mock Motion
class MockMotion(object):
    def __init__(self):
        self.controller = None
        self.vehicle = None
        self.stoped = False
    
    def start(self, controller, vehicle, eventHub, eventPublisher):
        self.controller = controller
        self.vehicle = vehicle
        
    def stop(self):
        self.stoped = True

class TestMotionManager(unittest.TestCase):
    def setUp(self):
        # Create the event hub to collect all published events
        self.eventHub = core.EventHub()
        self.vehicle = MockVehicle()
        self.controller = MockController(self.eventHub)
        
        # The QueuedEventHub lets us queue the events to be released when ready
        self.qeventHub = core.QueuedEventHub(self.eventHub)
        
        deps = [self.vehicle, self.controller, self.qeventHub, self.eventHub]
        self.motionManager = motion.MotionManager({}, deps)

    def testSetMotion(self):
        m = MockMotion()
        self.motionManager.setMotion(m)
        
        self.assertEquals(self.vehicle.getName(), m.vehicle.getName())
        self.assertEquals(self.controller.getName(), m.controller.getName())
        
        m2 = MockMotion()
        self.motionManager.setMotion(m2)
        self.assert_(m.stoped)
        self.assertEquals(self.vehicle.getName(), m2.vehicle.getName())
        self.assertEquals(self.controller.getName(), m2.controller.getName())
  
  
class TestChangeDepth(unittest.TestCase):
    def setUp(self):
        # Create the event hub to collect all published events
        self.eventHub = core.EventHub()
        self.vehicle = MockVehicle()
        self.controller = MockController(self.eventHub)
        
        # The QueuedEventHub lets us queue the events to be released when ready
        self.qeventHub = core.QueuedEventHub(self.eventHub)
        
        deps = [self.vehicle, self.controller, self.qeventHub, self.eventHub]
        self.motionManager = motion.MotionManager({}, deps)
    
        self.motionFinished = False

    def handleFinished(self, event):
        self.motionFinished = True
        
        
    def testDive(self):
        self.vehicle.depth = 5
        m = motion.ChangeDepth(10, 5) 
        self.qeventHub.subscribeToType(motion.Motion.FINISHED, 
                                       self.handleFinished)
        
        # First step
        self.motionManager.setMotion(m)
        
        for i in xrange(6,11):
            # Make sure we didn't finish early
            self.assert_(not self.motionFinished)
            # Make sure the proper depth was commanded
            self.assertEqual(i, self.controller.depth)
            # Say we have reached the depth to keep going
            self.controller.publishAtDepth(i)
            self.qeventHub.publishEvents()
            
        self.assert_(self.motionFinished)
            
    def testSurface(self):
        self.vehicle.depth = 10
        m = motion.ChangeDepth(5, 5) 
        self.qeventHub.subscribeToType(motion.Motion.FINISHED, 
                                      self.handleFinished)
        
        # First step
        self.motionManager.setMotion(m)
        
        for i in reversed(xrange(5,10)):
            # Make sure we didn't finish early
            self.assert_(not self.motionFinished)
            # Make sure the proper depth was commanded
            self.assertEqual(i, self.controller.depth)
            # Say we have reached the depth to keep going
            self.controller.publishAtDepth(i)
            self.qeventHub.publishEvents()
            
        self.assert_(self.motionFinished)
        
class TestChangeHeading(unittest.TestCase):
    def setUp(self):
        # Create the event hub to collect all published events
        self.eventHub = core.EventHub()
        self.vehicle = MockVehicle()
        self.controller = MockController(self.eventHub)
        
        # The QueuedEventHub lets us queue the events to be released when ready
        self.qeventHub = core.QueuedEventHub(self.eventHub)
        
        deps = [self.vehicle, self.controller, self.qeventHub, self.eventHub]
        self.motionManager = motion.MotionManager({}, deps)
    
        self.motionFinished = False

    def handleFinished(self, event):
        self.motionFinished = True
        
        
    def testLeft(self):
        m = motion.ChangeHeading(30, 5) 
        self.qeventHub.subscribeToType(motion.Motion.FINISHED, 
                                       self.handleFinished)
        
        # First step
        self.motionManager.setMotion(m)
        
        for i in xrange(6, 36, 6):
            # Make sure we didn't finish early
            self.assert_(not self.motionFinished)
            # Make sure the proper depth was commanded
            self.assertAlmostEqual(6, self.controller.yawChange, 2)
            # Say we have reached the depth to keep going
            self.controller.publishAtOrientation(
                ext.math.Quaternion(ext.math.Radian(ext.math.Degree(i)),
                                    ext.math.Vector3.UNIT_Z))
            self.qeventHub.publishEvents()
            
        self.assert_(self.motionFinished)
            
            
    def testRight(self):
        m = motion.ChangeHeading(-30, 5) 
        self.qeventHub.subscribeToType(motion.Motion.FINISHED, 
                                       self.handleFinished)
        
        # First step
        self.motionManager.setMotion(m)
        
        for i in xrange(6, 36, 6):
            # Make sure we didn't finish early
            self.assert_(not self.motionFinished)
            # Make sure the proper depth was commanded
            self.assertAlmostEqual(-6, self.controller.yawChange, 2)
            # Say we have reached the depth to keep going
            self.controller.publishAtOrientation(
                ext.math.Quaternion(ext.math.Radian(ext.math.Degree(-i)),
                                    ext.math.Vector3.UNIT_Z))
            self.qeventHub.publishEvents()
            
        self.assert_(self.motionFinished)
            
if __name__ == '__main__':
    unittest.main()
        
