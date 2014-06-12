import ext.core as core
import ext.vision as vision
import ext.control as control
from ext.control import yawVehicleHelper
import ext.math as math

import ram.motion as motion
import ram.motion.basic
from ram.motion.basic import Frame

import ram.ai.task as task
import ram.ai.state as state
import ram.ai.Utility as utility

# ----- Here are Task Imports
import ram.ai.gate as gate
import ram.ai.DeadReckon as deadReckon
import ram.ai.sonar as sonar
import ram.ai.grabDown as grab
import ram.ai.GoodPipe as pipe

import ram.ai.Skip as skip
import ram.ai.TwoPipe as twopipe
import ram.ai.window13final as window
# ----- End of Task Imports



class Gate(task.Task):
  
    # test
    #DONE = core.declareEventType('DONE')
    @staticmethod
    def _transitions():
        return { gate.COMPLETE : Pipe1,
                 #Gate.DONE : task.Next,
                 'GO' : state.Branch(gate.Start) }

    @staticmethod
    def getattr():
        return set(task.Task.getattr())

    def enter(self):
        
        self.stateMachine.start(state.Branch(gate.Start))
        # test
        #self.timer = self.timerManager.newTimer(Gate.DONE, 5)
        #self.timer.start()
        
    def exit(self):
        task.Task.exit(self)

        self.exited = True
        if (self.stateMachine.branches.has_key(gate.Start)):
            self.stateMachine.stopBranch(gate.Start)

# used for the straight shot to sonar from gate
class DeadReckon(task.Task):

    # test
    #DONE = core.declareEventType('DONE')
    @staticmethod
    def _transitions():
        return { deadReckon.COMPLETE : Sonar1,
                 #DeadReckon.DONE : task.Next,
                 'GO' : state.Branch(deadReckon.Start) }

    @staticmethod
    def getattr():
        return set(task.Task.getattr())

    def enter(self):
        
        self.stateMachine.start(state.Branch(deadReckon.Start))
        # test
        #self.timer = self.timerManager.newTimer(DeadReckon.DONE, 5)
        #self.timer.start()
        
    def exit(self):
        task.Task.exit(self)

        self.exited = True
        if (self.stateMachine.branches.has_key(deadReckon.Start)):
            self.stateMachine.stopBranch(deadReckon.Start)

# Sonar detection and approach
class Sonar1(task.Task):

    # test
    #DONE = core.declareEventType('DONE')
    @staticmethod
    def _transitions():
        return { sonar.COMPLETE : Sonar2,
                 #Sonar1.DONE : Sonar2,
                 'GO' : state.Branch(sonar.Start) }

    @staticmethod
    def getattr():
        return set(task.Task.getattr())

    def enter(self):
        
        self.stateMachine.start(state.Branch(sonar.Start))
        # test
        #self.timer = self.timerManager.newTimer(Sonar1.DONE, 3)
        #self.timer.start()
        
    def exit(self):
        task.Task.exit(self)


        self.exited = True
        if (self.stateMachine.branches.has_key(sonar.Start)):
            self.stateMachine.stopBranch(sonar.Start)

# Decent and Ascent with the object
class Sonar2(task.Task):

    # test
    # DONE = core.declareEventType('DONE')
    @staticmethod
    def _transitions():
        return { grab.COMPLETE : task.Next,
                 # Sonar2.DONE : task.Next,
                 'GO' : state.Branch(grab.Start) }

    @staticmethod
    def getattr():
        return set(task.Task.getattr())

    def enter(self):
        print 'here I am'
        self.stateMachine.start(state.Branch(grab.Start))
        # test
        # self.timer = self.timerManager.newTimer(Sonar2.DONE, 5)
        # self.timer.start()
        
    def exit(self):
        task.Task.exit(self)

        self.exited = True
        if (self.stateMachine.branches.has_key(grab.Start)):
            self.stateMachine.stopBranch(grab.Start)










