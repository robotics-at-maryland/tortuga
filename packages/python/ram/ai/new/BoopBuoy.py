import ram.ai.new.utilClasses as utilClasses
import ram.ai.new.utilStates as utilStates
import ram.ai.new.motionStates as motionStates
import ram.ai.new.searchPatterns as searches
import ram.ai.new.approach as approach

import ram.ai.new.stateMachine as stateMachine

class BoopBuoyTask(utilStates.Task):
    def __init__(self, searchDistance, boopDepth, endDepth, success, failure, duration = 120):
        super(BoopBuoyTask, self).__init__(BoopMachine(), 
                                           success, failure, duration)
        pipe = utilClasses.PipeVisionObject(self.getInnerStateMachine().getLegacyState())
        self.getInnerStateMachine().addStates({
                'start' : utilStates.Start(),
                'end' : utilStates.End(),
                'fail' : Fail(),
                'search' : searches.ForwardsSearchPattern(searchDistance,
                                                          pipe.isSeen, 
                                                          'center', 'fail'),
                'center' : approach.DownCenter(pipe, 'buf1', 'fail'),
                'buf1' : motionStates.Forward(0),
                'taskDive' : motionStates.DiveTo(boopDepth),
                'rise' : motionStates.DiveTo(endDepth)})

        self.getInnerStateMachine().addTransitions(
            ('start', 'next', 'search'),
            ('buf1', 'next', 'taskDive'),
            ('taskDive', 'next', 'rise'),
            ('rise', 'next', 'end'))

    def enter(self):
        self.getInnerStateMachine().getLegacyState().visionSystem.pipeLineDetectorOn()
        print "I'mma boop it"
        super(BoopBuoyTask, self).enter()

    def update(self):
        if isinstance(self.getInnerStateMachine().getCurrentState(), Fail):
            self.doTransition('failure')
        super(BoopBuoyTask, self).update()

    def leave(self):
        self.getInnerStateMachine().getLegacyState().visionSystem.pipeLineDetectorOff()
        print "I booped it!"
        super(BoopBuoyTask, self).leave()


class BoopMachine(stateMachine.StateMachine):
    pass

class Fail(utilStates.End):
    pass
