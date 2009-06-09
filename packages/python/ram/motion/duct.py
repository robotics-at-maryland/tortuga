# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   packages/python/ram/motion/duct.py

# Project Imports
import ram.motion.seek as seek

class Duct(seek.PointTarget):
    def __init__(self, azimuth, elevation, range, x, y, alignment, 
                 vehicle = None):
        seek.PointTarget.__init__(self, azimuth, elevation, range, x, y, 
                                  vehicle)
        self.setState(azimuth, elevation, range, x, y, alignment,
                      publish = False)

    def setState(self, azimuth, elevation, range, x, y, alignment = 0,
                 publish = True):
        self.alignment = alignment
        seek.PointTarget.setState(self, azimuth, elevation, range, x, y, 
                                  publish)

class DuctSeekAlign(seek.SeekPointToRange):
    """
    Seeks to align with the duct at a certain range
    """
    
    def __init__(self, target, maxAlignDiff, desiredRange, maxRangeDiff,
                 maxSidewaysSpeed, alignGain = 1.0, rangeGain = 1.0, 
                 maxSpeed = 0.0, depthGain = 1, iDepthGain = 0, 
                 dDepthGain = 0, maxDepthDt = 0):
        """
        @type maxAlignDiff: float
        @param maxAlignDiff: The align difference you want your speed to max out at
        
        @type desiredRange: float
        @param desiredRange: The range you wish to be at relative to the target
    
        @type maxRangeDiff: float
        @param maxRangeDiff: The range difference you wish your speed to max out at
        """    
        seek.SeekPointToRange.__init__(self, target, desiredRange, 
                                       maxRangeDiff,
                                       rangeGain = rangeGain, 
                                       maxSpeed = maxSpeed, 
                                       depthGain = depthGain,
                                       iDepthGain = iDepthGain,
                                       dDepthGain = dDepthGain)
        self._maxAlignDiff = float(maxAlignDiff)
        self._alignGain = float(alignGain)
        self._maxSidewaysSpeed = float(maxSidewaysSpeed)
        
    def _seek(self):
        # Normal seek, handles depth, range, and orientation
        seek.SeekPointToRange._seek(self)
        
        # Now do our sideways motion
        if self._maxSidewaysSpeed != 0:
            sidewaysSpeed = self._sidewaysSpeedScale() * self._maxSidewaysSpeed
            self._controller.setSidewaysSpeed(sidewaysSpeed)
        
    def _sidewaysSpeedScale(self):
        # Use the base alignment scalling
        baseScale = seek.SeekPoint._speedScale(self)
    
        # Neg if to the right, pos if to the left 
        alignDiff = self._target.alignment * -1
        alignScale = alignDiff / self._maxAlignDiff
        
        # Find final scale, then cap
        scale = baseScale * (alignScale/self._alignGain)
        
        if (scale < 0) and (scale < -1):
            scale = -1
        elif (scale > 0) and (scale > 1):
            scale = 1
            
        return scale
    
    def stop(self):
        self._controller.setSidewaysSpeed(0)
        seek.SeekPointToRange.stop(self)
