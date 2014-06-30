import motionStates
import utilStates
import stateMachine

#utility function used in this module for default parameters
def alwaysTrue():
    return true

#search patterns require a search machine which performs the motions associated with searching, and a function which returns true if the search has failed
#success and failure are the destination state ids for the success and failure cases
#if not present, the failure case will also map to next
#unlike constrainedState, SearchPattern has two parameters for stopping
#the first is the success conditions, the second are the termination conditions
#note that search patterns must ALWAYS have a failure case, if you always wan't to go to the same state then make success and failure both the same state ID
class SearchPattern(ConstrainedState):
    def __init__(self,searchMachine,stopConditions,constraint = alwaysTrue(),success,failure):
        #a search pattern only succeeds if its stop condition is met, it always if the search pattern finishes, or if a constraint is met
        super(SearchPattern,self).__init__(searchMachine,constraint,failure,failure)
        self.setNextState(success)
        self._stopConditions = stopConditions
        
    def update(self):
        if(self._stopConditions()):
            self.doTransition('next')
        else:
            super(SearchPattern, self).update()


#A simplistic search machine, it merely moves forwards a prespecified distance
class ForwardsSearchMachine(stateMachine.stateMachine):
    def __init__(self,distance):
        super(self,ForwardsSearchMachine).__init__()
        start = self.addState('start',utilStates.Start())
        end = self.addState('end',utilStates.End())
        move = self.addState('move',motionStates.Forward(distance))
        start.setNextState('move')
        move.setNextState('end')
        
class ForwardsSearchPattern(SearchPattern):
    def __init__(self,stopConditions, constraint = alwaysTrue(), success, failure):
        super(self,ForwardsSearchPattern).__init__(ForwardsSearchMachine(),stopConditions,success,failure)
        
