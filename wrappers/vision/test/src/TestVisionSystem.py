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

    if platform.system() == "Linux":
        
        def testRedLightDetector(self):
            # Create a vision system with two mock cameras and an EventHub
            cfg = { 'testing' : 1 }
            cfg = core.ConfigNode.fromString(str(cfg))
            
            forwardCamera = vision.Camera(640,480)
            backwardCamera = vision.Camera(640,480)
    
            eventHub = core.EventHub()
            deps = core.SubsystemList()
            deps.append(eventHub)
            visionSystem = vision.VisionSystem(forwardCamera, backwardCamera, cfg,
                                               deps)
    
            # Subscribe to our events about the red light
            eventHub.subscribeToType(vision.EventType.LIGHT_FOUND,
                                     self.redFoundHandler)
            eventHub.subscribeToType(vision.EventType.LIGHT_LOST,
                                     self.redLostHandler)
    
            # Load our test image (really upper right)
            image = vision.Image.loadFromFile(
                os.path.join(getConfigRoot(), 'red_light_upper_left.png'))

            # Start detector then unbackground it
            visionSystem.redLightDetectorOn()
            visionSystem.unbackground(True)

            forwardCamera.capturedImage(image)
            forwardCamera.background(0)
    
            # This stops the background thread
            visionSystem.update(0)
            visionSystem.redLightDetectorOff()
    
            # Check the event
            self.assert_(self.found)
            self.assert_(self.event)
            self.assertAlmostEqual(0.5 * 4.0/3.0, self.event.x, 2)
            self.assertAlmostEqual(0.5, self.event.y, 2)
            self.assertAlmostEqual(3, self.event.range, 1)
            self.assertAlmostEqual(-78.0/4, self.event.azimuth.valueDegrees(),
                                   2)
            self.assertAlmostEqual(105.0/4, self.event.elevation.valueDegrees(),
                                   0)


if __name__ == '__main__':
    unittest.main()
