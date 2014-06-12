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
from ext.control import yawVehicleHelper

import ram.ai.state as state
import ram.ai.tracking as tracking
import ram.motion as motion
import ram.motion.basic
import ram.motion.search
#import ram.motion.common
import ram.motion.pipe # For the maneuvering motions
from ram.motion.basic import Frame

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
        return {'filterLimit' : 75, 'sidewaysSpeedGain' : 3, 'speedGain' : 5,
                    'yawGain' : 1, 'maxSpeed' : 5, 'maxSidewaysSpeed' : 3,
                    'iSpeedGain' : 0, 'iSidewaysSpeedGain' : 0,
                    'dSpeedGain' : 0, 'dSidewaysSpeedGain' : 0,
                    'sidewaysRate' : 1, 'forwardRate' : 1,
                    'motionRange' : 0.1, 'forwardSpeed' : 0.15}
    
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
            self._bin.setState(event.x, event.y, event.angle, event.timeStamp)
            self.changedBin()
            
            self.ai.data["lastBinX"] = event.x
            self.ai.data["lastBinY"] = event.y

    def BIN_DROPPED(self, event):
        self._cleanupHistogram(event.id)
        
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
                
    def _cleanupHistogram(self, id):
        # Cleanup the histogram data
        histogram = self.ai.data['binData']['histogram']

        if histogram.has_key(id):
            del histogram[id]

    def enter(self, useMultiAngle = False, shouldRotate = True):
        """
        Use multiAngle determines whether or not you listen to the bin angle
        or the angle of the array of the bins
        """
            
        # Make sure we are tracking
        ensureBinTracking(self.queuedEventHub, self.ai)
        
        self._shouldRotate = shouldRotate
        self._useMultiAngle = useMultiAngle
        self._multiAngle = math.Degree(0)
        self._first = True
        
        self._bin = ram.motion.pipe.Pipe(0,0,0,timeStamp = None)
        
        self._currentDesiredPos = math.Vector2(self._bin.getY() * 
                                               self._forwardRate,
                                               self._bin.getX() * 
                                               self._sidewaysRate)
        self._currentDesiredOrientation = None
        
        currentOrientation = self.stateEstimator.getEstimatedOrientation()
        yawTrajectory = motion.trajectories.StepTrajectory(
            initialValue = currentOrientation,
            finalValue = yawVehicleHelper(currentOrientation, 
                                          self._bin.getAngle()),
            initialRate = self.stateEstimator.getEstimatedAngularRate(),
            finalRate = math.Vector3.ZERO)
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = math.Vector2.ZERO,
            finalValue = self._currentDesiredPos,
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._forwardSpeed)

        yawMotion = motion.basic.ChangeOrientation(yawTrajectory)
        translateMotion = ram.motion.basic.Translate(translateTrajectory,
                                                     frame = Frame.LOCAL)
        
        # If this is ever false then the motions are never recalculated
        # for this state
        self._recalculate = True

        if self._shouldRotate:
            self.motionManager.setMotion(yawMotion, translateMotion)
        else:
            self.motionManager.setMotion(translateMotion)

    def exit(self):
        #print '"Exiting Seek, going to follow"'
        self.motionManager.stopCurrentMotion()
        
    def changedBin(self):
        # A stupid solution. Stop current motion and restart it when
        # PipeInfo is changed

        if not self._changeMotion(self._currentDesiredPos, math.Vector2( 
                self._bin.getY(), self._bin.getX())) or not self._recalculate:
            return
            
        self._currentDesiredPos = math.Vector2(self._bin.getY() * 
                                               self._forwardRate, 
                                               self._bin.getX() * 
                                               self._sidewaysRate)
        self.motionManager.stopCurrentMotion()
            
        currentOrientation = self.stateEstimator.getEstimatedOrientation()

        orientation = self._currentDesiredOrientation
        if orientation is None:
            orientation = yawVehicleHelper(currentOrientation, self._bin.getAngle())

        yawTrajectory = motion.trajectories.StepTrajectory(
            initialValue = currentOrientation,
            finalValue = orientation,
            initialRate = self.stateEstimator.getEstimatedAngularRate(),
            finalRate = math.Vector3.ZERO)
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = math.Vector2.ZERO,
            finalValue = self._currentDesiredPos,
            initialRate = math.Vector2.ZERO,
            avgRate = self._forwardSpeed)
        
        yawMotion = motion.basic.ChangeOrientation(yawTrajectory)
        translateMotion = ram.motion.basic.Translate(translateTrajectory,
                                                     frame = Frame.LOCAL)
        if self._shouldRotate or self._currentDesiredOrientation is not None:
            self.motionManager.setMotion(yawMotion, translateMotion)
        else:
            self.motionManager.setMotion(translateMotion)


    def _changeMotion(self, vector1, vector2):
        # Another messy solution, compares two vectors to see if there
        # is much of a difference between them. Returns True if there is
        # is significant difference
        if pmath.sqrt((vector2.x - vector1.x) * (vector2.x - vector1.x) + 
                     (vector2.y - vector1.y) * 
                     (vector2.y - vector1.y)) < self._motionRange:
            return False
        return True

class SettlingState(HoveringState):
    """
    A specialization of the hover state which hovers for the given time.
    """
    @staticmethod
    def getattr():
        attrs = {}
        attrs.update(HoveringState.getattr())
        attrs.update({'planeThreshold' : 0.1, 'angleThreshold' : 5})
        return attrs

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
        attrs = {}
        attrs.update(HoveringState.getattr())
        attrs.update({'centeredRange' : 0.2})
        return attrs

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
        attrs = {}
        attrs.update(state.FindAttempt.getattr())
        attrs.update({'timeout' : 4, 'distance' : 5, 'forwardSpeed' : 0.15})
        return attrs
        #return {'timeout' : 4, 'speed' : 0.5}.union(state.FindAttempt.getattr())
        
    def BIN_FOUND(self, event):
        if event.symbol != vision.Symbol.UNKNOWN:
            histogram = \
                self.ai.data['binData']['histogram'].setdefault(
                    event.id, dict())
            histogram[event.symbol] = histogram.get(event.symbol, 0) + 1
            histogram['totalHits'] = histogram.get('totalHits', 0) + 1
            
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
        self._delay = None

        currentOrientation = self.stateEstimator.getEstimatedOrientation()
        yawTrajectory = motion.trajectories.StepTrajectory(
            initialValue = currentOrientation,
            finalValue = yawVehicleHelper(currentOrientation, 
                                          self._direction.valueDegrees()),
            initialRate = self.stateEstimator.getEstimatedAngularRate(),
            finalRate = math.Vector3.ZERO)
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = math.Vector2.ZERO,
            finalValue = math.Vector2(self._distance,0),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._forwardSpeed)

        yawMotion = motion.basic.ChangeOrientation(yawTrajectory)
        translateMotion = ram.motion.basic.Translate(translateTrajectory,
                                                     frame = Frame.LOCAL)

        self.motionManager.setMotion(yawMotion, translateMotion)
        
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
        attrs = {}
        attrs.update(HoveringState.getattr())
        attrs.update(state.FindAttempt.getattr())
        attrs.update({'timeout' : 3, 'recoverThreshold' : 0.2})
        return attrs
    
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
        state.FindAttempt.enter(self, timeout = self._timeout)
        
        self._currentIds = self.ai.data['binData']['currentIds']
    
class Start(state.State):
    """
    Gets us to proper depth to start the actual bin search.
    """
    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Searching }

    @staticmethod
    def getattr():
        return {'speed' : 1.0/3.0, 'offset' : 5}
    
    def enter(self):
        # Store the initial direction
        self.ai.data['numOfScans'] = 0
        
        # Set the failsafe
        self.ai.data.setdefault('doICare', True)

        orientation = self.stateEstimator.getEstimatedOrientation()
        self.ai.data['binStartOrientation'] = \
            orientation.getYaw().valueDegrees()
        
        desiredDepth = self.ai.data['config'].get('binDepth', 12)

        # Set a minimumDepth
        desiredDepth -= self._offset
        minimumDepth = self._config.get('minimumDepth', 2)
        if desiredDepth < minimumDepth:
            desiredDepth = minimumDepth

        # Go to 5 feet in 5 increments
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = desiredDepth,
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._speed)
        diveMotion = motion.basic.ChangeDepth(
            trajectory = diveTrajectory)
        self.motionManager.setMotion(diveMotion)
        
        self.ai.data['firstSearching'] = True

    def exit(self):
        self.motionManager.stopCurrentMotion()

class Searching(state.State):
    """When the vehicle is looking for a bin"""
    @staticmethod
    def transitions():
        return { vision.EventType.BIN_FOUND : Seeking,
                 motion.basic.MotionManager.FINISHED : End }

    @staticmethod
    def getattr():
        return {'legTime' : 5, 'sweepAngle' : 45, 'speed' : 2.5, 'distance' : 5,
                'forwardSpeed' : 0.15}

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
        
        orientation = self.stateEstimator.getEstimatedOrientation()
        direction = self.ai.data.setdefault(
            'binStartOrientation', orientation.getYaw().valueDegrees())

        # Turn on the vision system
        self.visionSystem.binDetectorOn()

        # Save cruising depth for later surface
        self.ai.data['preBinCruiseDepth'] = self.stateEstimator.getEstimatedDepth()


        #TODO: Change this back into a ZigZag motion. LOOK AT THE OLD VERSION
        currentOrientation = self.stateEstimator.getEstimatedOrientation()
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = math.Vector2.ZERO,
            finalValue = math.Vector2(self._distance,0),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._forwardSpeed)
        
        translateMotion = ram.motion.basic.Translate(translateTrajectory,
                                                     frame = Frame.LOCAL)
        
        self.motionManager.setMotion(translateMotion)

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
        attrs = {}
        attrs.update(HoveringState.getattr())
        attrs.update({'centeredLimit' : 0.2})
        return attrs
 
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

class RecoverSeeking(Recover):
    @staticmethod
    def transitions():
        return Recover.transitions(RecoverSeeking, Seeking)

    @staticmethod
    def getattr():
        attrs = {}
        attrs.update(HoveringState.getattr())
        attrs.update(Recover.getattr())
        attrs.update({'speedGain' : 5, 'sidewaysSpeedGain' : 5, 'yawGain' : 1,
                      'maxSpeed' : 1.5, 'maxSidewaysSpeed' : 1.5, 
                      'forwardSpeed' : 0.15})
        return attrs

    def enter(self, timeout = 4):
        Recover.enter(self, timeout)
        
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = math.Vector2.ZERO,
            finalValue = math.Vector2(self.ai.data["lastBinY"],
                                          self.ai.data["lastBinX"]),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._forwardSpeed)

        translateMotion = ram.motion.basic.Translate(translateTrajectory,
                                                     frame = Frame.LOCAL)
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = self.ai.data.get("preBinCruiseDepth", 7),
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = 0.3)
        diveMotion = motion.basic.ChangeDepth(
            trajectory = diveTrajectory)
        self.motionManager.setMotion(translateMotion, diveMotion)

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
            trans = { Centering.SETTLED : SeekEnd })
    
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
            'binDirection', 0)
        if self._binDirection is None:
            SettlingState.enter(self, Centering.SETTLED, 5,
                                useMultiAngle = True)
        else:
            SettlingState.enter(self, Centering.SETTLED, 5,
                                useMultiAngle = True, shouldRotate = False)
        
        self._currentDesiredOrientation = math.Quaternion(
            math.Degree(self._binDirection), math.Vector3.UNIT_Z)
    
        currentOrientation = self.stateEstimator.getEstimatedOrientation()
        yawTrajectory = motion.trajectories.StepTrajectory(
            initialValue = currentOrientation,
            # possible screw-up point, use local, not absolute
            finalValue = self._currentDesiredOrientation,
            initialRate = self.stateEstimator.getEstimatedAngularRate(),
            finalRate = math.Vector3.ZERO)
        
        yawMotion = motion.basic.ChangeOrientation(yawTrajectory)

        self.motionManager.setMotion(yawMotion)
        
class RecoverCentering(Recover):
    @staticmethod
    def transitions():
        return Recover.transitions(RecoverCentering, Centering)
        
class SeekEnd(BinSortingState):
    """
    Goes to the left most visible bin
    """
    POSSIBLE_END = core.declareEventType('POSSIBLE_END_')
    AT_END = core.declareEventType('AT_END')
    
    @staticmethod
    def transitions():
        return HoveringState.transitions(SeekEnd,
            lostState = RecoverSeekEnd, recoveryState = LostCurrentBinSeekEnd,
            trans = { BinSortingState.CENTERED_ : SeekEnd, 
                      SeekEnd.POSSIBLE_END : SeekEnd,
                      SeekEnd.AT_END : Dive })

    @staticmethod
    def getattr():
        attrs = {}
        attrs.update(BinSortingState.getattr())
        attrs.update({'timeout' : 5})
        return attrs
    
    def BIN_FOUND(self, event):
        # Cancel out angle commands (we don't want to control orientation)
        event.angle = math.Degree(0)
        BinSortingState.BIN_FOUND(self, event)
        
    def POSSIBLE_END_(self, event):
        self._timer = None
        if not self.fixEdgeBin():
            # If already there
            self.publish(SeekEnd.AT_END, core.Event())
            
    def _checkEnd(self):
        # Check if we've lost the bins already. If so, wait for
        # BIN_DROPPED to take over
        if len(self.ai.data['binData']['currentIds']) > 0:
            if not self.hasBinToLeft():
                self.ai.data['startSide'] = BinSortingState.LEFT
            elif not self.hasBinToRight():
                self.ai.data['startSide'] = BinSortingState.RIGHT
            else:
                # This direction doesn't matter
                self.ai.data['startSide'] = \
                    self._config.get('startDirection', BinSortingState.RIGHT)
        
    def enter(self):
        # Keep the hover motion going
        self._binDirection = self.ai.data['config'].get('Bin', {}).get(
            'binDirection', None)
        if self._binDirection is None:
            BinSortingState.enter(self, self.ai.data.get('startSide',
                                  BinSortingState.LEFT), useMultiAngle = True)
        else:
            BinSortingState.enter(self, self.ai.data.get('startSide',
                                  BinSortingState.LEFT), useMultiAngle = True,
                                  shouldRotate = False)
        
        # Check if it is already at the end and set the direction to move
        self._checkEnd()
        self.setSortDirection(self.ai.data['startSide'])
        
        # Set orientation to match the initial orientation
        if self.ai.data.has_key('binArrayOrientation'):
            self._currentDesiredOrientation = math.Quaternion(
                self.ai.data['binArrayOrientation'].getYaw(),
                math.Vector3.UNIT_Z)
            currentOrientation = self.stateEstimator.getEstimatedOrientation()
            yawTrajectory = motion.trajectories.StepTrajectory(
                initialValue = currentOrientation,
                finalValue = self._currentDesiredOrientation,
                initialRate = self.stateEstimator.getEstimatedAngularRate(),
                finalRate = math.Vector3.ZERO)
            
            yawMotion = motion.basic.ChangeOrientation(yawTrajectory)

            self.motionManager.setMotion(yawMotion)
        
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
            
    def exit(self):
        if self._timer is not None:
            self._timer.stop()
        
class RecoverSeekEnd(Recover):
    @staticmethod
    def transitions():
        return Recover.transitions(RecoverSeekEnd, SeekEnd)

class LostCurrentBinSeekEnd(LostCurrentBin):
    @staticmethod
    def transitions():
        return LostCurrentBin.transitions(LostCurrentBinSeekEnd,
                                          RecoverSeekEnd,
                                          SeekEnd)
   
class Dive(HoveringState):
    """
    Gets us down to the depth we can check the symbols out at
    """
    
    COMPLETE = core.declareEventType('COMPLETE')
    
    @staticmethod
    def transitions():
        return SettlingState.transitions(Dive,
        lostState = RecoverDive, recoveryState = LostCurrentBinDive,
        trans = { Dive.COMPLETE : Aligning })

    @staticmethod
    def getattr():
        attrs = {}
        attrs.update(HoveringState.getattr())
        attrs.update({'offset' : 1.5, 'diveSpeed' : 0.3})
        return attrs

    def BIN_FOUND(self, event):
        # Disable angle tracking
        event.angle = math.Degree(0)
        HoveringState.BIN_FOUND(self, event)
        
    def enter(self, useMultiAngle = False, offset = 1.5):

        self._timer = self.timerManager.newTimer(Dive.COMPLETE, 10)
        self._timer.start()
        self._binDirection = self.ai.data['config'].get('Bin', {}).get(
            'binDirection', None)

        # Keep the hover motion going (and use the bin angle)
        if self._binDirection is None:
            HoveringState.enter(self, useMultiAngle)
        else:
            HoveringState.enter(self, useMultiAngle, shouldRotate = False)
            
        self._recalculate = False

        # Set orientation to match the initial orientation
        if self.ai.data.has_key('binArrayOrientation'):
            self._currentDesiredOrientation = math.Quaternion(
                self.ai.data['binArrayOrientation'].getYaw(),
                math.Vector3.UNIT_Z)
            currentOrientation = self.stateEstimator.getEstimatedOrientation()
            yawTrajectory = motion.trajectories.StepTrajectory(
                initialValue = currentOrientation,
                finalValue = self._currentDesiredOrientation,
                initialRate = self.stateEstimator.getEstimatedAngularRate(),
                finalRate = math.Vector3.ZERO)
            
            yawMotion = motion.basic.ChangeOrientation(yawTrajectory)
            
            self.motionManager.setMotion(yawMotion)

        # While keeping center, dive down
        binDepth = self.ai.data['config'].get('binDepth', 11)

        if offset == 1.5:
            offset = self._offset
        offset_ = offset
        offset_ = offset_ + self.ai.data.get('dive_offsetTheOffset', 0) + \
            self.ai.data.get('closerlook_offsetTheOffset', 0)
        
        # Set a minimumDepth
        binDepth -= offset_
        minimumDepth = self._config.get('minimumDepth', 2)
        if binDepth < minimumDepth:
            binDepth = minimumDepth
        
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = binDepth,
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = 0.3)
        diveMotion = motion.basic.ChangeDepth(
            trajectory = diveTrajectory)
            
        self.motionManager.setMotion(diveMotion)
        
    def exit(self):
        if self._timer is not None:
            self._timer.stop()
     
class RecoverDive(Recover):
    @staticmethod
    def transitions():
        trans = Recover.transitions(RecoverDive, Dive)
        trans.update({ Recover.MOVE_ON : SurfaceToMove })
        
        return trans

    @staticmethod
    def getattr():
        attrs = {}
        attrs.update(HoveringState.getattr())
        attrs.update(Recover.getattr())
        attrs.update({'timeout' : 4, 'increase' :  0.25, 'maxIncrease' : 1,
                      'diveSpeed' : 0.3})
        return attrs

    def enter(self):
        timeout = self._timeout
        Recover.enter(self, timeout = timeout)
        self.ai.data['dive_offsetTheOffset'] = \
            self.ai.data.get('dive_offsetTheOffset', 0) + self._increase
            
        if self.ai.data['dive_offsetTheOffset'] > self._maxIncrease:
            self.publish(Recover.MOVE_ON, core.Event())
        else:
            depth = self.stateEstimator.getEstimatedDepth()
            offset = self.ai.data['dive_offsetTheOffset']

            diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
                initialValue = self.stateEstimator.getEstimatedDepth(),
                finalValue = self.ai.data['config'].get('binDepth', 12) - offset,
                initialRate = self.stateEstimator.getEstimatedDepthRate(),
                avgRate = self._diveSpeed)
            diveMotion = motion.basic.ChangeDepth(
                trajectory = diveTrajectory)
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
            trans = { Aligning.ALIGNED : PreDiveExamine })
    
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
        
class Examine(HoveringState):
    """
    Turns on the symbol detector, and determines the type of the bin
    """
    FOUND_TARGET = core.declareEventType('FOUND_TARGET')
    MOVE_ON = core.declareEventType('MOVE_ON')
    TIMEOUT = core.declareEventType('TIMEOUT')

    @staticmethod
    def transitions(myState, lostState, recoveryState, foundTarget, moveOn):
        return HoveringState.transitions(myState,
        lostState = lostState, recoveryState = recoveryState,
        trans = { Examine.FOUND_TARGET : foundTarget,
                  Examine.MOVE_ON : moveOn,
                  Examine.TIMEOUT : myState })

    @staticmethod
    def getattr():
        attrs = {}
        attrs.update(HoveringState.getattr())
        attrs.update({'minimumHits' : 40, 'timeout' : 2, 'foundLimit' : 0.7})
        return attrs

    def BIN_FOUND(self, event):
        
        if not self._adjustAngle:
            event.angle = math.Degree(0)
        
        HoveringState.BIN_FOUND(self, event)

        # Only look at the current bin
        if self._currentBin(event):
            if self.ai.data['doICare']:
                # Check if it's over the minimum required data
                if self.ai.data['binData']['histogram'][event.id]['totalHits'] >= \
                        self._minimumHits:
                    # Have it determine the symbol if it's got enough data
                    self._determineSymbols(event.id)
            else:
                self.publish(Examine.FOUND_TARGET, core.Event())

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
        targetSymbols = self.ai.data['config'].get(
                            'targetSymbols', ['Club', 'Diamond'])
        
        possibleTargetSymbols = set()
        symbolNames = []
        for symbol in targetSymbols:
            symbolName = symbol.upper()
            if hasattr(vision.Symbol, symbolName):
                possibleTargetSymbols.add(getattr(vision.Symbol, symbolName))
                symbolNames.append(symbolName)

        droppedSymbols = self.ai.data.get('droppedSymbols', set())
        self._targetSymbols = possibleTargetSymbols.difference(droppedSymbols)
        self.ai.data.setdefault('targetSymbols', symbolNames)

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
        self._adjustAngle = self.ai.data['config'].get('Bin', {}).get(
            'adjustAngle', True)

        if self._adjustAngle:
            HoveringState.enter(self)
        else:
            HoveringState.enter(self, shouldRotate = False)
        
        # Load needed symbols
        self._loadSymbolConfig()
        
        if timeout != 2:
            self._timeout = timeout

        if self._timeout >= 0:
            self._timer = self.timerManager.newTimer(Examine.TIMEOUT,
                                                     self._timeout)
            self._timer.start()

    def exit(self):
        if self._timer is not None:
            self._timer.stop()

class LostCurrentBinExamine(LostCurrentBin):
    # Put more specific actions for Examine recoveries later
    pass

class PreDiveExamine(Examine):

    LOOK_CLOSER = core.declareEventType('LOOK_CLOSER')

    @staticmethod
    def transitions():
        trans = Examine.transitions(myState = PreDiveExamine,
                                   lostState = RecoverDive,
                                   recoveryState = LostCurrentBinPreDiveExamine,
                                   foundTarget = CloserLook,
                                   moveOn = SurfaceToMove)
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
    
class LostCurrentBinPreDiveExamine(LostCurrentBinExamine):
    @staticmethod
    def transitions():
        return LostCurrentBinExamine.transitions(
            myState = LostCurrentBinPreDiveExamine,
            lostState = RecoverDive,
            originalState = PreDiveExamine)
        
class CloserLook(Dive):
    """
    Drop down before dropping the marker.
    """
    
    @staticmethod
    def transitions():
        return SettlingState.transitions(CloserLook,
        lostState = RecoverCloserLook, recoveryState = LostCurrentBinCloserLook,
        trans = { Dive.COMPLETE : PostDiveExamine })

    @staticmethod
    def getattr():
        attrs = {}
        attrs.update(Dive.getattr())
        attrs.update({'offset' : 1.0})
        return attrs
        
    def enter(self):
        # Standard dive
        Dive.enter(self, useMultiAngle = False,
                   offset = self._offset)
        
class RecoverCloserLook(Recover):
    @staticmethod
    def transitions():
        trans = Recover.transitions(RecoverCloserLook, CloserLook)
        trans.update({ Recover.MOVE_ON : SurfaceToMove })
        
        return trans

    @staticmethod
    def getattr():
        attrs = {}
        attrs.update(Recover.getattr())
        attrs.update({'timeout' : 4, 'increase' : 0.25, 'maxIncrease' : 1,
                      'diveSpeed' : 0.3})
        return attrs

    def enter(self):
        timeout = self._timeout
        Recover.enter(self, timeout = timeout)
        self.ai.data['closerlook_offsetTheOffset'] = \
            self.ai.data.get('closerlook_offsetTheOffset', 0) + self._increase
            
        if self.ai.data['closerlook_offsetTheOffset'] > self._maxIncrease:
            self.publish(Recover.MOVE_ON, core.Event())
        else:
            depth = self.stateEstimator.getEstimatedDepth()
            offset = self.ai.data['closerlook_offsetTheOffset']
            diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
                initialValue = self.stateEstimator.getEstimatedDepth(),
                finalValue = depth - offset,
                initialRate = self.stateEstimator.getEstimatedDepthRate(),
                avgRate = self._diveSpeed)
            diveMotion = motion.basic.ChangeDepth(
                trajectory = diveTrajectory)
            self.motionManager.setMotion(diveMotion)
            
class LostCurrentBinCloserLook(LostCurrentBin):
    @staticmethod
    def transitions():
        return LostCurrentBin.transitions(myState = LostCurrentBinCloserLook,
                                          lostState = RecoverCloserLook,
                                          originalState = CloserLook)

class PostDiveExamine(Examine):
    @staticmethod
    def transitions():
        return Examine.transitions(myState = PostDiveExamine,
                                  lostState = RecoverCloserLook,
                                  recoveryState = LostCurrentBinPostDiveExamine,
                                  foundTarget = DropMarker,
                                  moveOn = SurfaceToMove)

    @staticmethod
    def getattr():
        attrs = {}
        attrs.update(Examine.getattr())
        attrs.update({'minimumHitsOnTimeout' : attrs['minimumHits']/2})
        return attrs

    def TIMEOUT(self, event):
        histogram = self.ai.data['binData']['histogram']
        currentID = self.ai.data['binData'].get('currentID', None)

        # Make sure there is a currentID
        if currentID is not None:
            # Check if it has half the number of hits it needs
            # If so, go for it anyways
            if histogram[currentID]['totalHits'] >= self._minimumHitsOnTimeout:
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
    
class LostCurrentBinPostDiveExamine(LostCurrentBin):
    @staticmethod
    def transitions():
        return LostCurrentBin.transitions(
            myState = LostCurrentBinPostDiveExamine,
            lostState = RecoverCloserLook,
            originalState = PostDiveExamine)
               
class SurfaceToMove(HoveringState):
    """
    Goes back to starting cruise depth we had before we started the bins
    """

    @staticmethod
    def transitions():
        return SettlingState.transitions(SurfaceToMove,
            lostState = RecoverSurfaceToMove,
            recoveryState = LostCurrentBinSurfaceToMove,
            trans = { motion.basic.MotionManager.FINISHED : NextBin })

    @staticmethod
    def getattr():
        attrs = {}
        attrs.update(HoveringState.getattr())
        attrs.update({'offset' : 2, 'surfaceSpeed' : 1.0/3.0})
        return attrs
        
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
        
        offset = self._offset + self.ai.data.get('dive_offsetTheOffset', 0) + \
            self.ai.data.get('closerlook_offsetTheOffset', 0)

        # Set a minimumDepth
        binDepth -= offset
        minimumDepth = self._config.get('minimumDepth', 2)
        if binDepth < minimumDepth:
            binDepth = minimumDepth

        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = binDepth,
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._surfaceSpeed)
        diveMotion = motion.basic.ChangeDepth(
            trajectory = diveTrajectory)
        self.motionManager.setMotion(diveMotion)
        
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
                      NextBin.AT_END : CheckDropped })
    
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
            trans = { DropMarker.FINISHED : End,
                      DropMarker.CONTINUE : SurfaceToMove,
                      DropMarker.DROP : DropMarker })

    def _dropMarker(self):
        # Increment marker dropped count
        markerNum = self.ai.data.get('markersDropped',0)
        self.ai.data['markersDropped'] = markerNum + 1
        
        droppingSymbol = self.ai.data['droppingSymbol']
        targetSymbols = self.ai.data['targetSymbols']
        
        # Hackish solution
        value = 0
        for symbol in targetSymbols:
            if symbol == droppingSymbol:
                break
            else:
                value = value + 1

        if value == len(targetSymbols):
            value = 0

        # Release the marker
        self.vehicle.dropMarkerIndex(value)

        if self.ai.data['doICare']:
            # Mark that we dropped the symbol
            self.ai.data.setdefault('droppedSymbols', set()).add(droppingSymbol)

        markerNum = self.ai.data['markersDropped']
        if markerNum < 2:
            self.publish(DropMarker.CONTINUE, core.Event())
        else:
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
            SettlingState.enter(self, DropMarker.DROP, 5)
        else:
            SettlingState.enter(self, DropMarker.DROP, 5, shouldRotate = False)
        
class CheckDropped(HoveringState):
    """
    Check if all of the markers have been dropped.
    """
    
    FINISH = core.declareEventType('FINISH')
    RESTART = core.declareEventType('RESTART')
    
    @staticmethod
    def transitions():
        return HoveringState.transitions(CheckDropped,
            lostState = CheckDropped,
            trans = { CheckDropped.FINISH : End,
                      CheckDropped.RESTART : Dive })

    @staticmethod
    def getattr():
        attrs = {}
        attrs.update(HoveringState.getattr())
        attrs.update({'maximumScans' : 2})
        return attrs

    def BIN_FOUND(self, event):
        event.angle = math.Degree(0)
        HoveringState.BIN_FOUND(self, event)
        
    def enter(self):
        HoveringState.enter(self, shouldRotate = False)

        data = self.ai.data
        
        if data.get('markersDropped', 0) < 2:
            # We haven't gotten them all
            # Increment the number of times through  and restart
            data['numOfScans'] = data.get('numOfScans', 0) + 1
            if data['startSide'] == BinSortingState.LEFT:
                data['startSide'] = BinSortingState.RIGHT
            else:
                data['startSide'] = BinSortingState.LEFT
            
            if data['numOfScans'] == -1 or \
                    data['numOfScans'] < self._maximumScans:
                self.publish(CheckDropped.RESTART, core.Event())
            else:
                if not data['doICare']:
                    self.publish(CheckDropped.FINISH, core.Event())
                else:
                    data['doICare'] = False
                    self.publish(CheckDropped.RESTART, core.Event())
        else:
            # We've dropped them all. Finish.
            self.publish(CheckDropped.FINISH, core.Event())
        
class SurfaceToCruise(state.State):
    """
    Goes back to starting cruise depth we had before we started the bins
    """
    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : End }

    @staticmethod
    def getattr():
        return {'surfaceSpeed' : 1.0/3.0}
        
    def enter(self):
        # Surface
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = self.ai.data['preBinCruiseDepth'],
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._surfaceSpeed)
        diveMotion = motion.basic.ChangeDepth(
            trajectory = diveTrajectory)
        self.motionManager.setMotion(diveMotion)

class End(state.State):
    def enter(self):
        # If the offset values exist, delete them
        if self.ai.data.has_key('dive_offsetTheOffset'):
            del self.ai.data['dive_offsetTheOffset']
        if self.ai.data.has_key('closerlook_offsetTheOffset'):
            del self.ai.data['closerlook_offsetTheOffset']

        self.visionSystem.binDetectorOff()
        self.publish(COMPLETE, core.Event())
