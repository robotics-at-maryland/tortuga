import ram.ai.new.utilClasses as utilClasses
import ram.ai.new.utilStates as utilStates
import ram.ai.new.motionStates as motion
import ram.ai.new.searchPatterns as searches
import ram.ai.new.stateMachine as stateMachine
import ram.ai.new.state as state
import ram.ai.new.approach as approach


class SonarManipulation(stateMachine.StateMachine):
    def __init__(self, pogoHeight, pogoWidth, pogoShift, siteSearchDistance,boxX, boxY):
        
        start = self.addState('start', utilStates.Start())
        endS = self.addState('endSuccess',utilStates.End())
        endF = self.addState('endFailure',utilStates.End())
        #perform the initial pipe search(square)
        boxSearch = self.addState('boxSearch',search.BoxSearchPattern(boxX,boxY, pipe.isSeen(), 'pipeCenter','endFailure'))
        center = self.addState('pipeCenter', 
                              approach.DownCenter(pipe, 'alignPipe', 'endFailure'))
        align = self.addState('alignPipe', 
                              approach.DownOrient(pipe, 'siteSearch', 'endFailure'))
        pipeSearch = self.addState( searches.ForwardsSearchPattern(
            siteSearchDistance, 
            sampleSite.isSeen,
            'centerSite', 
            'endFailure'))
      center = self.addState('centerSite', 
                              approach.DownCenter(sampleSite, 'alignSite', 'endFailure'))    
      align = self.addState('alignSite', 
                              approach.DownOrient(sampleSite, 'pogoTime', 'endFailure'))


#this state machine performs the "pogo" motion which consists of the robot moving up and down in a square over an area
#it contains an cheap trick for telling if the robot has already hit the site by checking if the depth reading hasn't changed by a specified margin for a brief time(like a second).  This speeds up the manuver, avoids hangups, and reduces the chance of knocking over the task(within reason)
class PogoMotion(stateMachine.StateMachine):
    def __init__(pogoHeight, pogoWidth, pogoShift):
        super(PogoMotion, self).__init__()
        start = self.addState('start',utilStates.Start())
        end = self.addState('end', utilStates.End())
        translate = self.addState('translate', motion.Move(pogoHeight/2, pogoWidth/2))
        translate.setTransition('next', translate) 
        #begin the pogoing
        #this occurs in another state machine for the depth motion
        
#this state machine takes in a state INSTANCE
class MonoStateMachine(stateMachine.StateMachine):
    def __init__(self, inState):
        start = self.addState('start',utilStates.Start())
        end = self.addState('end', utilStates.End())
        start.setTransition('next', inState)
    

class DownStopIfHit(utilStates.ConstrainedState):
    def __init__(self, depth):
        super(DownStopIfHit,self).__init__(MonoStateMachine(motion.Dive(depth)),depthStopCondition)
