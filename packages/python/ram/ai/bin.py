# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/ai/bin.py

"""
Currently hovers over the bin
"""

# Project Imports
import ext.core as core
import ext.vision as vision
import ext.math as math

import ram.ai.state as state
import ram.motion as motion
import ram.motion.basic
import ram.motion.search
import ram.motion.common

COMPLETE = core.declareEventType('COMPLETE')

# Helper functions which deal with tracking bins based on "found" and "dropped"
# events
def ensureBinTracking(qeventHub, ai):
    if not ai.data.has_key('binTrackingEnabled'):
        ai.data['currentBins'] = set()
        ai.data['binData'] = {}


        def binFound(event):
            """
            Ensures found event is stored, along with its latest data
            """
            id = event.id
            ai.data['currentBins'].add(id)
            ai.data['binData'][id] = event
            
        def binDropped(event):
            """
            Remove dropped bins from records
            """
            id = event.id
            # Remove from the set of current bins
            ai.data['currentBins'].remove(id)
    
            # Remove from our data list
            binData = ai.data['binData']
            del binData[id]
            ai.data['binData'] = binData
            
        connA = qeventHub.subscribeToType(vision.EventType.BIN_FOUND, 
                                          binFound)
        connB = qeventHub.subscribeToType(vision.EventType.BIN_DROPPED, 
                                          binDropped)
    
        ai.addConnection(connA)
        ai.addConnection(connB)
        
        ai.data['binTrackingEnabled'] = True

class HoveringState(state.State):
    LOST_CURRENT_BIN = core.declareEventType('LOST_CURRENT_BIN')
    
    @staticmethod
    def transitions(myState, trans = None):
        if trans is None:
            trans = {}
        trans.update({vision.EventType.BIN_LOST : Searching,
                      vision.EventType.BIN_FOUND : myState})
        return trans
    
    def _currentBin(self, event):
        return self.ai.data.get('currentBinID', 0) == event.id
    
    def BIN_FOUND(self, event):
        """Update the state of the light, this moves the vehicle"""
        # Only listen to the current bin ID
        if self._currentBin(event):
            self._bin.setState(event.x, event.y)

    def enter(self):
        # Make sure we are tracking
        ensureBinTracking(self.queuedEventHub, self.ai)
        
        self._bin = ram.motion.common.Target(0,0)
        sidewaysSpeedGain = self._config.get('sidewaysSpeedGain',3)
        speedGain = self._config.get('speedGain', 5)
        motion = ram.motion.common.Hover(target = self._bin,
                                         maxSpeed = 5,
                                         maxSidewaysSpeed = 3,
                                         sidewaysSpeedGain = sidewaysSpeedGain,
                                         speedGain = speedGain)
        self.motionManager.setMotion(motion)

    def exit(self):
        #print '"Exiting Seek, going to follow"'
        self.motionManager.stopCurrentMotion()

class SettlingState(HoveringState):
    def enter(self, eventType, eventTime):
        self.timer = self.timerManager.newTimer(eventType, eventTime)
        self.timer.start()
        
        HoveringState.enter(self)

    def exit(self):
        HoveringState.exit(self)
        self.timer.stop()

class BinSortingState(HoveringState):
    LEFT = 1
    RIGHT = 2
    
    CENTERED_ = core.declareEventType('CENTERED')
    
    def BIN_FOUND(self, event):
        HoveringState.BIN_FOUND(self, event)

        # Fire event if we are centered over the bin
        if self._currentBin(event): 
            if math.Vector2(event.x, event.y).length() < self._centeredRange:
                self.publish(BinSortingState.CENTERED_, core.Event())
    
    def enter(self, direction):
        """
        @param direction: Says whether or you want to go left or right with the 
        bins
        """
        if (direction != BinSortingState.LEFT) and (direction != BinSortingState.RIGHT):
            raise Exception("ERORR Wrong Direction")
        self._direction = direction
        
        self._centeredRange = self._config.get('centeredRange', 0.2)
        
        HoveringState.enter(self)
    
    def _compareBins(self, idA, idB):
        """
        Sorts the list with the left most bin, at the start
        
        @type idA: int
        @param idA: ID of the bin compare
        
        @type idB: int
        @param idB: ID of the other bin to compare
        """
        binData = self.ai.data['binData']
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
    
    def _fixEdgeBin(self):
        """
        Makes the current bin the left/right most bin, returns true if that
        changes the current bin.
        """
        # Sorted left to right
        currentBins = [b for b in self.ai.data['currentBins']]
        sortedBins = sorted(currentBins, self._compareBins)
        
        # Compare to current ID
        currentBinId = self.ai.data['currentBinID']
        mostEdgeBinId = sortedBins[0]
        
        if currentBinId == mostEdgeBinId:
            # We found the "end" bin
            return False
        else:
            # Still more bins to go
            self.ai.data['currentBinID'] = mostEdgeBinId
            return True

class Searching(state.State):
    """When the vehicle is looking for a bin"""
    @staticmethod
    def transitions():
        return { vision.EventType.BIN_FOUND : Seeking }

    def BIN_FOUND(self, event):
        self.ai.data['currentBinID'] = event.id

    def enter(self):
        # Turn on the vision system
        self.visionSystem.binDetectorOn()

        # Save cruising depth for later surface
        self.ai.data['preBinCruiseDepth'] = self.controller.getDepth()

        # Create zig zag search to 
        zigZag = motion.search.ForwardZigZag(
            legTime = 5,
            sweepAngle = 45,
            speed = 2.5)

        self.motionManager.setMotion(zigZag)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class Seeking(HoveringState):
    """When the vehicle is moving over the found bin"""
    @staticmethod
    def transitions():
        return HoveringState.transitions(Seeking,
            { vision.EventType.BIN_CENTERED : Centering })
        
    def enter(self):
        HoveringState.enter(self)


class Centering(SettlingState):
    """
    When the vehicle is settling over the bin
    
    @cvar SETTLED: Event fired when vehile has settled over the bin
    """
    SETTLED = core.declareEventType('SETTLED')
    
    @staticmethod
    def transitions():
        return SettlingState.transitions(Centering,
            { Centering.SETTLED : SeekEnd })
    
    def enter(self):
        SettlingState.enter(self, Centering.SETTLED, 5)
        
        
class SeekEnd(BinSortingState):
    """
    Goes to the right most visible bin
    """
    AT_END = core.declareEventType('AT_END')
    
    @staticmethod
    def transitions():
        return HoveringState.transitions(SeekEnd,
            {BinSortingState.CENTERED_ : SeekEnd, 
             SeekEnd.AT_END : Dive })
    
    def enter(self):
        # Keep the hover motion going
        BinSortingState.enter(self, BinSortingState.LEFT)
        
        # Fix the current left most bin, as the currently tracked bin
        if not self._fixEdgeBin():
            # If already there
            self.publish(SeekEnd.AT_END, core.Event())
        
    def CENTERED(self, event):
        # Fix the current left most bin, as the currently tracked bin
        if not self._fixEdgeBin():
            # If already there
            self.publish(SeekEnd.AT_END, core.Event())
        

   
class Dive(HoveringState):
    """
    Gets us down to the depth we can check the suits out at
    """
    
    @staticmethod
    def transitions():
        return SettlingState.transitions(Dive,
        { motion.basic.Motion.FINISHED : Examine })

        
    def enter(self):
        # Keep the hover motion going
        HoveringState.enter(self)
        
        # While keeping center, dive down
        diveMotion = motion.basic.RateChangeDepth(
            desiredDepth = self._config.get('depth', 10.5),
            speed = self._config.get('diveSpeed', 0.4))
        
        self.motionManager.setMotion(diveMotion)
        
class Examine(SettlingState):
    """
    Turns on the suit detector, and determines the type of the bin
    """
    FOUND_TARGET = core.declareEventType('FOUND_TARGET')
    MOVE_ON = core.declareEventType('MOVE_ON')
        
    @staticmethod
    def transitions():
        return SettlingState.transitions(Examine,
        { Examine.FOUND_TARGET : DropMarker,
          Examine.MOVE_ON : SurfaceToMove })
        
    def BIN_FOUND(self, event):
        SettlingState.BIN_FOUND(self, event)
        
        # Count the hits
        if self._currentBin(event):
            suit = event.suit
            if suit == vision.Suit.HEART:
                self._hearts += 1
            elif suit == vision.Suit.CLUB:
                self._clubs += 1
            elif suit == vision.Suit.SPADE:
                self._spades += 1
            elif suit == vision.Suit.DIAMOND:
                self._diamonds += 1
                
        # Determine if we have found something and trigger FOUND_TARGET event 
        # if we have
        if self._hearts >= self._foundLimit:
            self._checkSuit(vision.Suit.HEART)
        elif self._clubs >= self._foundLimit:
            self._checkSuit(vision.Suit.CLUB)
        elif self._spades >= self._foundLimit:
            self._checkSuit(vision.Suit.SPADE)
        elif self._diamonds >= self._foundLimit:
            self._checkSuit(vision.Suit.DIAMOND)
                    
    def _loadSuitConfig(self):
        targetSuits = self._config.get('targetSuits', ['Club', 'Diamond'])
        
        self._targetSuits = set()
        for suit in targetSuits:
            suitName = suit.upper()
            if hasattr(vision.Suit, suitName):
                self._targetSuits.add(getattr(vision.Suit, suitName))
    
    def _checkSuit(self, suit):
        """
        Returns true if we are looking for this suit, and publishes
        FOUND_TARGET event.
        """
        if suit in self._targetSuits: 
            self.publish(Examine.FOUND_TARGET, core.Event())
            return True
        return False
        
    def enter(self):
        # Wait for 20 seconds while we examine things
        SettlingState.enter(self, Examine.MOVE_ON, 20)
        
        self._hearts = 0
        self._clubs = 0
        self._spades = 0
        self._diamonds = 0
        self._foundLimit = self._config.get('foundCount', 5)
        
        # Load needed suits
        self._loadSuitConfig()
       
class SurfaceToMove(HoveringState):
    """
    Goes back to starting cruise depth we had before we started the bins
    """
    @staticmethod
    def transitions():
        return SettlingState.transitions(SurfaceToMove,
            { motion.basic.Motion.FINISHED : NextBin })
        
    def enter(self):
        # Keep centered over the bin
        HoveringState.enter(self)
        
        # Also surface
        surfaceMotion = motion.basic.RateChangeDepth(
            desiredDepth = self._config.get('depth', 9),
            speed = self._config.get('surfaceSpeed', 1.0/3.0))
        
        self.motionManager.setMotion(surfaceMotion) 
        
class NextBin(BinSortingState):
    AT_END = core.declareEventType('AT_END')
    
    @staticmethod
    def transitions():
        return HoveringState.transitions(NextBin,
            {BinSortingState.CENTERED_ : Dive, 
             NextBin.AT_END : SurfaceToCruise })
    
    def enter(self):
        # Keep the hover motion going
        BinSortingState.enter(self, BinSortingState.RIGHT)
        
        # Fix the current left most bin, as the currently tracked bin
        if not self._fixEdgeBin():
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
              DropMarker.CONTINUE : SurfaceToMove })

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

        # TODO: drop marker here
        print "\"DROPPER MARKRED #: ", markerNum, "\""
        
        
class SurfaceToCruise(HoveringState):
    """
    Goes back to starting cruise depth we had before we started the bins
    """
    @staticmethod
    def transitions():
        return SettlingState.transitions(SurfaceToCruise,
            { motion.basic.Motion.FINISHED : End })
        
    def enter(self):
        # Keep centered over the bin
        HoveringState.enter(self)
        
        # Also surface
        surfaceMotion = motion.basic.RateChangeDepth(
            desiredDepth = self.ai.data['preBinCruiseDepth'],
            speed = self._config.get('surfaceSpeed', 1.0/3.0))
        
        self.motionManager.setMotion(surfaceMotion)


class End(state.State):
    def enter(self):
        self.visionSystem.binDetectorOff()
        self.publish(COMPLETE, core.Event())
