import time
import ram.ai.new.utilClasses as utilClasses
import searchPatterns as search
import ram.ai.new.utilStates as utilStates
import ram.ai.new.motionStates as motionStates

import ram.ai.new.acousticServoing as acousticServoing
import ram.ai.new.approach as approach
import ram.ai.new.checkpoints as checkpoints

import ram.ai.new.gate as gate
import ram.ai.new.Buoy2014 as buoy
#import ram.ai.new.SonarManip2014 as sonarm
import ram.ai.new.uprights as uprights


import ram.ai.new.Buoy2014 as buoy


from state import *
from stateMachine import *

import ext.math as math
import ram.motion as motion
from ram.motion.basic import Frame

#a terrible pun
def reverseFun(fun):
    def rev():
        return not fun()
    return rev

class TestMachine(StateMachine):
    def __init__(self):
        super(TestMachine, self).__init__()
#<<<<<<< HEAD
        #pipe = utilClasses.OldSimulatorHackPipe(self.getLegacyState())
        pinger = utilClasses.OldSimulatorHackSonarObject(self.getLegacyState())
        start = self.addState('start',utilStates.Start())
        end = self.addState('end',utilStates.End())
#<<<<<<< HEAD
#        start.setTransition('next','test')
#        test = self.addState('test', utilStates.NestedState(sonarm.PogoMotion(4,4,2,6)))
#        test.setTransition('complete', 'end')
        #upRightsTask = self.addState('uprights', 
        #                             uprights.UprightsTask(buoy, pipe, 
        #                                                   8, 4, 2, 2,
        #                                                   1, 1, 1.5,
        #                                                   'end', 'yaw', 
        #                                                   300))
        #yaw = self.addState('yaw', motionStates.Turn(30))
        #start.setTransition('next', 'uprights')
        #yaw.setTransition('next', 'end')
        #print 'testing'
#=======
        #center = self.addState('center', buoy.BuoySearchState(utilClasses.OldSimulatorHackVisionObject(self.getLegacyState()), 2, 100, 0.25, 2))
        center = self.addState('center', centering.SonarCenter(pinger, 'end', 'end', math.Vector3(0.0,0.0,3.0)))
        #align = self.addState('align', centering.DownOrient(pipe, 'end', 'end'))
        #acoustic = self.addState('acoustic', acousticServoing.AcousticServoing(pinger, math.Vector3(0.0,0.0,3.0)))
        start.setTransition('next', 'center')
        center.setTransition('complete', 'end')
        center.setTransition('failure', 'end'
#>>>>>>> rajath/vehicle_refactor_2014
#=======
#        self.addStates({
#            'start' : utilStates.Start(),
#            'save' : checkpoints.SaveCheckpoint(checkpoint = 'test'),
#            'forward' : motionStates.Move(4, 2),
#            'return' : checkpoints.GotoCheckpoint(checkpoint = 'test',
#                                                  x_offset = 2,
#                                                  y_offset = 2),
#            'end' : utilStates.End()
#          })
#        self.addTransitions(
#            ('start', 'next', 'save'),
#            ('save', 'next', 'forward'),
#            ('forward', 'next', 'return'),
#            ('return', 'next', 'end'),
#          )
#>>>>>>> mainRepo/vehicle_refactor_2014
