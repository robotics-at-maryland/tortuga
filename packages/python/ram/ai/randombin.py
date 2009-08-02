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
    
    LOST_CURRENT_BIN = core.declareEventType('LOST_CURRENT_BIN_')
    
    @staticmethod
    def transitions(myState, lostState = None,
                    recoveryState = None, trans = None):
        if lostState is None:
            lostState = Recover
        if recoveryState is None:
            recoveryState = myState
        if trans is None:
            trans = {}
        trans.update({vision.EventType.BINS_LOST : lostState,
                      vision.EventType.BIN_FOUND : myState,
                      vision.EventType.MULTI_BIN_ANGLE : myState,
                      HoveringState.LOST_CURRENT_BIN : recoveryState,
                      vision.EventType.BIN_DROPPED : myState})
        return trans

    @staticmethod
    def getattr():
        return set(['filterLimit', 'sidewaysSpeedGain', 'speedGain',
                    'yawGain', 'maxSpeed', 'maxSidewaysSpeed',
                    'iSpeedGain', 'iSidewaysSpeedGain', 'dSpeedGain',
                    'dSidewaysSpeedGain'])
    
    def _currentBin(self, event):
        return self.ai.data['binData'].get('currentID', 0) == event.id
    
    def MULTI_BIN_ANGLE(self, event):
        self._multiAngle = event.angle
    
    def BIN_FOUND(self, event):
        """Update the state of the light, this moves the vehicle"""
        # Set up the histogram. setdefault will make sure that it sets up
        # the histogram if it isn't set up and will get the histogram if
        # it already exists
        
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
            self._bin.setState(event.x, event.y, event.angle, event.timeStamp)
            
            self.ai.data["lastBinX"] = event.x
            self.ai.data["lastBinY"] = event.y

    def BIN_DROPPED(self, event):
        if self._currentBin(event):
            # Say we've lost the current bin
            if len(self.ai.data['binData']['currentIds'].
                   difference([event.id])):
                self.publish(HoveringState.LOST_CURRENT_BIN, core.Event())
            
    def LOST_CURRENT_BIN_(self, event):
        """
        Default actions for losing the current bin. This does not occur
        if a recoveryState was set.
        """
        # Check if it loops back to itself on this state
        recoveryState = \
            self.__class__.transitions()[HoveringState.LOST_CURRENT_BIN]
        if recoveryState == self.__class__:
            # Check if we have other ids and switch to one of those
            currentIds = self.ai.data['binData']['currentIds']
            if currentIds is not None:
                newEvent = self._findClosestBinIdEvent()
                if newEvent is not None:
                    self.ai.data['binData']['currentID'] = newEvent.id
                else:
                    raise Exception("Lost the currentID"\
                                        "without a proper recovery!")
                
    def _findClosestBinIdEvent(self):
        closestIdEvent = None
        currentBins = [b for b in self.ai.data['binData']['currentIds']]
        if len(currentBins) > 0:
            binIDs = sorted(currentBins, self._compareByDistance)
            
            closestIdEvent = self.ai.data['binData']['itemData'][binIDs[0]]
        
        return closestIdEvent
                
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
    
    def enter(self, useMultiAngle = False, shouldRotate = True):
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
        
        self._bin = ram.motion.pipe.Pipe(0,0,0,timeStamp = None)
        sidewaysSpeedGain = self._config.get('sidewaysSpeedGain',3)
        speedGain = self._config.get('speedGain', 5)
        if shouldRotate:
            yawGain = self._config.get('yawGain', 1)
        else:
            yawGain = 0
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
    @staticmethod
    def getattr():
        return set(['planeThreshold', 'angleThreshold']).union(
            HoveringState.getattr())

    def _compareChange(self, pvalues, dvalues):
        # Get the error adjustments
        errorAdj = self._bin.errorAdj()

        # Return true if all of the values are in the thresholds
        # x = [0], y = [1], angle = [2]
        return errorAdj[0] < self._planeThreshold and \
            errorAdj[1] < self._planeThreshold and \
            errorAdj[2] < self._angleThreshold

    def BIN_FOUND(self, event):
        HoveringState.BIN_FOUND(self, event)

        # Only do if we allow early timeouts and it is the current bin
        if self._earlyTimeout and self._currentBin(event):
            # Check if it is in the threshold
            if self._compareChange((event.x, event.y, event.angle),
                                   self._bin.changeOverTime()):
                # It is settled. Stop the timer and move on.
                if self.timer is not None:
                    self.timer.stop()
                self.publish(self._eventType, core.Event())

    def enter(self, eventType, eventTime, useMultiAngle = False,
              shouldRotate = True):
        self._eventType = eventType
        self._kp = self._config.get('kp', 1.0)
        self._kd = self._config.get('kd', 1.0)
        self._planeThreshold = self._config.get('planeThreshold', 0.1)
        self._angleThreshold = self._config.get('angleThreshold', 5)
        self._earlyTimeout = self.ai.data['config'].get('Bin', {}).get(
            'earlyTimeout', False)

        self.timer = self.timerManager.newTimer(eventType, eventTime)
        self.timer.start()
        
        HoveringState.enter(self, useMultiAngle, shouldRotate)

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
    
    @staticmethod
    def getattr():
        return set(['centeredRange']).union(HoveringState.getattr())

    def BIN_FOUND(self, event):
        HoveringState.BIN_FOUND(self, event)

        # Fire event if we are centered over the bin
        if self._currentBin(event): 
            if math.Vector2(event.x, event.y).length() < self._centeredRange:
                self.publish(BinSortingState.CENTERED_, core.Event())
    
    def enter(self, direction, useMultiAngle = False, shouldRotate = True):
        """
        @param direction: Says whether or you want to go left or right with the 
                          bins
        """
        if (direction != BinSortingState.LEFT) and (direction != BinSortingState.RIGHT):
            raise Exception("ERROR Wrong Direction")
        self._direction = direction
        
        self._centeredRange = self._config.get('centeredRange', 0.2)
        
        HoveringState.enter(self, useMultiAngle = useMultiAngle,
                            shouldRotate = shouldRotate)
    
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
        mostEdgeBinId = sortedBins[0]
        return mostEdgeBinId
        
class Recover(state.FindAttempt):
    MOVE_ON = core.declareEventType('MOVE_ON')
    RETURN = core.declareEventType('RETURN_')
    
    @staticmethod
    def transitions(myState, foundState):
        trans = state.FindAttempt.transitions(Recover.RETURN,
                                             foundState, Start)
        trans.update({vision.EventType.BIN_FOUND : myState,
                      vision.EventType.BINS_LOST : myState})
        
        return trans

    @staticmethod
    def getattr():
        return set(['timeout', 'speed']).union(state.FindAttempt.getattr())
        
    def BIN_FOUND(self, event):            
        # Create a delay timer to get all the bins before going back
        if self._delay is None:
            # Turn off the old timer
            if self.timer is not None:
                self.timer.stop()
            
            self._delay = self.timerManager.newTimer(Recover.RETURN, 0.5)
            self._delay.start()
            
    def BINS_LOST(self, event):
        # Restart the timeout timer and stop the delay timer
        if self._delay is not None:
            self._delay.stop()
            self._delay = None
            
        self.timer = \
            self.timerManager.newTimer(state.FindAttempt.TIMEOUT, self._timeout)
        self.timer.start()
                
    def RETURN_(self, event):
        location = math.Vector2(self.ai.data['lastBinX'], self.ai.data['lastBinY'])
        foundId = None
        foundIdDistance = None
        binData = self.ai.data['binData']
        currentBins = [b for b in binData['currentIds']]
        for id in currentBins:
            binLocation = math.Vector2(binData['itemData'][id].x,
                                       binData['itemData'][id].y)
            if foundId is None:
                foundId = id
                foundIdDistance = (binLocation - location).length()
            else:
                if (binLocation - location).length() < foundIdDistance:
                    foundId = id
                    foundIdDistance = (binLocation - location).length()
        self.ai.data['binData']['currentID'] = foundId
    
    def enter(self, timeout = 4):
        self._timeout = self._config.get('timeout', timeout)
        state.FindAttempt.enter(self, self._timeout)

        ensureBinTracking(self.queuedEventHub, self.ai)
        
        # Will be in the next commit
        x = self.ai.data['lastBinX']
        y = self.ai.data['lastBinY']
        
        ahead = math.Vector3(x, y, 0)
        quat = math.Vector3.UNIT_Y.getRotationTo(ahead)
        self._direction = quat.getYaw(True)
        self._speed = self._config.get('speed', 0.5)
        self._delay = None

        searchMotion = motion.basic.MoveDirection(self._direction, self._speed)

        self.motionManager.setMotion(searchMotion)
        
class LostCurrentBin(state.FindAttempt, HoveringState):
    """
    When the vehicle loses its current ID it will attempt to find it.
    If it fails to find it, it will choose a new ID and continue.
    """
    REFOUND_BIN = core.declareEventType('REFOUND_BIN')
    
    @staticmethod
    def transitions(myState, lostState, originalState):
        trans = HoveringState.transitions(myState, lostState = lostState)
        trans.update(state.FindAttempt.transitions(vision.EventType.BIN_FOUND,
                      myState, originalState, trans = trans))
        trans.update({ LostCurrentBin.REFOUND_BIN : originalState })
        
        return trans

    @staticmethod
    def getattr():
        return set(['timeout', 'recoverThreshold']).union(
            HoveringState.getattr()).union(state.FindAttempt.getattr())
    
    def BIN_FOUND(self, event):
        HoveringState.BIN_FOUND(self, event)
        # Check to see if this bin is new
        if event.id not in self._currentIds:
            # If it is, check if it's in the threshold
            oldX = self.ai.data['lastBinX']
            oldY = self.ai.data['lastBinY']
            
            if ((oldX - self._recoverThreshold) < event.x < \
                    (oldX + self._recoverThreshold)) and \
                    ((oldY - self._recoverThreshold) < \
                    event.y < (oldY + self._recoverThreshold)):
                # We've found the bin
                self.ai.data['binData']['currentID'] = event.id
                self.publish(LostCurrentBin.REFOUND_BIN, core.Event())
            else:
                # Add it to the currentIDs and continue searching
                self._currentIds.add(event.id)

    def TIMEOUT(self, event):
        currentIds = self.ai.data['binData']['currentIds']
        if currentIds is not None:
            newEvent = self._findClosestBinIdEvent()
            if newEvent is not None:
                self.ai.data['binData']['currentID'] = newEvent.id
            else:
                # Something went horribly wrong
                raise Exception("Lost the Current ID without anyway back!")
        
    def enter(self):
        HoveringState.enter(self)
        state.FindAttempt.enter(self, timeout = self._config.get('timeout', 3))
        self._recoverThreshold = self._config.get('recoverThreshold', 0.2)
        
        self._currentIds = self.ai.data['binData']['currentIds']
    
class Start(state.State):
    """
    Gets us to proper depth to start the actual bin search.
    """
    @staticmethod
    def transitions():
        return { motion.basic.Motion.FINISHED : Searching }

    @staticmethod
    def getattr():
        return set(['speed'])
    
    def enter(self):
        # Store the initial direction
        orientation = self.vehicle.getOrientation()
        self.ai.data['binStartOrientation'] = \
            orientation.getYaw().valueDegrees()

        # Go to 5 feet in 5 increments
        diveMotion = motion.basic.RateChangeDepth(
            desiredDepth = self.ai.data['config'].get('binStartDepth', 7),
            speed = self._config.get('speed', 1.0/3.0))
        self.motionManager.setMotion(diveMotion)
        
        self.ai.data['firstSearching'] = True

    def exit(self):
        self.motionManager.stopCurrentMotion()

class Searching(state.State):
    """When the vehicle is looking for a bin"""
    @staticmethod
    def transitions():
        return { vision.EventType.BIN_FOUND : Seeking }

    @staticmethod
    def getattr():
        return set(['legTime', 'sweepAngle', 'speed'])

    def BIN_FOUND(self, event):
        self.ai.data['binData']['currentID'] = event.id
        self.ai.data['binData']['currentIds'] = set()

    def enter(self):
        ensureBinTracking(self.queuedEventHub, self.ai)
        
        orientation = self.vehicle.getOrientation()
        direction = self.ai.data.setdefault(
            'binStartOrientation', orientation.getYaw().valueDegrees())

        # Turn on the vision system
        self.visionSystem.binDetectorOn()

        # Save cruising depth for later surface
        self.ai.data['preBinCruiseDepth'] = self.controller.getDepth()

        # Create the forward motion
        self._duration = self.ai.data['config'].get('Bin', {}).get(
            'forwardDuration', 2)
        self._forwardSpeed = self.ai.data['config'].get('Bin', {}).get(
            'forwardSpeed', 3)
        self._forwardMotion = motion.basic.TimedMoveDirection(
            desiredHeading = 0,
            speed = self._forwardSpeed,
            duration = self._duration,
            absolute = False)

        # Create zig zag search to 
        self._zigZag = motion.search.ForwardZigZag(
            legTime = self._config.get('legTime', 5),
            sweepAngle = self._config.get('sweepAngle', 45),
            speed = self._config.get('speed', 2.5),
            direction = direction)

        if self.ai.data.get('firstSearching', True) and self._duration > 0:
            self.motionManager.setQueuedMotions(self._forwardMotion,
                                                self._zigZag)
        else:
            self.motionManager.setMotion(self._zigZag)

        self.ai.data['firstSearching'] = False

    def exit(self):
        self.motionManager.stopCurrentMotion()

class Seeking(HoveringState):
    """When the vehicle is moving over the found bin"""
    BIN_CENTERED = core.declareEventType("BIN_CENTERED")
    
    @staticmethod
    def transitions():
        return HoveringState.transitions(Seeking,
            lostState = RecoverSeeking,
            trans = { Seeking.BIN_CENTERED : Centering })

    @staticmethod
    def getattr():
        return set(['centeredLimit']).union(HoveringState.getattr())
 
    def BIN_FOUND(self, event):
        eventDistance = math.Vector2(event.x, event.y).length()
        
        closestIdEvent = self._findClosestBinIdEvent()
        if closestIdEvent is None:
            self.ai.data['binData']['currentID'] = event.id
        else:
            closestIdDistance = math.Vector2(closestIdEvent.x,
                                             closestIdEvent.y).length()
            
            if closestIdDistance < eventDistance:
                self.ai.data['binData']['currentID'] = closestIdEvent.id
            else:
                self.ai.data['binData']['currentID'] = event.id
        
        HoveringState.BIN_FOUND(self, event)
        
        if self._currentBin(event):
            if eventDistance < self._centeredLimit:
                self.publish(Seeking.BIN_CENTERED, core.Event())
        
    def enter(self):
        HoveringState.enter(self, shouldRotate = False)
        self._centeredLimit = self._config.get('centeredLimit', 0.2)

class RecoverSeeking(Recover):
    @staticmethod
    def transitions():
        return Recover.transitions(RecoverSeeking, Seeking)

    @staticmethod
    def getattr():
        return set(['speedGain', 'sidewaysSpeedGain', 'yawGain',
                    'maxSpeed', 'maxSidewaysSpeed']).union(
            HoveringState.getattr())

    def enter(self, timeout = 4):
        Recover.enter(self, timeout)
        
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
        Recover.exit(self)

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
            lostState = RecoverCentering,
            trans = { Centering.SETTLED : Dive })
    
    def SETTLED_(self, event):
        """
        Records the orientation of the array when we are first centered over it
        """
        if not self.ai.data.has_key('binArrayOrientation'):
            # Level ourselves out
            self.controller.holdCurrentHeading()
            # Store result heading
            self.ai.data['binArrayOrientation'] = \
                self.controller.getDesiredOrientation()

    def BIN_FOUND(self, event):
        # Cancel out angle commands (we don't want to control orientation)
        event.angle = math.Degree(0)
        SettlingState.BIN_FOUND(self, event)
    
    def enter(self):
        self._binDirection = self.ai.data['config'].get('Bin', {}).get(
            'binDirection', None)
        if self._binDirection is None:
            SettlingState.enter(self, Centering.SETTLED, 5,
                                useMultiAngle = True)
        else:
            SettlingState.enter(self, Centering.SETTLED, 5,
                                useMultiAngle = True, shouldRotate = False)
            
            self.controller.setDesiredOrientation(
                math.Quaternion(math.Degree(self._binDirection),
                                            math.Vector3.UNIT_Z))
        
class RecoverCentering(Recover):
    @staticmethod
    def transitions():
        return Recover.transitions(RecoverCentering, Centering)
   
class Dive(HoveringState):
    """
    Gets us down to the depth we can check the symbols out at
    """
    
    @staticmethod
    def transitions():
        return SettlingState.transitions(Dive,
        lostState = RecoverDive, recoveryState = LostCurrentBinDive,
        trans = { motion.basic.Motion.FINISHED : Aligning })

    @staticmethod
    def getattr():
        return set(['offset', 'diveSpeed']).union(HoveringState.getattr())

    def BIN_FOUND(self, event):
        # Disable angle tracking
        event.angle = math.Degree(0)
        HoveringState.BIN_FOUND(self, event)
        
    def enter(self, useMultiAngle = False, offset = 1.5):
        self._binDirection = self.ai.data['config'].get('Bin', {}).get(
            'binDirection', None)

        # Keep the hover motion going (and use the bin angle)
        if self._binDirection is None:
            HoveringState.enter(self, useMultiAngle)
        else:
            HoveringState.enter(self, useMultiAngle, shouldRotate = False)
        
        # Set orientation to match the initial orientation
        if self.ai.data.has_key('binArrayOrientation'):
            self.controller.setDesiredOrientation(
                self.ai.data['binArrayOrientation'])

        # While keeping center, dive down
        binDepth = self.ai.data['config'].get('binDepth', 11)
        offset_ = self._config.get('offset', offset)
        offset_ = offset_ + self.ai.data.get('dive_offsetTheOffset', 0) + \
            self.ai.data.get('closerlook_offsetTheOffset', 0)
        
        diveMotion = motion.basic.RateChangeDepth(    
            desiredDepth = binDepth - offset_,
            speed = self._config.get('diveSpeed', 0.3))
        
        self.motionManager.setMotion(diveMotion)
        
class RecoverDive(Recover):
    @staticmethod
    def transitions():
        trans = Recover.transitions(RecoverDive, Dive)
        trans.update({ Recover.MOVE_ON : SurfaceToMove })
        
        return trans

    @staticmethod
    def getattr():
        return set(['timeout', 'increase', 'maxIncrease', 'diveSpeed']).union(
            HoveringState.getattr())

    def enter(self):
        Recover.enter(self, timeout = self._config.get('timeout', 4))
        self._increase = self._config.get('increase', 0.25)
        self._maxIncrease = self._config.get('maxIncrease', 1)
        self.ai.data['dive_offsetTheOffset'] = \
            self.ai.data.get('dive_offsetTheOffset', 0) + self._increase
            
        if self.ai.data['dive_offsetTheOffset'] > self._maxIncrease:
            self.publish(Recover.MOVE_ON, core.Event())
        else:
            depth = self.vehicle.getDepth()
            offset = self.ai.data['dive_offsetTheOffset']
            diveMotion = motion.basic.RateChangeDepth(
                                desiredDepth = depth - offset,
                                speed = self._config.get('diveSpeed', 0.3))
            self.motionManager.setMotion(diveMotion)
            
class LostCurrentBinDive(LostCurrentBin):
    @staticmethod
    def transitions():
        return LostCurrentBin.transitions(myState = LostCurrentBinDive,
                                          lostState = RecoverDive,
                                          originalState = Dive)
        
class Aligning(SettlingState):
    """
    When the vehicle is settling over the bin
    
    @cvar SETTLED: Event fired when vehicle has settled over the bin
    """
    ALIGNED = core.declareEventType('ALIGNED')
    
    @staticmethod
    def transitions():
        return SettlingState.transitions(Aligning,
            lostState = RecoverDive, recoveryState = LostCurrentBinAligning,
            trans = { Aligning.ALIGNED : CloserLook })
    
    def enter(self):
        self._adjustAngle = self.ai.data['config'].get('Bin', {}).get(
            'adjustAngle', True)
        SettlingState.enter(self, Aligning.ALIGNED, 5,
                            shouldRotate = self._adjustAngle)
    
class LostCurrentBinAligning(LostCurrentBin):
    @staticmethod
    def transitions():
        return LostCurrentBin.transitions(myState = LostCurrentBinAligning,
                                          lostState = RecoverDive,
                                          originalState = Aligning)
        
class CloserLook(Dive):
    """
    Drop down before dropping the marker.
    """
    
    @staticmethod
    def transitions():
        return SettlingState.transitions(CloserLook,
        lostState = RecoverCloserLook, recoveryState = LostCurrentBinCloserLook,
        trans = { motion.basic.Motion.FINISHED : DropMarker })

    @staticmethod
    def getattr():
        return set(['offset']).union(Dive.getattr())
        
    def enter(self):
        # Standard dive
        Dive.enter(self, useMultiAngle = False,
                   offset = self._config.get('offset', 1.0))
        
class RecoverCloserLook(Recover):
    @staticmethod
    def transitions():
        trans = Recover.transitions(RecoverCloserLook, CloserLook)
        trans.update({ Recover.MOVE_ON : SurfaceToMove })
        
        return trans

    @staticmethod
    def getattr():
        return set(['timeout', 'increase', 'maxIncrease', 'diveSpeed']).union(
            Recover.getattr())

    def enter(self):
        Recover.enter(self, timeout = self._config.get('timeout', 4))
        self._increase = self._config.get('increase', 0.25)
        self._maxIncrease = self._config.get('maxIncrease', 1)
        self.ai.data['closerlook_offsetTheOffset'] = \
            self.ai.data.get('closerlook_offsetTheOffset', 0) + self._increase
            
        if self.ai.data['closerlook_offsetTheOffset'] > self._maxIncrease:
            self.publish(Recover.MOVE_ON, core.Event())
        else:
            depth = self.vehicle.getDepth()
            offset = self.ai.data['closerlook_offsetTheOffset']
            diveMotion = motion.basic.RateChangeDepth(
                                desiredDepth = depth - offset,
                                speed = self._config.get('diveSpeed', 0.3))
            self.motionManager.setMotion(diveMotion)
            
class LostCurrentBinCloserLook(LostCurrentBin):
    @staticmethod
    def transitions():
        return LostCurrentBin.transitions(myState = LostCurrentBinCloserLook,
                                          lostState = RecoverCloserLook,
                                          originalState = CloserLook)
               
class SurfaceToMove(HoveringState):
    """
    Goes back to starting cruise depth we had before we started the bins
    """

    @staticmethod
    def transitions():
        return SettlingState.transitions(SurfaceToMove,
            lostState = RecoverSurfaceToMove,
            recoveryState = LostCurrentBinSurfaceToMove,
            trans = { motion.basic.Motion.FINISHED : NextBin })

    @staticmethod
    def getattr():
        return set(['offset', 'surfaceSpeed']).union(HoveringState.getattr())
        
    def BIN_FOUND(self, event):
        # Disable angle tracking
        event.angle = math.Degree(0)
        HoveringState.BIN_FOUND(self, event)
        
    def enter(self):
        # Keep centered over the bin
        self._binDirection = self.ai.data['config'].get('Bin', {}).get(
            'binDirection', None)

        if self._binDirection is None:
            HoveringState.enter(self)
        else:
            HoveringState.enter(self, shouldRotate = False)
        
        # Also surface
        binDepth = self.ai.data['config'].get('binDepth', 11)
        offset = self._config.get('offset', 2)
        
        offset = offset + self.ai.data.get('dive_offsetTheOffset', 0) + \
            self.ai.data.get('closerlook_offsetTheOffset', 0)
        
        surfaceMotion = motion.basic.RateChangeDepth(
            desiredDepth = binDepth - offset,
            speed = self._config.get('surfaceSpeed', 1.0/3.0))
        
        self.motionManager.setMotion(surfaceMotion)
        
class RecoverSurfaceToMove(Recover):
    @staticmethod
    def transitions():
        return Recover.transitions(RecoverSurfaceToMove, SurfaceToMove)
    
class LostCurrentBinSurfaceToMove(LostCurrentBin):
    @staticmethod
    def transitions():
        return LostCurrentBin.transitions(myState = LostCurrentBinSurfaceToMove,
                                          lostState = RecoverSurfaceToMove,
                                          originalState = SurfaceToMove)
        
class NextBin(BinSortingState):
    AT_END = core.declareEventType('AT_END')
    
    @staticmethod
    def transitions():
        return HoveringState.transitions(NextBin,
            lostState = RecoverNextBin, recoveryState = LostCurrentBinNextBin,
            trans = { BinSortingState.CENTERED_ : Dive, 
                      NextBin.AT_END : SurfaceToCruise })
    
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
            #self.publish(vision.EventType.BINS_LOST, core.Event())
            # We lost the current ID, recover by finding the next best
            # Use the old getNextBin
            result = BinSortingState._getNextBin(self, sortedBins, currentBinId)
            return result
    
    def BIN_FOUND(self, event):
        # Cancel out angle commands (we don't want to control orientation)
        event.angle = math.Degree(0)
        BinSortingState.BIN_FOUND(self, event)
    
    def enter(self):
        # Keep the hover motion going
        direction = BinSortingState.RIGHT
        if self.ai.data['startSide'] == BinSortingState.RIGHT:
            direction = BinSortingState.LEFT

        self._binDirection = self.ai.data['config'].get('Bin', {}).get(
            'binDirection', None)
        if self._binDirection is None:
            BinSortingState.enter(self, direction, useMultiAngle = True)
        else:
            BinSortingState.enter(self, direction, useMultiAngle = True,
                                  shouldRotate = False)

        # Fix the current left most bin, as the currently tracked bin
        if not self.fixEdgeBin():
            # If already there
            self.publish(NextBin.AT_END, core.Event())

class RecoverNextBin(Recover):
    @staticmethod
    def transitions():
        return Recover.transitions(RecoverNextBin, Dive)
    
class LostCurrentBinNextBin(LostCurrentBin):
    @staticmethod
    def transitions():
        return LostCurrentBin.transitions(myState = LostCurrentBinNextBin,
                                          lostState = RecoverNextBin,
                                          originalState = Dive)
        
class DropMarker(SettlingState):
    """
    Drops the marker on the bin, then either continues searching, or surfaces
    based on the how many markers its dropped.
    """

    FINISHED = core.declareEventType('FINISHED')
    CONTINUE = core.declareEventType('CONTINUE')
    DROP = core.declareEventType('DROP_')
    
    @staticmethod
    def transitions():
        return SettlingState.transitions(DropMarker,
            lostState = RecoverCloserLook,
            recoveryState = LostCurrentBinCloserLook,
            trans = { DropMarker.FINISHED : SurfaceToCruise,
                      DropMarker.DROP : DropMarker })

    def _dropMarker(self):
        # Release the marker
        self.vehicle.dropMarker()

        self.publish(DropMarker.FINISHED, core.Event())

    def DROP_(self, event):
        # Drop the marker when the timeout happens regardless
        self._dropMarker()

    def BIN_FOUND(self, event):
        # Zero out the angle
        event.angle = math.Degree(0)
        SettlingState.BIN_FOUND(self, event)

    def enter(self):
        self._binDirection = self.ai.data['config'].get('Bin', {}).get(
            'binDirection', None)
        if self._binDirection is None:
            SettlingState.enter(self, DropMarker.DROP, 2)
        else:
            SettlingState.enter(self, DropMarker.DROP, 2, shouldRotate = False)
        
class SurfaceToCruise(state.State):
    """
    Goes back to starting cruise depth we had before we started the bins
    """
    @staticmethod
    def transitions():
        return { motion.basic.Motion.FINISHED : End }

    @staticmethod
    def getattr():
        return set(['surfaceSpeed'])
        
    def enter(self):
        # Surface
        surfaceMotion = motion.basic.RateChangeDepth(
            desiredDepth = self.ai.data['preBinCruiseDepth'],
            speed = self._config.get('surfaceSpeed', 1.0/3.0))
        
        self.motionManager.setMotion(surfaceMotion)

class End(state.State):
    def enter(self):
        # If the offset values exist, delete them
        if self.ai.data.has_key('dive_offsetTheOffset'):
            del self.ai.data['dive_offsetTheOffset']
        if self.ai.data.has_key('closerlook_offsetTheOffset'):
            del self.ai.data['closerlook_offsetTheOffset']

        self.visionSystem.binDetectorOff()
        self.publish(COMPLETE, core.Event())
