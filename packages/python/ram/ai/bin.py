# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/ai/bin.py

"""
Currently hovers over the bin
"""

# STD Imports
import math as pmath

# Project Imports
import ext.core as core
import ext.vision as vision
import ext.math as math

import ram.ai.state as state
import ram.ai.tracking as tracking
import ram.motion as motion
import ram.motion.basic
import ram.motion.search
#import ram.motion.common
import ram.motion.pipe # For the maneuvering motions

COMPLETE = core.declareEventType('COMPLETE')

def ensureBinTracking(qeventHub, ai):        
    tracking.ensureItemTracking(qeventHub, ai, 'binData',
                                vision.EventType.BIN_FOUND,
                                vision.EventType.BIN_DROPPED)
    ai.data['binData'].setdefault('histogram', {})

class HoveringState(state.State):
    """
    Base state for hovering over the bins or the array of bins.  It can use 
    either angle of the entire array, or of the current bin.
    """
    
    LOST_CURRENT_BIN = core.declareEventType('LOST_CURRENT_BIN')
    
    @staticmethod
    def transitions(myState, trans = None, lostState = None):
        if lostState is None:
            lostState = Recover
        if trans is None:
            trans = {}
        trans.update({vision.EventType.BINS_LOST : lostState,
                      vision.EventType.BIN_FOUND : myState,
                      vision.EventType.MULTI_BIN_ANGLE : myState,
                      vision.EventType.BIN_DROPPED : myState})
        return trans
    
    def _currentBin(self, event):
        return self.ai.data['binData'].get('currentID', 0) == event.id
    
    def MULTI_BIN_ANGLE(self, event):
        self._multiAngle = event.angle
    
    def BIN_FOUND(self, event):
        """Update the state of the light, this moves the vehicle"""
        # Set up the histogram. setdefault will make sure that it sets up
        # the histogram if it isn't set up and will get the histogram if
        # it already exists
        histogram = \
            self.ai.data['binData']['histogram'].setdefault(
                event.id, {})
        histogram.setdefault('totalHits', 0)
        if event.symbol != vision.Symbol.UNKNOWN:
            histogram[event.symbol] = histogram.get(event.symbol, 0) + 1
            histogram['totalHits'] = histogram.get('totalHits', 0) + 1

        if not self._useMultiAngle:
            # Use only the bin angle
            if self._first:
                self._first = False
                self._lastAngle = event.angle
            else:
                # Only allow sane angles (less then 95)
                if pmath.fabs(event.angle.valueDegrees()) < 95:
                    # Don't accept a new angle if it varies to much from the 
                    # last given angle
                    lastDegree = self._lastAngle.valueDegrees()
                    currentDegree = event.angle.valueDegrees()
                    if (pmath.fabs(lastDegree - currentDegree) > self._filterLimit):
                        event.angle = math.Degree(lastDegree)
                    else:
                        self._lastAngle = event.angle
        else:
            # Using the array of bin angle instead
            event.angle = self._multiAngle
        
        # Only listen to the current bin ID
        if self._currentBin(event):
            self._bin.setState(event.x, event.y, event.angle)
            
            self.ai.data["lastBinX"] = event.x
            self.ai.data["lastBinY"] = event.y

    def BIN_DROPPED(self, event):
        # Cleanup the histogram data
        histogram = self.ai.data['binData']['histogram']

        if histogram.has_key(event.id):
            del histogram[event.id]

    def enter(self, useMultiAngle = False):
        """
        Use multiAngle determines whether or not you listen to the bin angle
        or the angle of the array of the bins
        """
            
        # Make sure we are tracking
        ensureBinTracking(self.queuedEventHub, self.ai)
        
        self._useMultiAngle = useMultiAngle
        self._multiAngle = math.Degree(0)
        self._first = True
        self._filterLimit = self._config.get('filterLimit', 75)
        
        self._bin = ram.motion.pipe.Pipe(0,0,0)
        sidewaysSpeedGain = self._config.get('sidewaysSpeedGain',3)
        speedGain = self._config.get('speedGain', 5)
        yawGain = self._config.get('yawGain', 1)
        maxSpeed = self._config.get('maxSpeed', 5)
        maxSidewaysSpeed = self._config.get('maxSidewaysSpeed', 3)
        
        iSpeedGain = self._config.get('iSpeedGain', 0)
        iSidewaysSpeedGain = self._config.get('iSidewaysSpeedGain', 0)
        dSpeedGain = self._config.get('dSpeedGain', 0)
        dSidewaysSpeedGain = self._config.get('dSidewaysSpeedGain', 0)
        
        motion = ram.motion.pipe.Hover(pipe = self._bin,
                                       maxSpeed = maxSpeed,
                                       maxSidewaysSpeed = maxSidewaysSpeed,
                                       sidewaysSpeedGain = sidewaysSpeedGain,
                                       speedGain = speedGain,
                                       yawGain = yawGain,
                                       iSpeedGain = iSpeedGain,
                                       iSidewaysSpeedGain = iSidewaysSpeedGain,
                                       dSpeedGain = dSpeedGain,
                                       dSidewaysSpeedGain = dSidewaysSpeedGain)
        self.motionManager.setMotion(motion)

    def exit(self):
        #print '"Exiting Seek, going to follow"'
        self.motionManager.stopCurrentMotion()

class SettlingState(HoveringState):
    """
    A specialization of the hover state which hovers for the given time.
    """
    def enter(self, eventType, eventTime, useMultiAngle = False):
        self.timer = self.timerManager.newTimer(eventType, eventTime)
        self.timer.start()
        
        HoveringState.enter(self, useMultiAngle)

    def exit(self):
        HoveringState.exit(self)
        self.timer.stop()

class BinSortingState(HoveringState):
    """
    A specialization of the hover state which lets the user shift the bin
    currently being hovered over to left or right most bin.  
    """
    
    LEFT = 1
    RIGHT = 2
    
    CENTERED_ = core.declareEventType('CENTERED')
    
    def BIN_FOUND(self, event):
        HoveringState.BIN_FOUND(self, event)

        # Fire event if we are centered over the bin
        if self._currentBin(event): 
            if math.Vector2(event.x, event.y).length() < self._centeredRange:
                self.publish(BinSortingState.CENTERED_, core.Event())
    
    def enter(self, direction, useMultiAngle = False):
        """
        @param direction: Says whether or you want to go left or right with the 
                          bins
        """
        if (direction != BinSortingState.LEFT) and (direction != BinSortingState.RIGHT):
            raise Exception("ERROR Wrong Direction")
        self._direction = direction
        
        self._centeredRange = self._config.get('centeredRange', 0.2)
        
        HoveringState.enter(self, useMultiAngle = useMultiAngle)
    
    def fixEdgeBin(self):
        """
        Makes the current bin the left/right most bin, returns true if that
        changes the current bin.
        
        @rtype: bool
        @return: True if we changed to a new Bin, False if not.
        """
        sortedBins = self._getSortedBins()
        
        # Compare to current ID
        currentBinId = self.ai.data['binData']['currentID']
        mostEdgeBinId = self._getNextBin(sortedBins, currentBinId)
        
        if mostEdgeBinId is None:
            return True
        elif (currentBinId == mostEdgeBinId):
            # We found the "end" bin
            return False
        else:
            # Still more bins to go
            self.ai.data['binData']['currentID'] = mostEdgeBinId
            return True

    def hasBinToRight(self):
        currentBinId = self.ai.data['binData']['currentID']
        sortedBins = self._getSortedBins()
        
        if self._direction == BinSortingState.RIGHT:
            return currentBinId != sortedBins[0]
        else:
            return currentBinId != sortedBins[len(sortedBins)-1]
    
    def hasBinToLeft(self):
        currentBinId = self.ai.data['binData']['currentID']
        sortedBins = self._getSortedBins()
        
        if self._direction == BinSortingState.LEFT:
            return currentBinId != sortedBins[0]
        else:
            return currentBinId != sortedBins[len(sortedBins)-1]
    
    def setSortDirection(self, direction):
        if (direction != BinSortingState.LEFT) and (direction != BinSortingState.RIGHT):
            raise Exception("ERROR Wrong Direction")
        self._direction = direction
    
    def setStartSideDirection(self, direction):
        if (direction != BinSortingState.LEFT) and (direction != BinSortingState.RIGHT):
            raise Exception("ERROR Wrong Direction")
        self.ai.data['startSide'] = direction
    
    def _compareBins(self, idA, idB):
        """
        Sorts the list with the left most bin, at the start
        
        @type idA: int
        @param idA: ID of the bin compare
        
        @type idB: int
        @param idB: ID of the other bin to compare
        """
        binData = self.ai.data['binData']['itemData']
        binAx = binData[idA].x
        binBx = binData[idB].x
        
        if self._direction == BinSortingState.LEFT:
            if binAx < binBx:
                return -1
            elif binAx > binBx:
                return 1
        elif self._direction == BinSortingState.RIGHT:
            if binAx > binBx:
                return -1
            elif binAx < binBx:
                return 1
        return 0
        #return type(binAx).__cmp__(binAx, binBx)
    
    def _getSortedBins(self):
        """
        Returns the bins sorted based desired direction, left/right
        """
        currentBins = [b for b in self.ai.data['binData']['currentIds']]
        sortedBins = sorted(currentBins, self._compareBins)
        return sortedBins
    
    def _getNextBin(self, sortedBins, currentBinId):
        """
        Returns the next bin out of the sorted bin list, returns currentBinId
        if thats there is no next bin.
        """
        # Compare to current ID
        currentBinId = self.ai.data['binData']['currentID']
        if len(sortedBins) == 0:
            #event = vision.BinEvent(0, 0, vision.Symbol.UNKNOWN, math.Degree(0))
            self.publish(vision.EventType.BINS_LOST, core.Event())
            return None
        else:
            mostEdgeBinId = sortedBins[0]
            return mostEdgeBinId
    
class Start(state.State):
    """
    Gets us to proper depth to start the actual bin search.
    """
    @staticmethod
    def transitions():
        return { motion.basic.Motion.FINISHED : Searching }
    
    def enter(self):
        # Go to 5 feet in 5 increments
        diveMotion = motion.basic.RateChangeDepth(
            desiredDepth = self._config.get('depth', 7),
            speed = self._config.get('speed', 1.0/3.0))
        self.motionManager.setMotion(diveMotion)
        
    def exit(self):
        self.motionManager.stopCurrentMotion()

class Searching(state.State):
    """When the vehicle is looking for a bin"""
    @staticmethod
    def transitions():
        return { vision.EventType.BIN_FOUND : Seeking }

    def BIN_FOUND(self, event):
        self.ai.data['binData']['currentID'] = event.id
        self.ai.data['binData']['currentIds'] = set()
        if event.symbol != vision.Symbol.UNKNOWN:
            histogram = \
                self.ai.data['binData']['histogram'].setdefault(
                    event.id, dict())
            histogram[event.symbol] = histogram.get(event.symbol, 0) + 1
            histogram['totalHits'] = histogram.get('totalHits', 0) + 1

    def enter(self):
        ensureBinTracking(self.queuedEventHub, self.ai)
        
        # Turn on the vision system
        self.visionSystem.binDetectorOn()

        # Save cruising depth for later surface
        self.ai.data['preBinCruiseDepth'] = self.controller.getDepth()

        # Create zig zag search to 
        zigZag = motion.search.ForwardZigZag(
            legTime = self._config.get('legTime', 5),
            sweepAngle = self._config.get('sweepAngle', 45),
            speed = self._config.get('speed', 2.5))

        self.motionManager.setMotion(zigZag)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class Seeking(HoveringState):
    """When the vehicle is moving over the found bin"""
    BIN_CENTERED = core.declareEventType("BIN_CENTERED")
    
    @staticmethod
    def transitions():
        return HoveringState.transitions(Seeking,
            { Seeking.BIN_CENTERED : Centering })
 
    def _compareByDistance(self, idA, idB):
        """
        Sorts the list with the closest bin at the start
        
        @type idA: int
        @param idA: ID of the bin compare
        
        @type idB: int
        @param idB: ID of the other bin to compare
        """
        binData = self.ai.data['binData']['itemData']
        binADistance = math.Vector2(binData[idA].x, binData[idA].y).length()
        binBDistance = math.Vector2(binData[idB].x, binData[idB].y).length()
        
        if binADistance < binBDistance:
            return -1
        else:
            return 1
 
    def BIN_FOUND(self, event):
        eventDistance = math.Vector2(event.x, event.y).length()
        
        # Change the currentID to the closest bin
        currentBins = [b for b in self.ai.data['binData']['currentIds']]
        if len(currentBins) > 0:
            binIDs = sorted(currentBins, self._compareByDistance)
            self.ai.data['binData']['currentID'] = binIDs[0]
            
            # If the new event is closer, then it is our current ID
            currentIdEvent = self.ai.data['binData']['itemData'][binIDs[0]]
            if eventDistance < math.Vector2(currentIdEvent.x,
                                            currentIdEvent.y).length():
                self.ai.data['binData']['currentID'] = event.id
        
        # Disable angle tracking
        event.angle = math.Degree(0)
        HoveringState.BIN_FOUND(self, event)
        
        if self._currentBin(event):
            if eventDistance < self._centeredLimit:
                self.publish(Seeking.BIN_CENTERED, core.Event())
        
    def enter(self):
        HoveringState.enter(self)
        self._centeredLimit = self._config.get('centeredLimit', 0.2)

class Recover(state.FindAttempt):
    
    MOVE_ON = core.declareEventType('MOVE_ON')
    
    @staticmethod
    def transitions(foundState = Seeking):
        return state.FindAttempt.transitions(vision.EventType.BIN_FOUND,
                                             foundState, Searching)

    def BIN_FOUND(self, event):
        self.ai.data['binData']['currentID'] = event.id
        self.ai.data['binData']['currentIds'] = set()
        if event.symbol != vision.Symbol.UNKNOWN:
            histogram = \
                self.ai.data['binData']['histogram'].setdefault(
                    event.id, dict())
            histogram[event.symbol] = histogram.get(event.symbol, 0) + 1
            histogram['totalHits'] = histogram.get('totalHits', 0) + 1

    def enter(self, timeout = 4):
        self._timeout = self._config.get('timeout', timeout)
        state.FindAttempt.enter(self, self._timeout)

        ensureBinTracking(self.queuedEventHub, self.ai)
        
        self._bin = ram.motion.common.Target(self.ai.data["lastBinX"],
                                             self.ai.data["lastBinY"])

        speedGain = self._config.get('speedGain', 5)
        sidewaysSpeedGain = self._config.get('sidewaysSpeedGain',5)
        #yawGain = self._config.get('yawGain', 1)
        maxSpeed = self._config.get('maxSpeed', 1.5)
        maxSidewaysSpeed = self._config.get('maxSidewaysSpeed', 1.5)
        
        motion = ram.motion.common.Hover(target = self._bin,
                                         maxSpeed = maxSpeed,
                                         maxSidewaysSpeed = maxSidewaysSpeed,
                                         sidewaysSpeedGain = sidewaysSpeedGain,
                                         speedGain = speedGain)
        self.motionManager.setMotion(motion)
         

        motion = ram.motion.basic.RateChangeDepth(
	        self.ai.data.get("preBinCruiseDepth", 7), 0.3)
        self.motionManager.setMotion(motion)

    def exit(self):
        state.FindAttempt.exit(self)
        self.motionManager.stopCurrentMotion()

class RecoverCentering(Recover):
    @staticmethod
    def transitions():
        return Recover.transitions(Centering)
class RecoverCheckEnd(Recover):
    @staticmethod
    def transitions():
        return Recover.transitions(CheckEnd)
class RecoverSeekEnd(Recover):
    @staticmethod
    def transitions():
        return Recover.transitions(SeekEnd)
class RecoverDive(Recover):
    @staticmethod
    def transitions():
        trans = Recover.transitions(Dive)
        trans.update({ Recover.MOVE_ON : SurfaceToMove })
        
        return trans
    def enter(self):
        Recover.enter(self, timeout = self._config.get('timeout', 4))
        self._increase = self._config.get('increase', 0.25)
        self._maxIncrease = self._config.get('maxIncrease', 1)
        self.ai.data['dive_offsetTheOffset'] = \
            self.ai.data.get('dive_offsetTheOffset', 0) + self._increase
            
        if self.ai.data['dive_offsetTheOffset'] > 1:
            self.publish(Recover.MOVE_ON, core.Event())
class RecoverAligning(Recover):
    @staticmethod
    def transitions():
        return Recover.transitions(Aligning)
class RecoverExamine(Recover):
    pass
class RecoverPreDiveExamine(RecoverExamine):
    @staticmethod
    def transitions():
        return RecoverExamine.transitions(foundState = PreDiveExamine)
class RecoverPostDiveExamine(RecoverExamine):
    @staticmethod
    def transitions():
        return RecoverExamine.transitions(foundState = PostDiveExamine)
class RecoverCloserLook(Recover):
    @staticmethod
    def transitions():
        trans = Recover.transitions(CloserLook)
        trans.update({ Recover.MOVE_ON : SurfaceToMove })
        
        return trans
    def enter(self):
        Recover.enter(self, timeout = self._config.get('timeout', 4))
        self._increase = self._config.get('increase', 0.25)
        self._maxIncrease = self._config.get('maxIncrease', 1)
        self.ai.data['closerlook_offsetTheOffset'] = \
            self.ai.data.get('closerlook_offsetTheOffset', 0) + self._increase
            
        if self.ai.data['closerlook_offsetTheOffset'] > 1:
            self.publish(Recover.MOVE_ON, core.Event())
class RecoverSettleBeforeDrop(Recover):
    @staticmethod
    def transitions():
        return Recover.transitions(SettleBeforeDrop)
class RecoverSurfaceToMove(Recover):
    @staticmethod
    def transitions():
        return Recover.transitions(SurfaceToMove)
class RecoverNextBin(Recover):
    @staticmethod
    def transitions():
        return Recover.transitions(NextBin)
class RecoverDropMarker(Recover):
    @staticmethod
    def transitions():
        return Recover.transitions(DropMarker)
class RecoverCheckDropped(Recover):
    @staticmethod
    def transitions():
        return Recover.transitions(CheckDropped)
class RecoverSurfaceToCruise(Recover):
    @staticmethod
    def transitions():
        return Recover.transitions(SurfaceToCruise)

class Centering(SettlingState):
    """
    When the vehicle is settling over the first found bin, it uses the angle of
    entire bin array this time.
    
    @cvar SETTLED: Event fired when vehicle has settled over the bin
    """
    SETTLED = core.declareEventType('SETTLED_')
    
    @staticmethod
    def transitions():
        return SettlingState.transitions(Centering,
            { Centering.SETTLED : CheckEnd }, lostState = RecoverCentering)
    
    def SETTLED_(self, event):
        """
        Records the orientation of the array when we are first centered over it
        """
        if not self.ai.data.has_key('binArrayOrientation'):
            # Level ourselves out
            self.controller.holdCurrentHeading()
            # Store result heding
            self.ai.data['binArrayOrientation'] = \
                self.controller.getDesiredOrientation()

    def BIN_FOUND(self, event):
        # Cancel out angle commands (we don't want to control orientation)
        event.angle = math.Degree(0)
        SettlingState.BIN_FOUND(self, event)
    
    def enter(self):
        SettlingState.enter(self, Centering.SETTLED, 5, useMultiAngle = True)
        
class CheckEnd(BinSortingState):
    """
    Checks if it's already at the end of the bins before trying to find the
    end
    """
    CONTINUE = core.declareEventType('CONTINUE')
         
    @staticmethod
    def transitions():
        return HoveringState.transitions(CheckEnd,
            {CheckEnd.CONTINUE : SeekEnd}, lostState = RecoverCheckEnd)
        
    def enter(self):
        BinSortingState.enter(self, BinSortingState.LEFT)
        
        if not self.hasBinToLeft():
            self.ai.data['startSide'] = BinSortingState.LEFT
        elif not self.hasBinToRight():
            self.ai.data['startSide'] = BinSortingState.RIGHT
        else:
            # This direction doesn't matter
            self.ai.data['startSide'] = \
                self._config.get('startDirection', BinSortingState.RIGHT)
                
        self.publish(CheckEnd.CONTINUE, core.Event())
        
class SeekEnd(BinSortingState):
    """
    Goes to the left most visible bin
    """
    POSSIBLE_END = core.declareEventType('POSSIBLE_END_')
    AT_END = core.declareEventType('AT_END')
    
    @staticmethod
    def transitions():
        return HoveringState.transitions(SeekEnd,
            {BinSortingState.CENTERED_ : SeekEnd, 
             SeekEnd.POSSIBLE_END : SeekEnd,
             SeekEnd.AT_END : Dive }, lostState = RecoverSeekEnd)
    
    def BIN_FOUND(self, event):
        # Cancel out angle commands (we don't want to control orientation)
        event.angle = math.Degree(0)
        BinSortingState.BIN_FOUND(self, event)
        
    def POSSIBLE_END_(self, event):
        self._timer = None
        if not self.fixEdgeBin():
            # If already there
            self.publish(SeekEnd.AT_END, core.Event())
        
    def enter(self):
        # Keep the hover motion going
        BinSortingState.enter(self, self.ai.data.get('startSide',
            BinSortingState.LEFT), useMultiAngle = True)
        
        # Set orientation to match the initial orientation
        if self.ai.data.has_key('binArrayOrientation'):
            self.controller.setDesiredOrientation(
                self.ai.data['binArrayOrientation'])
        
        self._timeout = self._config.get('timeout', 5)
        self._timer = None

        # Fix the current left most bin, as the currently tracked bin
        if not self.fixEdgeBin():
            # If already there
            self._startTimer()
            
    def _startTimer(self):
        if self._timer is not None:
            self._timer.stop()
        self._timer = \
            self.timerManager.newTimer(SeekEnd.POSSIBLE_END, self._timeout)
        self._timer.start()
        
    def CENTERED(self, event):
        # Fix the current left most bin, as the currently tracked bin
        if (not self.fixEdgeBin()) and (self._timer is None):
            # If already there
            self._startTimer()
        

   
class Dive(HoveringState):
    """
    Gets us down to the depth we can check the symbols out at
    """
    
    @staticmethod
    def transitions():
        return SettlingState.transitions(Dive,
        { motion.basic.Motion.FINISHED : Aligning }, lostState = RecoverDive)

    def BIN_FOUND(self, event):
        # Disable angle tracking
        event.angle = math.Degree(0)
        HoveringState.BIN_FOUND(self, event)
        
    def enter(self, useMultiAngle = False, offsetTheOffset = None):
        # Keep the hover motion going (and use the bin angle)
        HoveringState.enter(self, useMultiAngle)
        
        # Set orientation to match the initial orientation
        if self.ai.data.has_key('binArrayOrientation'):
            self.controller.setDesiredOrientation(
                self.ai.data['binArrayOrientation'])

        # While keeping center, dive down
        binDepth = self._config.get('binDepth', 11)
        offset = self._config.get('offset', 1.5)
        if offsetTheOffset is None:
            offset = offset + self.ai.data.get('dive_offsetTheOffset', 0)
        else:
            offset = offset + offsetTheOffset
        
        diveMotion = motion.basic.RateChangeDepth(    
            desiredDepth = binDepth - offset,
            speed = self._config.get('diveSpeed', 0.3))
        
        self.motionManager.setMotion(diveMotion)
        
class Aligning(SettlingState):
    """
    When the vehicle is settling over the bin
    
    @cvar SETTLED: Event fired when vehicle has settled over the bin
    """
    ALIGNED = core.declareEventType('ALIGNED')
    
    @staticmethod
    def transitions():
        return SettlingState.transitions(Aligning,
            { Aligning.ALIGNED : PreDiveExamine }, lostState = RecoverAligning)
    
    def enter(self):
        SettlingState.enter(self, Aligning.ALIGNED, 5)
        
class Examine(HoveringState):
    """
    Turns on the symbol detector, and determines the type of the bin
    """
    FOUND_TARGET = core.declareEventType('FOUND_TARGET')
    MOVE_ON = core.declareEventType('MOVE_ON')
    TIMEOUT = core.declareEventType('TIMEOUT')

    @staticmethod
    def transitions(myState, foundTarget, moveOn, lostState):
        return HoveringState.transitions(myState,
        { Examine.FOUND_TARGET : foundTarget,
          Examine.MOVE_ON : moveOn,
          Examine.TIMEOUT : myState }, lostState = lostState)

    def BIN_FOUND(self, event):
        HoveringState.BIN_FOUND(self, event)

        # Only look at the current bin
        if self._currentBin(event):
            # Check if it's over the minimum required data
            if self.ai.data['binData']['histogram'][event.id]['totalHits'] >= \
                    self._minimumHits:
                # Have it determine the symbol if it's got enough data
                self._determineSymbols(event.id)

    def _determineSymbols(self, currentID):
        """
        Determine if we have found something and trigger FOUND_TARGET event 
        if we have
        """

        histogram = self.ai.data['binData']['histogram'].setdefault(
            currentID, {'totalHits' : 0})
        totalHits = histogram['totalHits']
        
        # Bail out if we have found nothing
        if 0 == totalHits:
            self.publish(Examine.MOVE_ON, core.Event())
            return
        
        # Convert to percentage of total hits
        # Finds the percentage of the symbols in the histogram
        percentages = {}
        for symbol in histogram.iterkeys():
            if symbol != 'totalHits':
                percentages[symbol] = \
                    float(histogram[symbol]) / totalHits
        
        # See if any of the types is over the needed percentage
        foundTarget = False
        for symbol in percentages.iterkeys():
            if percentages[symbol] >= self._foundLimit:
                foundTarget = self._checkSymbol(symbol)

        # If we didn't find anything, time to move on
        if not foundTarget:
            self.publish(Examine.MOVE_ON, core.Event())
                    
    def _loadSymbolConfig(self):
        targetSymbols = self._config.get('targetSymbols', ['Club', 'Diamond'])
        
        possibleTargetSymbols = set()
        for symbol in targetSymbols:
            symbolName = symbol.upper()
            if hasattr(vision.Symbol, symbolName):
                possibleTargetSymbols.add(getattr(vision.Symbol, symbolName))

        droppedSymbols = self.ai.data.get('droppedSymbols', set())
        self._targetSymbols = possibleTargetSymbols.difference(droppedSymbols)

    def _checkSymbol(self, symbol):
        """
        Returns true if we are looking for this symbol, and publishes
        FOUND_TARGET event.
        """
        if symbol in self._targetSymbols: 
            # Record which symbol we are dropping 
            self.ai.data['droppingSymbol'] = symbol
            # Publish the fact that we found the target to drop
            self.publish(Examine.FOUND_TARGET, core.Event())
            return True
        return False
        
    def enter(self, timeout = 2):
        HoveringState.enter(self)

        self._minimumHits = self._config.get('minimumHits', 100)

        self._timeout = self._config.get('timeout', timeout)

        self._foundLimit = self._config.get('foundLimit', 0.8)
        
        # Load needed symbols
        self._loadSymbolConfig()

        if self._timeout >= 0:
            self._timer = self.timerManager.newTimer(Examine.TIMEOUT,
                                                     self._timeout)
            self._timer.start()

    def exit(self):
        if self._timer is not None:
            self._timer.stop()

class PreDiveExamine(Examine):

    LOOK_CLOSER = core.declareEventType('LOOK_CLOSER')

    @staticmethod
    def transitions():
        trans = Examine.transitions(myState = PreDiveExamine,
                                    foundTarget = CloserLook,
                                    moveOn = SurfaceToMove,
                                    lostState = RecoverPreDiveExamine)
        trans.update({PreDiveExamine.LOOK_CLOSER : CloserLook})

        return trans

    def TIMEOUT(self, event):
        histogram = self.ai.data['binData']['histogram']
        currentID = self.ai.data['binData'].get('currentID', None)

        # Make sure there is a currentID
        if currentID is not None:
            # Check if the histogram has enough hits
            if histogram[currentID]['totalHits'] >= self._minimumHits:
                # If it does, determine the symbol now
                self._determineSymbols(self.ai.data['binData']['currentID'])
            else:
                # If it doesn't, get a closer look
                self.publish(PreDiveExamine.LOOK_CLOSER, core.Event())
        else:
            # If we don't have a currentID, something has gone horribly wrong
            # Move on immediately
            self.publish(Examine.MOVE_ON, core.Event())

    def enter(self):
        Examine.enter(self, timeout = 2)
        
class CloserLook(Dive):
    """
    Drop down before dropping the marker.
    """
    
    @staticmethod
    def transitions():
        return SettlingState.transitions(CloserLook,
        { motion.basic.Motion.FINISHED : PostDiveExamine },
        lostState = RecoverCloserLook)
        
    def enter(self):
        # Standard dive
        offsetTheOffset = self.ai.data.get('closerlook_offsetTheOffset', 0) + \
            self.ai.data.get('dive_offsetTheOffset', 0)
        Dive.enter(self, useMultiAngle = False, offsetTheOffset = offsetTheOffset)

class PostDiveExamine(Examine):
    @staticmethod
    def transitions():
        return Examine.transitions(myState = PostDiveExamine,
                                   foundTarget = SettleBeforeDrop,
                                   moveOn = SurfaceToMove,
                                   lostState = RecoverPostDiveExamine)

    def TIMEOUT(self, event):
        histogram = self.ai.data['binData']['histogram']
        currentID = self.ai.data['binData'].get('currentID', None)

        # Make sure there is a currentID
        if currentID is not None:
            # Check if it has half the number of hits it needs
            # If so, go for it anyways
            if histogram[currentID]['totalHits'] >= self.minimumHitsOnTimeout:
                # If it does, determine the symbol now
                self._determineSymbols(self.ai.data['binData']['currentID'])
            else:
                # If it doesn't, don't take a chance
                self.publish(Examine.MOVE_ON, core.Event())
        else:
            # If we don't have a currentID, something has gone horribly wrong
            # Move on immediately
            self.publish(Examine.MOVE_ON, core.Event())

    def enter(self):
        Examine.enter(self, timeout = 3)
        self._minimumHitsOnTimeout = self._config.get('minimumHitsOnTimeout',
                                                      self._minimumHits/2)
        
class SettleBeforeDrop(SettlingState):
    """
    Gives the vehicle some time to settle over the bins before dropping the
    marker
    """
    
    SETTLED = core.declareEventType('SETTLED')
    
    @staticmethod
    def transitions():
        return SettlingState.transitions(SettleBeforeDrop,
            { SettleBeforeDrop.SETTLED : DropMarker },
            lostState = RecoverSettleBeforeDrop)
    
    def enter(self):
        settleTime = self._config.get('settleTime', 3)
        SettlingState.enter(self, SettleBeforeDrop.SETTLED, settleTime)
       
class SurfaceToMove(HoveringState):
    """
    Goes back to starting cruise depth we had before we started the bins
    """

    @staticmethod
    def transitions():
        return SettlingState.transitions(SurfaceToMove,
            { motion.basic.Motion.FINISHED : NextBin },
            lostState = RecoverSurfaceToMove)
        
    def BIN_FOUND(self, event):
        # Disable angle tracking
        event.angle = math.Degree(0)
        HoveringState.BIN_FOUND(self, event)
        
    def enter(self):
        # Keep centered over the bin
        HoveringState.enter(self)
        
        # Also surface
        binDepth = self._config.get('binDepth', 11)
        offset = self._config.get('offset', 2)
        
        surfaceMotion = motion.basic.RateChangeDepth(
            desiredDepth = binDepth - offset,
            speed = self._config.get('surfaceSpeed', 1.0/3.0))
        
        self.motionManager.setMotion(surfaceMotion) 
        
class NextBin(BinSortingState):
    AT_END = core.declareEventType('AT_END')
    
    @staticmethod
    def transitions():
        return HoveringState.transitions(NextBin,
            {BinSortingState.CENTERED_ : Dive, 
             NextBin.AT_END : CheckDropped },
             lostState = RecoverNextBin)
    
    def _getNextBin(self, sortedBins, currentBinId):
        """
        Override default behaviour to return the next bin to the right
        """
        # Find where the currentBinId is in the list of sorted bins
        try:
            startIdx = sortedBins.index(currentBinId) - 1;
            endIdx = startIdx + 1;
        
            # Pull out the sub list of length one right after that point 
            results = sortedBins[startIdx:endIdx]
            if len(results) == 0:
                # We are at the end
                return currentBinId
            else:
                return results[0]
        except ValueError:
            # We have lost our shit
            self.publish(vision.EventType.BINS_LOST, core.Event())
            return None
        
    
    def BIN_FOUND(self, event):
        # Cancel out angle commands (we don't want to control orientation)
        event.angle = math.Degree(0)
        BinSortingState.BIN_FOUND(self, event)
    
    def enter(self):
        # Keep the hover motion going
        direction = BinSortingState.RIGHT
        if self.ai.data['startSide'] == BinSortingState.RIGHT:
            direction = BinSortingState.LEFT
            
        BinSortingState.enter(self, direction, useMultiAngle = True)
        
        # Fix the current left most bin, as the currently tracked bin
        if not self.fixEdgeBin():
            # If already there
            self.publish(NextBin.AT_END, core.Event())

        
class DropMarker(SettlingState):
    """
    Drops the marker on the bin, then either continues searching, or surfaces
    based on the how many markers its dropped.
    """

    DROPPED = core.declareEventType('DROPPED_')
    FINISHED = core.declareEventType('FINISHED')
    CONTINUE = core.declareEventType('CONTINUE')
    
    @staticmethod
    def transitions():
        return SettlingState.transitions(DropMarker,
            { DropMarker.DROPPED : DropMarker,
              DropMarker.FINISHED : SurfaceToCruise,
              DropMarker.CONTINUE : SurfaceToMove },
              lostState = RecoverDropMarker)

    def DROPPED_(self, event):
        markerNum = self.ai.data['markersDropped']
        if markerNum < 2:
            self.publish(DropMarker.CONTINUE, core.Event())
        else:
            self.publish(DropMarker.FINISHED, core.Event())

    def enter(self):
        SettlingState.enter(self, DropMarker.DROPPED, 5)

        # Increment marker dropped count
        markerNum = self.ai.data.get('markersDropped',0)
        self.ai.data['markersDropped'] = markerNum + 1

        # Release the marker
        self.vehicle.dropMarker()

        # Mark that we dropped the symbol
        droppingSymbol = self.ai.data['droppingSymbol']
        self.ai.data.setdefault('droppedSymbols', set()).add(droppingSymbol)
        
class CheckDropped(HoveringState):
    """
    Check if all of the markers have been dropped.
    """
    
    FINISH = core.declareEventType('FINISH')
    RESTART = core.declareEventType('RESTART')
    
    @staticmethod
    def transitions():
        return HoveringState.transitions(RecoverCheckDropped,
            { CheckDropped.FINISH : SurfaceToCruise,
              CheckDropped.RESTART : Dive }, lostState = RecoverCheckDropped)
        
    def enter(self):
        self._maximumScans = self._config.get('maximumScans', 2)
        data = self.ai.data
        
        if data.get('markersDropped', 0) < 2:
            # We haven't gotten them all
            # Increment the number of times through  and restart
            data['numOfScans'] = data.get('numOfScans', 0) + 1
            if data['startSide'] == BinSortingState.LEFT:
                data['startSide'] = BinSortingState.RIGHT
            else:
                data['startSide'] = BinSortingState.LEFT
            if data['numOfScans'] < self._maximumScans:
                self.publish(CheckDropped.RESTART, core.Event())
            else:
                self.publish(CheckDropped.FINISH, core.Event())
        else:
            # We've dropped them all. Finish.
            self.publish(CheckDropped.FINISH, core.Event())
        
class SurfaceToCruise(HoveringState):
    """
    Goes back to starting cruise depth we had before we started the bins
    """
    @staticmethod
    def transitions():
        return HoveringState.transitions(SurfaceToCruise,
            { motion.basic.Motion.FINISHED : End },
            lostState = RecoverSurfaceToCruise)
        
    def enter(self):
        # Keep centered over the bin
        HoveringState.enter(self)
        
        # Also surface
        surfaceMotion = motion.basic.RateChangeDepth(
            desiredDepth = self.ai.data['preBinCruiseDepth'],
            speed = self._config.get('surfaceSpeed', 1.0/3.0))
        
        self.motionManager.setMotion(surfaceMotion)
        
        # If the offset values exist, delete them
        if self.ai.data.has_key('dive_offsetTheOffset'):
            del self.ai.data['dive_offsetTheOffset']
        if self.ai.data.has_key('closerlook_offsetTheOffset'):
            del self.ai.data['closerlook_offsetTheOffset']


class End(state.State):
    def enter(self):
        self.visionSystem.binDetectorOff()
        self.publish(COMPLETE, core.Event())
