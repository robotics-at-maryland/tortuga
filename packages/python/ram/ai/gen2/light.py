# Copyright (C) 2010 Maryland Robotics Club
# Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
# All rights reserved.
#
# Author: Jonathan Sternberg <jsternbe@umd.edu>
# File:  packages/python/ram/ai/gen2/light.py

"""
A state machine for finding the light using the map as an aid.

The AI moves under the assumption that it has just come off a pipe line
and the light should be directly in front of it. It then uses the map to
validate that assumption.

It can do one of two things based on this.

1) The map agrees with the above assumption
 - Moves forward to where the map believes the light is located
 - If nothing is found, zig zag forward to try and find the light
 - If the light is found, align and hit the buoy using the vision system
2) The map disagrees with the above assumption
 - Zig zag movement forwards (assumes the map is incorrect)
 - If nothing is found after a timeout, move toward where the map
       says the buoy is located

Both methods default to the map if things go wrong.

"""

# STD Imports
import math as pmath

# Project Imports
import ext.core as core
import ext.vision as vision
import ext.math as math

import ram.ai.state as state
import ram.ai.light as legacy
import ram.motion as motion
import ram.timer as timer

COMPLETE = core.declareEventType('COMPLETE')
FAILURE = core.declareEventType('FAILURE')

class Start(state.State):
    """
    Makes sure the buoy object is located in the map
    """
    SUCCESS = core.declareEventType('SUCCESS')
    FAIL = core.declareEventType('FAIL')

    @staticmethod
    def transitions():
        return { Start.SUCCESS : Dive,
                 Start.FAIL : Failure }

    def enter(self):
        # Interstate data is stored here
        # All previous data will be lost
        self.ai.data['light'] = {}

        # TODO: Make 'buoy' a configurable value
        if self.vehicle.hasObject('buoy'):
            self.publish(Start.SUCCESS, core.Event())
        else:
            self.publish(Start.FAIL, core.Event())

class Dive(state.State):
    """
    Dives to the depth specified in the map
    """
    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : DeterminePath }

    def enter(self):
        # Check the map for the depth
        depth = self.vehicle.getDepth('buoy')
        diveMotion = motion.basic.RateChangeDepth(depth, 0.3)

        self.motionManager.setMotion(diveMotion)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class DeterminePath(state.State):
    """
    Determines which method of searching to use

    This has the capability to choose to follow the map, but
    it is not currently programmed to use that transition.
    """
    MAP = core.declareEventType('MAP')
    FORWARD = core.declareEventType('FORWARD')
    ZIGZAG = core.declareEventType('ZIGZAG')

    @staticmethod
    def transitions():
        return { DeterminePath.MAP : SearchMap,
                 DeterminePath.FORWARD : SearchForward,
                 DeterminePath.ZIGZAG : SearchZigZag }

    @staticmethod
    def getattr():
        return set(['threshold'])

    def enter(self):
        self._angleThreshold = self._config.get('threshold', 25)

        # Find the angle
        vehiclePos = self.vehicle.getPosition()
        buoyPos = self.vehicle.getPosition('buoy')
        posVect = buoyPos - vehiclePos
        posVect.normalise()

        unit = math.Vector2(0, 1)
        angle = math.Radian(pmath.acos(posVect.dotProduct(unit)))

        if abs(angle.valueDegrees()) < self._angleThreshold:
            self.publish(DeterminePath.FORWARD, core.Event())
        else:
            self.publish(DeterminePath.ZIGZAG, core.Event())

class Searching(state.State):
    """
    Searching template
    """
    @staticmethod
    def transitions():
        return { vision.EventType.LIGHT_FOUND : Align }

class SearchForward(Searching):
    """
    Searches directly in front of the vehicle
    """
    @staticmethod
    def transitions():
        trans = Searching.transitions()
        trans.update({ motion.basic.MotionManager.FINISHED : SearchZigZag })

        return trans

    def enter(self):
        # Turn red light detector on
        self.visionSystem.redLightDetectorOn()

        # Get the estimated distance to the object
        vehiclePos = self.vehicle.getPosition()
        lightPos = self.vehicle.getPosition('buoy')

        length = (lightPos - vehiclePos).length()

        speed = self._config.get('speed', 3)
        offset = self._config.get('offset', 1)

        length -= offset

        # Move that distance forwards
        moveMotion = motion.basic.MoveDistance(0, length, 3, absolute = False)
        self.motionManager.setMotion(moveMotion)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class SearchZigZag(Searching):
    """
    Searches in a zig zag pattern with no regard to the map
    """
    TIMEOUT = core.declareEventType('TIMEOUT')

    @staticmethod
    def transitions():
        trans = Searching.transitions()
        trans.update({ SearchZigZag.TIMEOUT : SearchMap })

        return trans

    def enter(self):
        # Make sure the red light detector is on
        pass

class SearchMap(Searching):
    """
    Searches for the light based on the maps position
    """
    def enter(self):
        pass

class Align(legacy.Align):
    """
    Aligns to the light using legacy code
    """
    @staticmethod
    def transitions():
        # Modify legacy transitions to move to the gen2 align and seek
        trans = legacy.Align.transitions()
        for event, state in trans.iteritems():
            if state == legacy.Align:
                trans[event] = Align
            elif state == legacy.Seek:
                trans[event] = Seek

        return trans

class Seek(legacy.Seek):
    """
    Seeks the light using legacy code
    """
    @staticmethod
    def transitions():
        return { vision.EventType.LIGHT_LOST : SearchMap,
                 vision.EventType.LIGHT_FOUND : Seek,
                 vision.EventType.LIGHT_ALMOST_HIT : Hit }

class Hit(legacy.Hit):
    """
    Hits the light using legacy code
    """
    @staticmethod
    def transitions():
        return { legacy.Hit.FORWARD_DONE : Continue }

class Continue(legacy.Continue):
    """
    Moves around the buoy
    """
    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Complete }

class End(state.State):
    """
    Template for ending the state
    """
    def enter(self):
        self.motionManager.stopCurrentMotion()
        # Cleanup state data
        del self.ai.data['light']

class Complete(End):
    def enter(self):
        End.enter(self)
        self.publish(COMPLETE, core.Event())

class Failure(End):
    def enter(self):
        End.enter(self)
        self.publish(FAILURE, core.Event())
