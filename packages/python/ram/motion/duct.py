# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   packages/python/ram/motion/duct.py

# Project Imports
import ram.motion.seek as seek

class AirDuct(seek.PointTarget):
    def __init__(self, azimuth, elevation, range, x, y, alignment):
        ext.core.EventPublisher.__init__(self)
        self.setState(azimuth, elevation, range, x, y, alignement,
                      publish = False)

    def setState(self, azimuth, elevation, range, x, y, alignment,
                 publish = True):
        self.alignment = alignment
        self.setState(azimuth, elevation, range, x, y, publish)

class Align(seek.SeekPointToRange):
    """
    Seeks to align with the duct at a certain range
    """
    
    def __init__(self, target, maxAlignDiff, desiredRange, maxRangeDiff,
                 maxSidewaysSpeed, alignGain = 1.0, rangeGain = 1.0, 
                 maxSpeed = 0.0, depthGain = 1):
        """
        @type maxAlignDiff: float
        @param maxAlignDiff: The align difference you want your speed to max out at
        
        @type desiredRange: float
        @param desiredRange: The range you wish to be at relative to the target
    
        @type maxRangeDiff: float
        @param maxRangeDiff: The range difference you wish your speed to max out at
        """    
        seek.SeekPointToRange.__init__(target, desiredRange, maxRangeDiff,
                                       rangeGain, maxSpeed, depthGain)
        self._maxAlignDiff = maxAlignDiff
        self._alignGain = alignGain
        self._maxSidewaysSpeed = maxSidewaysSpeed
        
    def _sidewaysSpeedScale(self):
        baseScale = SeekPoint._speedScale(self)
    
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