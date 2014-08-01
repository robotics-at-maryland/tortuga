import ram.ai.new.utilClasses as utilClasses
import ram.ai.new.utilStates as utilStates
import ram.ai.new.motionStates as motion
import ram.ai.new.searchPatterns as searches
import ram.ai.new.stateMachine as stateMachine
import ram.ai.new.state as state
import ram.ai.new.approach as approach

import time


class SonarManipulation(stateMachine.StateMachine):
    def __init__(self, siteSearchDistance,boxX, boxY):
        super(SonarManipulation,self).__init__()
        pipe = utilClasses.PipeVisionObject(self.getLegacyState())
        self.sampleSite = utilClasses.PipeVisionObject(self.getLegacyState())
        start = self.addState('start', utilStates.Start())
        start.setEnterCallback('next', self.turnPipeOn)
        start.setTransition('next', 'boxSearch')
        endS = self.addState('endSuccess',utilStates.End())
        endF = self.addState('endFailure',utilStates.End())
        #perform the initial pipe search(square)
        boxSearch = self.addState('boxSearch',searches.BoxSearchPattern(boxX,boxY, pipe.isSeen, 'pipeCenter','endFailure'))
        center = self.addState('pipeCenter', approach.DownCenter(pipe, 'alignPipe', 'endFailure'))
        align = self.addState('alignPipe', approach.DownOrient(pipe, 'siteSearch', 'endFailure'))
        align.setLeaveCallback('next', self.turnPipeOff)
        align.setLeaveCallback('failure', self.turnPipeOff)
        pipeSearch = self.addState('siteSearch',searches.ForwardsSearchPattern(siteSearchDistance, self.sampleSite.isSeen, 'centerSite',  'endFailure'))
        pipeSearch.setEnterCallback('next',self.turnSiteOn)
        center = self.addState('centerSite', approach.DownCenter(self.sampleSite, 'alignSite', 'endFailure'))    
        align = self.addState('alignSite', approach.DownOrient(self.sampleSite, 'endSuccess', 'endFailure'))
        
    def turnPipeOn(self):
        self.getLegacyState().visionSystem.pipeLineDetectorOn()        
    def turnPipeOff(self):
        self.getLegacyState().visionSystem.pipeLineDetectorOff()
    def turnSiteOn(self):
        self.sampleSite.seen = False#makes sure site isn't in view
        self.getLegacyState().visionSystem.downwardSafeDetectorOn()
    def turnSiteOff(self):
        self.getLegacyState().visionSystem.downwardSafeDetectorOff()
       



            
        
#this state machine performs the "pogo" motion which consists of the robot moving up and down in a square over an area
#it contains an cheap trick for telling if the robot has already hit the site by checking if the depth reading hasn't changed by a specified margin for a brief time(like a second).  This speeds up the manuver, avoids hangups, and reduces the chance of knocking over the task(within reason)
class PogoMotion(stateMachine.StateMachine):
    def __init__(self,pogoHeight, pogoWidth, pogoShift, pogoShiftV,attackHeight):
        super(PogoMotion, self).__init__()
        start = self.addState('start',utilStates.Start())
        start.setTransition('next', 'taskDepth')
        taskDepth = self.addState('taskDepth', motion.DiveTo(attackHeight,.3))
        taskDepth.setTransition('next', 'translate')
        end = self.addState('end', utilStates.End())
        translate = self.addState('translate', motion.Move(pogoHeight/2, pogoWidth/2))
        translate.setTransition('next', 'pogo') 
        #begin the pogoing
        #this occurs in another state machine for the depth motion
        shift = self.addState('shift', motion.Strafe(pogoShift,.5))
        shift.setTransition('next', 'pogo')
        pogo = self.addState('pogo', DownStopIfHit(1.5, 10.0,'up'))
        up = self.addState('up', motion.DiveTo(attackHeight,.5))
        up.setTransition('next','countSide')
        countSide = self.addState('countSide', utilStates.PassCounter('countSwitch'))
        countSwitch = self.addState('countSwitch', utilStates.Switch('shiftSwitch', 'vShift', countSide.getPassChecker(pogoWidth/pogoShift)))
        vShift = self.addState('vShift',motion.Forward(pogoShiftV,.5))
        vShift.setTransition('next', 'countvShift')
        countvShift = self.addState('countvShift', utilStates.PassCounter('vShiftSwitch'))
        #need to do magical reset somehow, need to figure out
        #probably using exit transition
        vShiftSwitch = self.addState('vShiftSwitch', utilStates.Switch('pogo', 'end', countvShift.getPassChecker(pogoHeight/pogoShiftV)))
        rShift = self.addState('rShift', motion.Strafe(-pogoShift))
        shiftSwitch = self.addState('shiftSwitch',utilStates.Switch('rShift','shift', lambda: (countvShift.getPasses()%2 == 0)))
        rShift.setTransition('next', 'pogo')
        vShiftSwitch.setLeaveCallback('next', countSide.reset)
        
        
        
#this state machine takes in a state INSTANCE
class MonoStateMachine(stateMachine.StateMachine):
    def __init__(self, inState):
        super(MonoStateMachine,self).__init__()
        start = self.addState('start',utilStates.Start())
        self.addState('state',inState)
        inState.setTransition('next', 'end')
        end = self.addState('end', utilStates.End())
        start.setTransition('next', 'state')
   

class DepthStopCondition(object):
    def __init__(self, stateptr, minVel,tm):
        self._stateptr = stateptr
        self._minVel = minVel
        self.check = utilClasses.hasQueryBeenFalse(tm, self.query).query
        
    def query(self):
        return abs(self._stateptr.getStateMachine().getLegacyState().stateEstimator.getEstimatedDepthRate())>self._minVel
         

class DownStopIfHit(utilStates.ConstrainedState):
    def __init__(self, depth,tm,success):
        self.tim = utilClasses.Timer(tm)
        super(DownStopIfHit,self).__init__(MonoStateMachine(motion.Dive(depth,.5)),self.tim.check,success)

    def enter(self):
        super(DownStopIfHit, self).enter()
        self.tim.reset()
        
#DepthStopCondition(self,.025 ,tm).check,success)
    
class PogoTask(utilStates.Task):
    def __init__(self, pogoHeight, pogoWidth, pogoShift, pogoShiftV,attackHeight,success= 'end', failure = 'end', duration = 30000):
        super(PogoTask,self).__init__(PogoMotion(pogoHeight, pogoWidth, pogoShift, pogoShiftV,attackHeight),success, failure, duration) 

    def update(self):
        super(PogoTask,self).update()
        if self.getInnerStateMachine().getCurrentState().getName() == 'end':
            self.doTransition('success')

class SonarManipTask(utilStates.Task):
    def __init__(self, siteSearchDistance,boxX, boxY, success= 'end', failure = 'end', duration = 30000):
        super(SonarManipTask,self).__init__(SonarManipulation(siteSearchDistance,boxX, boxY), success, failure, duration)
        
    def update(self):
        super(SonarManipTask,self).update()
        if self.getInnerStateMachine().getCurrentState().getName() == 'endFailure':
            self.doTransition('failure')
        elif self.getInnerStateMachine().getCurrentState().getName() == 'endSuccess':
            self.doTransition('success')

class ManipRelease(state.State):
    def __init__(self, release = 0, wait = 7):
        super(ManipRelease, self).__init__()
        self._release = release
        self._start = time.time()

    def enter(self):
        self.getStateMachine().getLegacyState().vehicle.releaseGrabber(self._release)
        super(ManipRelease, self).enter()
        

    def update(self):
        if time.time() - self._start >= 7:
            self.doTransition('next')
