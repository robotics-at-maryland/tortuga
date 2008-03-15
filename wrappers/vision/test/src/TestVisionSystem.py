# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File: wrapper/vision/test/src/TestImport.py

# STD Imports
import os
import unittest
import platform

# Project Imports
import ext.core as core
import ext.vision as vision
import ext.math as math
import ram.timer as timer

def getConfigRoot():
    root = os.environ['RAM_SVN_DIR']
    return os.path.join(root, 'wrappers', 'vision', 'test', 'data')

class TestVisionSystem(unittest.TestCase):
    def setUp(self):
        self.found = False
        self.event = None
    
    def testCreate(self):
        cfg = { 'name' : 'Test', 'type' : 'TestSubsystem' }
        cfg = core.ConfigNode.fromString(str(cfg))
        deps = core.SubsystemList()

        cam1 = vision.Camera(640,480)
        cam2 = vision.Camera(640,480)
        visionSystem = vision.VisionSystem(cam1, cam2, cfg, deps)      

    def redFoundHandler(self, event):
        self.found = True
        self.event = event

    def redLostHandler(self, event):
        self.found = False
        self.event = event

    if platform.system() == "DON'T RUN":
        
        def testRedLightDetector(self):
            # Create a vision system with two mock cameras and an EventHub
            cfg = { 'name' : 'Test', 'type' : 'TestSubsystem' }
            cfg = core.ConfigNode.fromString(str(cfg))
            
            forwardCamera = vision.Camera(640,480)
            backwardCamera = vision.Camera(640,480)
    
            eventHub = core.EventHub()
            deps = core.SubsystemList()
            deps.append(eventHub)
            visionSystem = vision.VisionSystem(forwardCamera, backwardCamera, cfg,
                                               deps)
    
            # Subscribe to our events about the red light
            qeventHub = core.QueuedEventHub(eventHub)
            qeventHub.subscribeToType(vision.EventType.LIGHT_FOUND,
                                     self.redFoundHandler)
            qeventHub.subscribeToType(vision.EventType.LIGHT_LOST,
                                     self.redLostHandler)
    
            # Load our test image
            image = vision.Image.loadFromFile(
                os.path.join(getConfigRoot(), 'red_light_upper_left.png'))
            
            # Have to wait for the processing thread to be waiting on the camera
            visionSystem.redLightDetectorOn()
            timer.sleep(0.033)
    
            # Release a new image from the camera (and wait for the detector
            # capture it)
            forwardCamera.capturedImage(image)
            timer.sleep(0.033)
    
            # This stops the background thread
            visionSystem.redLightDetectorOff()
    
            # Check the event
            qeventHub.publishEvents()
            self.assert_(self.found)
            self.assert_(self.event)
            self.assertAlmostEqual(-0.5, self.event.x, 2)
            self.assertAlmostEqual(0.5, self.event.y, 2)
            self.assertAlmostEqual(3, self.event.range, 1)
            self.assertAlmostEqual(78.0/4, self.event.azimuth.valueDegrees(), 2)
            self.assertAlmostEqual(105.0/4, self.event.elevation.valueDegrees(), 0)


if __name__ == '__main__':
    unittest.main()
