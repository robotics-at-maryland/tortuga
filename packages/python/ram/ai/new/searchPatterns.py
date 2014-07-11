import ram.ai.new.motionStates as motion
import ram.ai.new.utilStates as utilStates
import ram.ai.new.stateMachine as stateMachine
import ram.ai.Utility as oldUtil

#search patterns require a search machine which performs the motions associated
#with searching, and a function which returns true if the search has failed
#success and failure are the destination state ids for the success and failure
#cases if not present, the failure case will also map to next unlike
#constrainedState, SearchPattern has two parameters for stopping
#the first is the success conditions, the second are the termination conditions
#note that search patterns must ALWAYS have a failure case, if you always wan't
# to go to the same state then make success and failure both the same state ID
class SearchPattern(utilStates.ConstrainedState):
    def __init__(self, searchMachine, stopConditions, success, failure,
                 constraint = lambda: True):
        #a search pattern only sucforwardceeds if its stop condition is met, it always if the search pattern finishes, or if a constraint is met
        super(SearchPattern,self).__init__(searchMachine, constraint, failure,
                                           failure)
        self.setTransition('next', success)
        self._stopConditions = stopConditions
        
    def update(self):
        if(self._stopConditions()):
            self.doTransition('next')
        else:
            super(SearchPattern, self).update()
    
    def leave(self):
        oldUtil.freeze(self.getStateMachine().getLegacyState())


#A simplistic search machine, it merely moves forwards a prespecified distance
class ForwardsSearchMachine(stateMachine.StateMachine):
    def __init__(self,distance):
        super(ForwardsSearchMachine,self).__init__()
        start = self.addState('start',utilStates.Start())
        end = self.addState('end',utilStates.End())
        move = self.addState('move',motion.Forward(distance))
        start.setTransition('next', 'move')
        move.setTransition('next', 'end')
        
class ForwardsSearchPattern(SearchPattern):
    def __init__(self,searchDistance, stopConditions, success, failure,
                 constraint = lambda: True):
        super(ForwardsSearchPattern,self).__init__(
            ForwardsSearchMachine(searchDistance),
            stopConditions, success, failure, constraint)

class StrafeSearchMachine(stateMachine.StateMachine):
    def __init__(self,distance):
        super(StrafeSearchMachine,self).__init__()
        start = self.addState('start',utilStates.Start())
        end = self.addState('end',utilStates.End())
        move = self.addState('move',motion.Strafe(distance))
        start.setTransition('next', 'move')
        move.setTransition('next', 'end')
        
class StrafeSearchPattern(SearchPattern):
    def __init__(self,searchDistance, stopConditions, success, failure,
                 constraint = lambda: True):
        super(StrafeSearchPattern,self).__init__(
            StrafeSearchMachine(searchDistance),
            stopConditions, success, failure, constraint)


# Search in a rectangle in the plane of the robot orthogonal to depth
class BoxSearchMachine(stateMachine.StateMachine):
    def __init__(self, X, Y):
        super(BoxSearchMachine, self).__init__()
        start = self.addState('start', utilStates.Start())
        end = self.addState('end', utilStates.End())
        transXOut = self.addState('transXOut', motion.Forward(X))
        transXIn = self.addState('transXIn', motion.Forward(-X))
        strafeOut = self.addState('strafeOut', motion.Strafe(-Y))
        strafeIn = self.addState('strafeIn', motion.Strafe(Y))
        start.setTransition('next', 'transXOut')
        transXOut.setTransition('next', 'strafeOut')
        strafeOut.setTransition('next', 'transXIn')
        transXIn.setTransition('next', 'strafeIn')
        strafeIn.setTransition('next', 'end')

class BoxSearchPattern(SearchPattern):
    def __init__(self, xDistance, yDistance, stopConditions, success, failure,
                 constraint = lambda: True):
        super(BoxSearchPattern,self).__init__(
            BoxSearchMachine(xDistance, yDistance),
            stopConditions, success, failure, constraint)

# Maintaining position yaw the robot to search
class YawSearchMachine(stateMachine.StateMachine):
    def __init__(self, yawAngle):
        super(YawSearchMachine, self).__init__()
        start = self.addState('start', utilStates.Start())
        end = self.addState('end', utilStates.End())
        yaw = self.addState('yaw', motion.Turn(yawAngle))
        start.setTransition('next', 'yaw')
        yaw.setTransition('next', 'end')
        
        
class YawSearchPattern(SearchPattern):
    def __init__(self, yawAngle, stopConditions, success, failure,
                 constraint = lambda: True):
        super(YawSearchPattern,self).__init__(YawSearchMachine(yawAngle),
                                              stopConditions, success,
                                              failure, constraint)
        
# zigzag search
class ZigZagSearchMachine(stateMachine.StateMachine):
    def __init__(self, travelDistance, strafeDistance):
        super(ZigZagSearchMachine, self).__init__()
        start = self.addState('start', utilStates.Start())
        end = self.addState('end', utilStates.End())
        # starting half strafe
        startingStrafe = self.addState('startingStrafe',
                                       motion.Strafe(strafeDistance/2))
        move1 = self.addState('move1', motion.Forward(travelDistance))
        move2 = self.addState('move2', motion.Forward(travelDistance))
        strafeIn = self.addState('strafeIn', motion.Strafe(-strafeDistance))
        strafeOut = self.addState('strafeOut', motion.Strafe(strafeDistance))
        passCount = self.addState('passCount', utilStates.PassCounter('move1'))

        start.setTransition('next', 'startingStrafe')
        startingStrafe.setTransition('next', 'move1')
        move1.setTransition('next', 'strafeIn')
        strafeIn.setTransition('next', 'move2')
        move2.setTransition('next', 'strafeOut')
        strafeOut.setTransition('next', 'passCount')
        passCount.setTransition('next', 'move1')

    def getPasses(self):
        return self.getState('passCount').getPasses()
        

class ZigZagSearchPattern(SearchPattern):
    def __init__(self, travelDistance, strafeDistance, stopConditions, success, failure,
                 constraint = lambda: True):
        super(ZigZagSearchPattern, self).__init__(
            ZigZagSearchMachine(travelDistance, strafeDistance),
            stopConditions, success, failure, constraint)
        
    
