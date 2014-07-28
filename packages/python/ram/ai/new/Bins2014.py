import ram.ai.new.utilClasses as utilClasses
import ram.ai.new.utilStates as utilStates
import ram.ai.new.motionStates as motion
import ram.ai.new.searchPatterns as searches
import ram.ai.new.approach as approach
import ram.ai.new.checkpoints as checkpoints

from ram.ai.new.state import *
from ram.ai.new.stateMachine import *

class BinsTask(utilStates.Task):
    def __init__(self, binCenterVO, bin1VO, bin2VO, 
                 initialDepth, searchDist, success, failure, duration = 300):
        super(BinsTask, self).__init__(StateMachine(), success, failure, duration)

        self.getInnerStateMachine().addStates({
            'start'             : utilStates.Start(),
            'forwardsSearch'    : searches.ForwardsSearchPattern(searchDist,
                                                                 binCenterVO.isSeen, 
                                                                 'center', 'failure'),
            'center'            : approach.DownCenter(binCenterVO,
                                                      'saveCenter', 'failure'),
            'saveCenter'        : checkpoints.SaveCheckpoint('bins-center'),
            'bin1'              : BinSearchState(bin1VO, 1),
            'gotoCenter'        : checkpoints.GotoCheckpoint('bins-center'),
            'bin2'              : BinSearchState(bin2VO, 2),
            'failure'           : utilStates.End(),
            'success'           : utilStates.End()
            })

        self.getInnerStateMachine().addTransitions(
            ('start'            , 'next'    , 'forwardsSearch'   ),
            ('forwardsSearch'   , 'next'    , 'center'           ),
            ('forwardsSearch'   , 'failure' , 'failure'          ),
            ('center'           , 'next'    , 'saveCenter'       ),
            ('center'           , 'failure' , 'failure'          ),
            ('saveCenter'       , 'next'    , 'bin1'             ),
            ('bin1'             , 'complete', 'gotoCenter'       ),
            ('bin1'             , 'failure' , 'failure'          ),
            ('gotoCenter'       , 'next'    , 'bin2'             ),
            ('bin2'             , 'complete', 'success'          ),
            ('bin2'             , 'failure' , 'failure'          ) 
            )

    def update(self):
        super(BinsTask,self).update()
        if self.getInnerStateMachine().getCurrentState().getName() == 'failure':
            self.doTransition('failure')
        elif self.getInnerStateMachine().getCurrentState().getName() == 'success':
            self.doTransition('success')

@require_transitions('complete', 'failure')
class BinSearchState(utilStates.NestedState):
    def __init__(self, binVisionObject, markerNum):
        super(BinSearchState, self).__init__(StateMachine())

        self.getInnerStateMachine().addStates({
            'start'             : utilStates.Start(),
            'center'            : approach.DownCenter(binVisionObject,
                                                      'drop', 'end'),
            'drop'              : BinDrop(markerNum),
            'end'               : utilStates.End()
            })

        self.getInnerStateMachine().addTransitions(
            ('start'  , 'next'     , 'center'),
            ('center' , 'complete' , 'drop'  ),
            ('center' , 'failure'  , 'end'   ),
            ('drop'   , 'next'     , 'end'   )
            )

@require_transitions('next')
class BinDrop(State):
    def __init__(self, markerNum):
        super(BinDrop, self).__init__()
        self._markerNum = markerNum

    def enter(self):
        print 'Dropped marker ' + str(self._markerNum)
        #StateMachine._LEGACY_STATE.vehicle.dropMarkerIndex(self._markerNum)
        self.doTransition('next')
