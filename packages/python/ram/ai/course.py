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
import ram.ai.GoodPipe as pipe
import ram.ai.Skip as nextPipe
import ram.ai.transToTarget as tran
import ram.ai.window13final as window
import ram.ai.GenericDiveStart as dive
# ----- End of Task Imports



class Dive(task.Task):
  
    # test timer
    #DONE = core.declareEventType('DONE')
    @staticmethod
    def _transitions():
        return { dive.COMPLETE : task.Next,
                 #Pipe1.DONE : task.Next,
                 'GO' : state.Branch(dive.Start) }

    @staticmethod
    def getattr():
        return set(task.Task.getattr())

    def enter(self):
        
        self.stateMachine.start(state.Branch(dive.Start))
        # test
        #self.timer = self.timerManager.newTimer(Pipe1.DONE, 5)
        #self.timer.start()
        
    def exit(self):
        task.Task.exit(self)

        self.exited = True
        if (self.stateMachine.branches.has_key(dive.Start)):
            self.stateMachine.stopBranch(dive.Start)

class NextPipe1(task.Task):

    # test timer
    #DONE = core.declareEventType('DONE')
    @staticmethod
    def _transitions():
        return { nextPipe.COMPLETE : task.Next,
                 #NextPipe1.DONE : task.Next,
                 'GO' : state.Branch(nextPipe.Start) }

    @staticmethod
    def getattr():
        return set(task.Task.getattr())

    def enter(self):
        
        self.stateMachine.start(state.Branch(nextPipe.Start))
        # test
        #self.timer = self.timerManager.newTimer(NextPipe1.DONE, 5)
        #self.timer.start()
        
    def exit(self):
        task.Task.exit(self)

        self.exited = True
        if (self.stateMachine.branches.has_key(nextPipe.Start)):
            self.stateMachine.stopBranch(nextPipe.Start)

class Pipe2(task.Task):
  
    # test timer
    #DONE = core.declareEventType('DONE')
    @staticmethod
    def _transitions():
        return { pipe.COMPLETE : task.Next,
                 #Pipe2.DONE : task.Next,
                 'GO' : state.Branch(pipe.Start) }

    @staticmethod
    def getattr():
        return set(task.Task.getattr())

    def enter(self):
        
        self.stateMachine.start(state.Branch(pipe.Start))
        # test
        #self.timer = self.timerManager.newTimer(Pipe2.DONE, 5)
        #self.timer.start()
        
    def exit(self):
        task.Task.exit(self)

        self.exited = True
        if (self.stateMachine.branches.has_key(pipe.Start)):
            self.stateMachine.stopBranch(pipe.Start)


class transTar(task.Task):

# test timer
    #DONE = core.declareEventType('DONE')
    @staticmethod
    def _transitions():
        return { tran.COMPLETE : task.Next,
                 #NextPipe4.DONE : task.Next,
                 'GO' : state.Branch(tran.Start) }

    @staticmethod
    def getattr():
        return set(task.Task.getattr())

    def enter(self):
        
        self.stateMachine.start(state.Branch(tran.Start))
        # test
        #self.timer = self.timerManager.newTimer(transTar.DONE, 5)
        #self.timer.start()
        
    def exit(self):
        task.Task.exit(self)

        self.exited = True
        if (self.stateMachine.branches.has_key(tran.Start)):
            self.stateMachine.stopBranch(tran.Start)

class Target(task.Task):

    # test timer
    #DONE = core.declareEventType('DONE')
    @staticmethod
    def _transitions():
        return { window.COMPLETE : task.Task,
                 #Target.DONE : task.Next,
                 'GO' : state.Branch(window.Start) }

    @staticmethod
    def getattr():
        return set(task.Task.getattr())

    def enter(self):
        
        self.stateMachine.start(state.Branch(window.Start))
        # test
        #self.timer = self.timerManager.newTimer(Target.DONE, 5)
        #self.timer.start()
      
    def exit(self):
        task.Task.exit(self)

        self.exited = True
        if (self.stateMachine.branches.has_key(window.Start)):
            self.stateMachine.stopBranch(window.Start)


