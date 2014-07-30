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

import ram.ai.new.mainState as mainState

# ----- Here are Task Imports
import ram.ai.gate as gate
import ram.ai.GoodPipe as pipe
import ram.ai.Skip as nextPipe
import ram.ai.transToTarget as tran
import ram.ai.window13final as window
import ram.ai.GenericDiveStart as dive
# ----- End of Task Imports



class GateTask(task.Task):
  
    # test timer
    #DONE = core.declareEventType('DONE')
    @staticmethod
    def _transitions():
        return { gate.COMPLETE : task.Next,
                 'GO' : state.Branch(gate.Start) }

    @staticmethod
    def getattr():
        return set(task.Task.getattr())

    def enter(self):
        
        self.stateMachine.start(state.Branch(gate.Start))
        
    def exit(self):
        task.Task.exit(self)

        self.exited = True
        if (self.stateMachine.branches.has_key(gate.Start)):
            self.stateMachine.stopBranch(gate.Start)



