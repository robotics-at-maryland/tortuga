import ram.ai.new.utilClasses as utilClasses
import ram.ai.new.utilStates as utilStates
import ram.ai.new.motionStates as motion
import ram.ai.new.searchPatterns as searches
import ram.ai.new.stateMachine as stateMachine
import ram.ai.new.state as state
import ram.ai.new.approach as approach

class UprightsTask(utilStates.Task):
    def __init__(self, taskDepth, returnDepth, uprightsSearchDistance,
                 endRange, rectWidth, rectLength, 
                 success, failure, duration = 300):
        super(UprightsTask, self).__init__(stateMachine.StateMachine(),
                                           success, failure,
                                           duration)
        

        passcounter = utilStates.PassCounter('switch')
        self.getInnerStateMachine().addStates({
                'start' : utilStates.Start(),
                'end' : utilStates.End(),
                'dive' : motion.DiveTo(taskDepth),
                'uprightsSearch' : FindUprights(uprightsSearchDistance, 
                                                utilClasses.hasQueryBeenFalse(1, uprights.isSeen).query, 
                                                'approach',
                                                'raise'),
                'approach' : UprightsApproach(uprights, 
                                             'squareBuffer', 
                                             'raiseBuffer', endRange),
                'squareBuffer' : motion.Forward(0),
                'startSquare' : motion.Strafe(-rectWidth*0.5, 0.3),
                'moveForward' : motion.Forward(rectLength, 0.25),
                'moveRight' : motion.Strafe(rectWidth*0.8, 0.3),
                'moveBack' : motion.Forward(-rectLength, 0.25),
                'moveLeft' : motion.Strafe(-rectWidth*0.5, 0.3),
                'raiseBuffer' : motion.Forward(0),
                'raise' : motion.DiveTo(returnDepth),
                'findPipe' : FindPipe(pipeSearchDistance,
                                      pipe.isSeen,
                                      'align',
                                      'pipeFail'),
                'pipeFail' : utilStates.Failure(),
                'moveOver' : motion.Forward(3)
                })

        self.getInnerStateMachine().addTransitions(
            ('start', 'next', 'dive'),
            ('dive', 'next', 'uprightsSearch'),
            ('squareBuffer', 'next', 'startSquare'),
            ('startSquare', 'next', 'moveForward'),
            ('moveForward', 'next', 'moveRight'),
            ('moveRight', 'next', 'moveBack'),
            ('moveBack', 'next', 'moveLeft'),
            ('moveLeft', 'next', 'raise'),
            ('raiseBuffer', 'next', 'raise'),
            ('raise', 'next', 'moveOver'))

        def enter(self):
            self.getStateMachine().getLegacyState().visionSystem\
                .forwardPipeDetectorOn()
            super(UprightsTask, self).enter()

        def leave(self):
            self.getStateMachine().getLegacyState().visionSystem\
                .forwardPipeDetectorOff()
            super(UprightsTask, self).enter()

        def update(self):
            if self._InnerMachine().isCompleted() and \
                    isinstance(self.getInnerStateMachine().getCurrentState(), 
                               utilStates.Failure):
                    self.doTransition(failure)
            else:
                super(UprightsTask, self).update()

class FindUprights(searches.ForwardsSearchPattern):
    def __init__(self, searchDistance, query, success, failure):
        super(FindUprights, self).__init__(searchDistance, query, success, failure)
        self._leaveCallBacks = { failure : self.turnOff }

    def enter(self):
        self.getStateMachine().getLegacyState().visionSystem.buoyDetectorOn()
        super(FindUprights, self).enter()

    def turnOff(self):
        self.getStateMachine().getLegacyState().visionSystem.buoyDetectorOff()
        super(FindUprights, self).leave()


class UprightsApproach(approach.ForwardsCenter):
    def __init__(self, visionObject, success, failure, rangeValue):
        super(UprightsApproach, self).__init__(visionObject, success, 
                                               failure, rangeValue)

    def leave(self):
        self.getStateMachine().getLegacyState().visionSystem.buoyDetectorOff()
        super(UprightsApproach, self).leave()



