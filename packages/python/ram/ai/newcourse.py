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


# pipe after gate
class Gatepipe(task.Task):
  
    # test
    #DONE = core.declareEventType('DONE')
    @staticmethod
    def _transitions():
        return { pipe.COMPLETE : Skipbuoy,
                 #Gatepipe.DONE : task.Next,
                 'GO' : state.Branch(pipe.Start) }

    @staticmethod
    def getattr():
        return set(task.Task.getattr())

    def enter(self):
        
        self.stateMachine.start(state.Branch(pipe.Start))
        # test
        #self.timer = self.timerManager.newTimer(Gatepipe.DONE, 5)
        #self.timer.start()
        
    def exit(self):
        task.Task.exit(self)

        self.exited = True
        if (self.stateMachine.branches.has_key(pipe.Start)):
            self.stateMachine.stopBranch(pipe.Start)

# skip buoys
class Skipbuoy(task.Task):
  
    # test
    #DONE = core.declareEventType('DONE')
    @staticmethod
    def _transitions():
        return { skip.COMPLETE : Buoypipe,
                 #Skipbuoy.DONE : task.Next,
                 'GO' : state.Branch(skip.Start) }

    @staticmethod
    def getattr():
        return set(task.Task.getattr())

    def enter(self):
        
        self.stateMachine.start(state.Branch(skip.Start))
        # test
        #self.timer = self.timerManager.newTimer(Skipbuoy.DONE, 5)
        #self.timer.start()
        
    def exit(self):
        task.Task.exit(self)

        self.exited = True
        if (self.stateMachine.branches.has_key(skip.Start)):
            self.stateMachine.stopBranch(skip.Start)

#buoys

# --- nope --- #

#pipe after buoys
class Buoypipe(task.Task):
  
    # test
    #DONE = core.declareEventType('DONE')
    @staticmethod
    def _transitions():
        return { pipe.COMPLETE : Upright,
                 #Buoypipe.DONE : task.Next,
                 'GO' : state.Branch(pipe.Start) }

    @staticmethod
    def getattr():
        return set(task.Task.getattr())

    def enter(self):
        
        self.stateMachine.start(state.Branch(pipe.Start))
        # test
        #self.timer = self.timerManager.newTimer(Buoypipe.DONE, 5)
        #self.timer.start()
        
    def exit(self):
        task.Task.exit(self)

        self.exited = True
        if (self.stateMachine.branches.has_key(pipe.Start)):
            self.stateMachine.stopBranch(pipe.Start)

#skip uprights
class Uprightskip(task.Task):
  
    # test
    #DONE = core.declareEventType('DONE')
    @staticmethod
    def _transitions():
        return { skip.COMPLETE : Pipe1,
                 #Uprightskip.DONE : task.Next,
                 'GO' : state.Branch(skip.Start) }

    @staticmethod
    def getattr():
        return set(task.Task.getattr())

    def enter(self):
        
        self.stateMachine.start(state.Branch(gate.Start))
        # test
        #self.timer = self.timerManager.newTimer(Uprightskip.DONE, 5)
        #self.timer.start()
        
    def exit(self):
        task.Task.exit(self)

        self.exited = True
        if (self.stateMachine.branches.has_key(skip.Start)):
            self.stateMachine.stopBranch(skip.Start)

# uprights

#pipe after uprights
class Uprightpipe(task.Task):
  
    # test
    #DONE = core.declareEventType('DONE')
    @staticmethod
    def _transitions():
        return { twopipe.COMPLETE : Pipe1,
                 #Uprightpipe.DONE : task.Next,
                 'GO' : state.Branch(twopipe.Start) }

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

# skip bins
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

# bins


# pipe after bins
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

# skip torps
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

# torps
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

# pipe after torps
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










