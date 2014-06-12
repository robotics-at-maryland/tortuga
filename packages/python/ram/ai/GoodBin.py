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
import ram.ai.Approach as approach
import ram.ai.Utility as util

import inspect

##########################################################################
#Here we go everybody!
##########################################################################

SEARCH_DEPTH = 3
VANTAGE_DEPTH = 7

'''
Stores the data from the bin event in a HyperApproach compatible format
'''
class BinData:
    def __init__(self, x, y, z, angle, typ):
        self.x = x
        self.y = y
        self.range = z
        self.angle = angle
        self.visible = (typ > -2)
        self.identified = (typ > -1)
        self.number = typ

class BinApproach( approach.DHyperApproach ):
    @staticmethod
    def transitions():
        return { vision.EventType.BIN_FOUND : BinApproach , 
                 approach.DONE : state.State }

    @staticmethod
    def getattr():
        return { 'kx' : .15 , 'ky' : .4 , 'kz' : 9 , 
                 'x_d' : 0 , 'r_d' : 50 , 'y_d' : 0 , 
                 'x_bound': .05 , 'r_bound': 20 , 'y_bound':.025 ,
                 'minvx': .1 , 'minvy': .1 , 'minvz' : .1} 

    '''
    Parses a Bin Event and turns it into an array[5] of BinData objects
    '''
    def parseEvent(self, event):
        binData = [BinData(0,0,0,0,0), BinData(0,0,0,0,0), BinData(0,0,0,0,0), BinData(0,0,0,0,0), BinData(0,0,0,0,0), BinData(0,0,0,0,0)]

        p = event.vectorbin0
        binData[0] = BinData(p.x, p.y, p.z, event.angle, -1)

        p = event.vectorbin1
        binData[1] = BinData(p.x, p.y, p.z, event.angle, event.type1)

        p = event.vectorbin2
        binData[2] = BinData(p.x, p.y, p.z, event.angle, event.type2)

        p = event.vectorbin3
        binData[3] = BinData(p.x, p.y, p.z, event.angle, event.type3)

        p = event.vectorbin4
        binData[4] = BinData(p.x, p.y, p.z, event.angle, event.type4)

        members = [attr for attr in dir(event) if not callable(attr) and not attr.startswith("__")]
        print members

        return binData

    def BUOY_FOUND(self, event):
        pass

    def BIN_FOUND(self, event):

        binData = self.parseEvent(event)
        
        for i in range(0,4):
            if binData[i].identified == True and binData[i].number == 37:
                self.run(event)

    def decideZ(self, event):
        return 0

# '''
# Dives to the appropriate depth and starts the task.
# '''
# class Start( util.MotionState ):
#     @staticmethod
#     def transitions():
#         return { motion.basic.MotionManager.FINISHED : Start , 
#                  utility.DONE : ReOrient }

#     @staticmethod
#     def getattr():
#         return { 'rate' : 0.3 }
    
#     def enter( self ):
#         #Lets dive!
#         self.visionSystem.binDetectorOn()
#         self.dive( SEARCH_DEPTH , self._rate )
 
#     def exit( self ):
#         utility.freeze( self )
#         self.motionManager.stopCurrentMotion() 

# '''
# Rotate to face the same ways as the bins
# We will need to set this beforehand, since we 
# don't get an angle measuement in the bin events
# '''
# class ReOrient( util.MotionState ):
#     @staticmethod
#     def transitions():
#         return { motion.basic.MotionManager.FINISHED : ReOrient , 
#                  utility.DONE : InitialSearch }

#     @staticmethod
#     def getattr():
#         return { 'angle' : -20 , 'wait_time' : 5 }

#     def enter( self ):
#         self.yawGlobal( self._angle , self._wait_time )
 
#     def exit( self ):
#         utility.freeze( self )
#         self.motionManager.stopCurrentMotion()


# '''
# Does an initial search pattern to find the first bin.
# TODO: Swap this with ZigZag if we get that working
# '''
# class InitialSearch( util.MotionState ):
#     READY_TO_APPROACH = core.declareEventType('DONE')

#     @staticmethod
#     def transitions():
#         return { motion.basic.MotionManager.FINISHED : InitialSearch , 
#                  vision.EventType.BIN_FOUND : InitialSearch , 
#                  READY_TO_APPROACH : CenterOnAllBins , 
#                  utility.DONE : End }

#     @staticmethod
#     def getattr():
#         return { 'x_dist' : 5 , 'y_dist' : 5 , 'rate' : 0.2 }

#     def enter( self ):
#         #Move towards the bin
#         self.translate( self._x_dist , self._y_dist , self._rate )
 
#     def exit( self ):
#         utility.freeze( self )
#         self.motionManager.stopCurrentMotion() 

# '''
# Positions the robot roughly over the first bin so we know about where we are.
# '''
# class CenterOnAllBins( approach.DHyperApproach ):
#     @staticmethod
#     def transitions():
#         return { vision.EventType.BIN_FOUND : CenterOnAllBins , 
#                 approach.DONE : GetCloser }

#     @staticmethod
#     def getattr():
#         return { 'kx' : .15 , 'ky' : .4 , 'kz' : 9 , 
#                  'x_d' : 0 , 'r_d' : 50 , 'y_d' : 0 , 
#                  'x_bound': .05 , 'r_bound': 20 , 'y_bound':.025 ,
#                  'minvx': .1 , 'minvy': .1 , 'minvz' : .1} 

#     def decideZ( self , event ):
#         return 0

# '''
# Gets closer to the board so we can see the symbols
# TODO: Replace this with an approach maybe?
# '''
# class GetCloser( util.MotionState ):
#     @staticmethod
#     def transitions():
#         return { motion.basic.MotionManager.FINISHED : GetCloser , 
#                  utility.DONE : ApproachFirstBin }

#     @staticmethod
#     def getattr():
#         return { 'rate' : 0.3 }
    
#     def enter( self ):
#         self.dive( VANTAGE_DEPTH , self._rate )
 
#     def exit( self ):
#         utility.freeze( self )
#         self.motionManager.stopCurrentMotion()

# '''
# Dive while approaching the first bin
# TODO: Test this out and see if we actually need to dive to drop the marker.
# '''
# class ApproachFirstBin( approach.DHyperApproach ):
#     @staticmethod
#     def transitions():
#         return { vision.EventType.BIN_FOUND : ApproachFirstBin , 
#                  approach.DONE : BackUp }

#     @staticmethod
#     def getattr():
#         return { 'kx' : .15 , 'ky' : .4 , 'kz' : 9 , 
#                  'x_d' : 0 , 'r_d' : 50 , 'y_d' : 0 , 
#                  'x_bound': .05 , 'r_bound': 20 , 'y_bound':.025 ,
#                  'minvx': .1 , 'minvy': .1 , 'minvz' : .1 } 

#     def BUOY_FOUND:
#         pass

# '''
# Go back up to the depth we were at before
# '''
# class GoBackUp( util.MotionState ):
#     @staticmethod
#     def transitions():
#         return { motion.basic.MotionManager.FINISHED : GetCloser , 
#                  utility.DONE : ReOrient }

#     @staticmethod
#     def getattr():
#         return { 'rate' : 0.3 }
    
#     def enter( self ):
#         self.dive( VANTAGE_DEPTH , self._rate )
 
#     def exit( self ):
#         utility.freeze( self )
#         self.motionManager.stopCurrentMotion()

# class ReCenterOnAllBins( CenterOnAllBins ):
#     @staticmethod
#     def transitions():
#         return { vision.EventType.BIN_FOUND : ReCenterOnAllBins , 
#                 approach.DONE : ApproachSecondBin }

# class ApproachSecondBin( ApproachFirstBin ):
#     @staticmethod
#     def transitions():
#         return { vision.EventType.BIN_FOUND : ReCenterOnAllBins , 
#                 approach.DONE : ApproachSecondBin }

# class End( state.State ):
#     def enter( self ):
#         self.visionSystem.binDetectorOff()
#         self.publish(COMPLETE, core.Event())
