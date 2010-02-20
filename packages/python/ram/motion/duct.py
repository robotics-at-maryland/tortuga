# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   packages/python/ram/motion/duct.py

# Project Imports
import ram.motion.seek as seek
import ram.timer

class Duct(seek.PointTarget):
    def __init__(self, azimuth, elevation, range, x, y, alignment, 
                 timeStamp = None, vehicle = None):
        seek.PointTarget.__init__(self, azimuth, elevation, range, x, y, 
                                  timeStamp, vehicle)
        self.prevAlignment = None
        self.alignment = alignment

    def setState(self, azimuth, elevation, range, x, y, alignment,
                 timeStamp, publish = True):
        # Store the old values
        self.prevAlignment = self.alignment

        # Store the new values
        self.alignment = alignment

        seek.PointTarget.setState(self, azimuth, elevation, range, x, y, 
                                  timeStamp, publish)

    def changeOverTime(self):
        diff = seek.PointTarget.changeOverTime(self)
        if self.prevTimeStamp is not None:
            diffAlignment = self.alignment - self.prevAlignment
            diffTime = self.timeStamp - self.prevTimeStamp

            return diff + ((diffAlignment / diffTime),)
        else:
            return diff + (float('inf'),)

class DuctSeekAlign(seek.SeekPointToRange):
    """
    Seeks to align with the duct at a certain range
    """
    
    def __init__(self, target, maxAlignDiff, desiredRange, maxRangeDiff,
                 maxSidewaysSpeed, alignGain = 1.0, rangeGain = 1.0, 
                 maxSpeed = 0.0, depthGain = 1, iDepthGain = 0, 
                 dDepthGain = 0, maxDepthDt = 0, yawGain = 1.0):
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
                                       dDepthGain = dDepthGain,
                                       yawGain = yawGain)
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
        scale = baseScale * (alignScale * self._alignGain)
        
        if (scale < 0) and (scale < -1):
            scale = -1
        elif (scale > 0) and (scale > 1):
            scale = 1
            
        return scale
    
    def stop(self):
        self._controller.setSidewaysSpeed(0)
        seek.SeekPointToRange.stop(self)
