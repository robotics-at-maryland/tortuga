import ram.ai.new.utilClasses as utilClasses
import ram.ai.new.utilStates as utilStates
import ram.ai.new.motionStates as motion
import ram.ai.new.searchPatterns as searches
import ram.ai.new.stateMachine as stateMachine
import ram.ai.new.state as state
import ram.ai.new.approach as approach


class TorpedoTask(utilStates.Task):
    def __init__(self, torps, initialDepth,yawSearchBound ,forwardsSearchIncrement, maxSearchAttempts, squareCenteringRange, squareCenteringRangeBound, squareCenteringXYBound, holeSearchDist, holeCenteringRange, holeCenteringRangeBound, holeCenteringXYBound,reverseRefindDistance,success, failure, duration = 300)
    super(TorpedoTask,self).__init(TorpedoTaskMachine(torps,initialDepth, yawSearchBound ,forwardsSearchIncrement, maxSearchAttempts, holeSearchDist, squareCenteringRange, squareCenteringRangeBound, squareCenteringXYBound, holeCenteringRange, holeCenteringRangeBound, holeCenteringXYBound,reverseRefindDistance),success, failure, duration) 

    def update():
        super(TorpedoTask,self).update()
        if self.getInnerStateMachine().getCurrentState().getName() == 'taskFailed':
            self.doTransition('failure')
        elif self.getInnerStateMachine().getCurrentState().getName() == 'taskSuccess':
            self.doTransition('success')
            

class FireLeft(state.State):
    def __init__(self, next):
        super(FireLeft, self).__init__()
        self.setTransition('next', next)

    def update():
        self.doTransition('next')
        pass#put firing code here



class FireRight(state.State):
    def __init__(self, next):
        super(FireRight, self).__init__()
        self.setTransition('next', next)

    def update():
        self.doTransition('next')
        pass#put firing code here


class TorpedoMachine(stateMachine.StateMachine):
    def __init__(torps,initialDepth, yawSearchBound ,forwardsSearchIncrement, maxSearchAttempts, holeSearchDist, squareCenteringRange, squareCenteringRangeBound, squareCenteringXYBound, holeCenteringRange, holeCenteringRangeBound, holeCenteringXYBound,reverseRefindDistance)
    super(TorpedoMachine).__init__()
    #other hole option is to shoot through the large hole, torps.large
    hole1 = torps.left
    hole2 = torps.right
    pipe = BLAH BLAH LOAD A PIPE
    start = self.addState('start',utilStates.Start())
    endS = self.addState('taskSuccess',utilStates.End())
    endF = self.addState('taskFailure',utilState.End())
    #box finding search
    initialMove = self.addState('initialMove', motion.Turn(-yawSearchBound/2))
    initialMove.setTransition('next', 'initialSearch')
    initialSearch = self.addState('initialSearch', searches.YawSearchPattern(yawSearchBound, torps.box.isSeen, 'boxCentering', 'prepareForwards'))
    prepareForwards =  self.addState('prepareForwards', motion.Turn(-yawSearchBound/2))
    prepareForwards.setTransition('next', 'forwardsSearch')
    forwardsSearch = self.addState('forwardsSearch', search.ForwardsSearchPattern(forwardsSearchIncrement, torps.box.isSeen, 'boxCentering', 'counter'))
    counter = self.addState('counter', utilStates.PassCounter('passSwitch'))
    passSwitch = self.addState('passSwitch', utilStates.Switch('initialMove', 'taskFailure', counter.getPassChecker(maxSearchAttempts)))
    #aligning with box, if that fails, try to search again
    boxCentering = self.addState('boxCentering', approach.ForwardsCenter(torps.box, 'holePassCount', 'initialMove', squareCenteringRange, squareCenteringXYBound,squareCenteringXYBound,squareCenteringRangeBound))
    holePassCount = self.addState('holePassCount', utilStates.PassCount('holeSwitch'))
    holeSwitch = self.addState('holeSwitch', utilStates.Switch('hole1Search','hole2search',holePassCount.getPassChecker(2)))
    hole1Search = self.addState('hole1Search', search.ForwardsSearchPattern(holeSearchDist, hole1.isSeen,'hole1Center', 'backUp'))
    hole1Center = self.addState('hole1Center', approach.ForwardsCenter(hole1, 'fireLeft', 'backUp', holeCenteringRange, holeCenteringXYBound, holeCenteringXYBound, holeCenteringRangeBound))
    fireLeft = self.addState('fireLeft', FireLeft('backUp'))
    backUp = self.addState('backUp', motion.Forwards(-reverseRefindDistance))
    backUp.setTransition('next', 'holePassCount')
    hole2Search = self.addState('hole2Search', search.ForwardsSearchPattern(holeSearchDist, hole1.isSeen,'hole2Center', 'backUp'))
    hole2Center = self.addState('hole2Center', approach.ForwardsCenter(hole1, 'fireRight', 'backUp', holeCenteringRange, holeCenteringXYBound, holeCenteringXYBound, holeCenteringRangeBound))
    fireRight = self.addState('fireRight', FireRight('moveAway'))
    #Now that manipulator is dropped, this probably just needs to move to the side to prepare for the sonar task
    

    
