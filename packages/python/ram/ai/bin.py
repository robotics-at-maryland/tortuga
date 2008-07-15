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
    
    def BIN_FOUND(self, event):
        """Update the state of the light, this moves the vehicle"""
        # Only listen to the current bin ID
        if self.ai.data.get('currentBinID', 0) == event.id:
            self._bin.setState(event.x, event.y)

    def enter(self):
        # Make sure we are tracking
        ensureBinTracking(self.queuedEventHub, self.ai)
        
        self._bin = ram.motion.common.Target(0,0)
        motion = ram.motion.common.Hover(target = self._bin,
                                         maxSpeed = 5,
                                         maxSidewaysSpeed = 3)
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


class Centering(SettlingState):
    """
    When the vehicle is settling over the bin
    
    @cvar SETTLED: Event fired when vehile has settled over the bin
    """
    SETTLED = core.declareEventType('SETTLED')
    
    @staticmethod
    def transitions():
        return SettlingState.transitions(Centering,
            { Centering.SETTLED : Dive })
    
    def enter(self):
        SettlingState.enter(self, Centering.SETTLED, 5)
        
#class SeekEnd(HoveringState):
   
class Dive(HoveringState):
    @staticmethod
    def transitions():
        return { vision.EventType.BIN_LOST : Searching,
                 vision.EventType.BIN_FOUND : Dive,
                 motion.basic.Motion.FINISHED : DropMarker }
        
    def enter(self):
        # Keep the hover motion going
        HoveringState.enter(self)
        
        # While keeping center, dive down
        diveMotion = motion.basic.RateChangeDepth(
            desiredDepth = self._config.get('depth', 8),
            speed = self._config.get('diveSpeed', 0.4))
        
        self.motionManager.setMotion(diveMotion)
        
class DropMarker(SettlingState):
    DROPPED = core.declareEventType('DROPPPED')
    
    @staticmethod
    def transitions():
        return SettlingState.transitions(Seeking,
            { DropMarker.DROPPED : Surface })

    def enter(self):
        SettlingState.enter(self, DropMarker.DROPPED, 15)
        # TODO: drop marker here
        
class Surface(HoveringState):
    @staticmethod
    def transitions():
        return SettlingState.transitions(Seeking,
            { motion.basic.Motion.FINISHED : End })
        
    def enter(self):
        # Keep centered over the bin
        HoveringState.enter(self)
        
        # Also surface
        surfaceMotion = motion.basic.RateChangeDepth(
            desiredDepth = self._config.get('depth', 3),
            speed = self._config.get('surfaceSpeed', 1.0/3.0))
        
        self.motionManager.setMotion(surfaceMotion)

class End(state.State):
    def enter(self):
        self.visionSystem.binDetectorOff()
        self.publish(COMPLETE, core.Event())
