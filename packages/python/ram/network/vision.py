# Copyright (C) 2010 Maryland Robotics Club
# Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
# All rights reserved.
#
# Author: Jonathan Sternberg <jsternbe@umd.edu>
# File:  packages/python/ram/network/vision.py

# Library Imports
import Ice

# Project Imports
import ext.core
import ext.vision
import proxy.vision

class VisionSystemProxy(ext.vision.VisionSystem):
    def __init__(self, config, deps = None):
        cfg = ext.core.ConfigNode.fromString(str(config))
        depList = ext.core.SubsystemList()
        for subsys in deps:
            depList.append(subsys)

        # The cameras do nothing    
        ext.vision.VisionSystem.__init__(self, ext.vision.Camera(640, 480),
                                         ext.vision.Camera(640, 480), cfg,
                                         depList)

        # Initialize Ice runtime
        self._ic = Ice.initialize()
        host = config.get('host', 'localhost')
        port = config.get('port', '10000')
        base = self._ic.stringToProxy(
            'VisionSystem:default -h %s -p %s' % (host, port))
        self._impl = proxy.vision.VisionSystemPrx.checkedCast(base)

    def __del__(self):
        self._ic.destroy()

    def redLightDetectorOn(self):
        self._impl.redLightDetectorOn()
    def redLightDetectorOff(self):
        self._impl.redLightDetectorOff()
    def buoyDetectorOn(self):
        self._impl.buoyDetectorOn()
    def buoyDetectorOff(self):
        self._impl.buoyDetectorOff()
    def binDetectorOn(self):
        self._impl.binDetectorOn()
    def binDetectorOff(self):
        self._impl.binDetectorOff()
    def pipeLineDetectorOn(self):
        self._impl.pipeLineDetectorOn()
    def pipeLineDetectorOff(self):
        self._impl.pipeLineDetectorOff()
    def ductDetectorOn(self):
        self._impl.ductDetectorOn()
    def ductDetectorOff(self):
        self._impl.ductDetectorOff()
    def downwardSafeDetectorOn(self):
        self._impl.downwardSafeDetectorOn()
    def downwardSafeDetectorOff(self):
        self._impl.downwardSafeDetectorOff()
    def gateDetectorOn(self):
        self._impl.gateDetectorOn()
    def gateDetectorOff(self):
        self._impl.gateDetectorOff()
    def targetDetectorOn(self):
        self._impl.targetDetectorOn()
    def targetDetectorOff(self):
        self._impl.targetDetectorOff()
    def windowDetectorOn(self):
        self._impl.windowDetectorOn()
    def windowDetectorOff(self):
        self._impl.windowDetectorOff()
    def barbedWireDetectorOn(self):
        self._impl.barbedWireDetectorOn()
    def barbedWireDetectorOff(self):
        self._impl.barbedWireDetectorOff()
    def hedgeDetectorOn(self):
        self._impl.hedgeDetectorOn()
    def hedgeDetectorOff(self):
        self._impl.hedgeDetectorOff()
    def velocityDetectorOn(self):
        self._impl.velocityDetectorOn()
    def velocityDetectorOff(self):
        self._impl.velocityDetectorOff()

ext.core.SubsystemMaker.registerSubsystem('VisionSystemProxy',
                                          VisionSystemProxy)
