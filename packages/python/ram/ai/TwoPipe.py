# Project Imports
import ext.core as core
import ext.vision as vision
import math as m
import ext.math as math
import ext.estimation as estimation
from ext.control import yawVehicleHelper

import ram.ai.state as state
import ram.motion as motion
import ram.timer
from ram.motion.basic import Frame
import ram.ai.Utility as util
import ram.ai.Approach as vs

import ram.ai.GoodPipe as GoodPipe

DESIRED_VECTOR = math.Vector2(0,0)
PIPE_VECTOR = []

def processEvent(you, event):
    global DESIRED_VECTOR
    global PIPE_VECTOR
    currentOrientation = you.stateEstimator.getEstimatedOrientation()
    currentYaw = currentOrientation.getYaw().valueDegrees()
    currentVec = math.Vector2(m.cos(currentYaw), m.sin(currentYaw))

    pipeYaw = currentYaw - event.angle
    pipeVec = math.Vector2(m.cos(pipeYaw), m.sin(pipeYaw))

    if pipeVec.dotProduct(DESIRED_VECTOR) < 0:
        pipeVec = -pipeVec
    
    PIPE_VECTOR[event.id] = pipeVec

    #Get which pipe is closer to the desired vector
    desired_id = 1
    if PIPE_VECTOR[1].dotProduct(DESIRED_VECTOR) < PIPE_VECTOR[2].dotProduct(DESIRED_VECTOR):
        desired_id = 2

    #If this is not the pipe we're looking for, return None
    if event.id != desired_id:
        return None

    #Get the true anle and put it in the correct range
    angle = currentVec.angleTo(pipeVec).valueDegrees()
    if angle > 180:
        angle -= 360
    elif angle < -180:
        angle += 360
    return -angle

class Centering(GoodPipe.Centering):
    @staticmethod 
    def transitions():
        return {vs.DONE : Align , vision.EventType.PIPE_FOUND : Centering}

    @staticmethod
    def getattr():
        return { 'kx' : .15 ,  'ky' : .4 , 'kz' : 9, 
        		 'x_d' : 0, 'r_d' : 50 , 'y_d' : 0, 
        		 'x_bound': .05, 'r_bound': 20, 'y_bound':.025 ,
        		 'minvx': .1, 'minvy': .1 ,'minvz' : .1 ,
        		 'direction' : 0 }

    def enter(self):
        global DESIRED_VECTOR
        global PIPE_VECTOR
        DESIRED_VECTOR = math.Vector2(m.cos(self._direction), m.sin(self._direction))
        PIPE_VECTOR.insert(1, math.Vector2(0,0))
        PIPE_VECTOR.insert(2, math.Vector2(0,0))

    def PIPE_FOUND(self, event):
    	event.angle = processEvent(self, event)

        if event.angle != None:
            GoodPipe.Centering.PIPE_FOUND(self, event)

class Align(GoodPipe.Align):
    @staticmethod
    def getattr():
        return {'yaw_bound' : .05}

    def PIPE_FOUND(self,event):
        event.angle = processEvent(self, event)

        if event.angle != None:
            GoodPipe.Align.PIPE_FOUND(self, event)
            
    @staticmethod
    def transitions():
        return {GoodPipe.DONE: state.State, vision.EventType.PIPE_FOUND : Align}

