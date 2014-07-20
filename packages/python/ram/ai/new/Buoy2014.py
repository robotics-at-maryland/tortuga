import ram.ai.new.utilClasses as utilClasses
import ram.ai.new.utilStates as utilStates
import ram.ai.new.motionStates as motion
import ram.ai.new.searchPatterns as searches
import ram.ai.new.approach as approach

from ram.ai.new.state import *
from ram.ai.new.stateMachine import *

@require_transitions('success', 'failure')
class BuoyTask(utilStates.Task):
    def __init__(self, buoysVisionObject, success, failure, duration = 300):
        super(BuoyTask, self).__init__(StateMachine(), success, failure, duration)

        self.getInnerStateMachine().addStates({
            'start'             : utilStates.Start(),
            'initialDive'       : motion.Dive(),
            'initialSearch'     : searches.ForwardsSearchPattern(self.forwardsSearchDist, 
                                                                 buoysVisionObject.isSeen, 
                                                                 'redBuoySearch', 'failure'),
            'redBuoySearch'     : BuoySearchState(buoysVisionObject.red, 
                                                  self.buoySearchDist,
                                                  self.backUpDist),
            'greenBuoySearch'   : BuoySearchState(buoysVisionObject.green, 
                                                  self.buoySearchDist,
                                                  self.backUpDist),
            'cyclingBuoySearch' : BuoySearchState(buoysVisionObject.cycling, 
                                                  self.buoySearchDist,
                                                  self.backUpDist),
            'failure'           : utilStates.End(),
            'success'           : utilStates.End()
            })

        self.getInnerStateMachine().addTransitions([
            ('start'            , 'next'    , 'initialDive'      ),
            ('initialDive'      , 'next'    , 'initialSearch'    ),
            ('initialSearch'    , 'success' , 'redBuoySearch'    ),
            ('initialSearch'    , 'failure' , 'failure'          ),
            ('redBuoySearch'    , 'success' , 'greenBuoySearch'  ),
            ('redBuoySearch'    , 'failure' , 'failure'          ),
            ('greenBuoySearch'  , 'success' , 'cyclingBuoySearch'),
            ('greenBuoySearch'  , 'failure' , 'failure'          ),
            ('cyclingBuoySearch', 'success' , 'success'          ),
            ('cyclingBuoySearch', 'failure' , 'failure'          )
            ])

    def update():
        super(BuoyTask,self).update()
        if self.getInnerStateMachine().getCurrentState().getName() == 'failure':
            self.doTransition('failure')
        elif self.getInnerStateMachine().getCurrentState().getName() == 'success':
            self.doTransition('success')

@require_transitions('complete', 'failure')
class BuoySearchState(utilStates.NestedState):
    def __init__(self, buoyVisionObject, searchDist, approachDist, bumpDist, backDist):
        super(BuoySearchState, self).__init__()

        self.getInnerStateMachine().addStates({
            'start'             : utilStates.Start(),
            'search'            : searches.StrafeSearchPattern(searchDist,
                                                               buoyVisionObject.isSeen, 
                                                               'approach', 'failure'),
            'approach'          : approach.ForwardsCenter(buoyVisionObject,
                                                          'bump', 'failure',
                                                          approachDist),
            'bump'              : motion.Forward(bumpDist),
            'back'              : motion.Forward(backDist),
            'failure'           : utilStates.End(),
            'success'           : utilStates.End()
            })

        self.getInnerStateMachine().addTransitions([
            ('start'            , 'next'    , 'search'      ),
            ('search'           , 'next'    , 'approach'    ),
            ('search'           , 'failure' , 'failure'     ),
            ('approach'         , 'next'    , 'bump'        ),
            ('approach'         , 'failure' , 'failure'     ),
            ('bump'             , 'next'    , 'back'        ),
            ('back'             , 'next'    , 'success'     )
            ])
