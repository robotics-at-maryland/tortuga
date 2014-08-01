import ram.motion as motion
import ext.estimation as estimation
import ext.math as math
import ram.ai.new.utilStates as utilStates
import ram.ai.new.stateMachine as stateMachine
import ram.ai.Utility as oldUtil
import ram.ai.new.searchPatterns as search
import ram.ai.new.state as state
import ram.ai.new.utilClasses as util
from ext.control import yawVehicleHelper
import math as m

class AcousticServoing(state.State):
    def __init__(self, sonarObject, destinationVector, minVx, minVy):
        super(AcousticServoing,self).__init__()
        self._sonarObject = sonarObject
        self._destinationVector = destinationVector
        self.kx = .030
        self.ky = -.030
        self.kz = 0
        self.minVx = minVx
        self.minVy = minVy
 
    def update(self):
        self._sonarObject.update()
        self.runMotion(self._sonarObject)

    def runMotion(self,event):
        new_x = self.decideX(self.kx*(event.x - self._destinationVector.x))
        new_y = self.decideY(self.ky*(event.y - self._destinationVector.y))
        new_z = self.kz*(event.z - self._destinationVector.z)
        self.getStateMachine().getLegacyState().motionManager._controller.moveVel(new_x,new_y,new_z)

    def decideX(self, x_velocity):
        if(abs(x_velocity) < self.minVx):
            return m.copysign(self.minVx, x_velocity)
        else:
            return x_velocity
        
    def decideY(self, y_velocity):
        if(abs(y_velocity) < self.minVy):
            return m.copysign(self.minVy, y_velocity)
        else:
            return y_velocity
