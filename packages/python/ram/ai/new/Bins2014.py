import ram.ai.new.utilClasses as utilClasses
import ram.ai.new.utilStates as utilStates
import ram.ai.new.motionStates as motion
import ram.ai.new.searchPatterns as searches
import ram.ai.new.approach as approach
import ram.ai.new.checkpoints as checkpoints

from ram.ai.new.state import *
from ram.ai.new.stateMachine import *

import itertools
from collections import defaultdict
from operator import itemgetter

class BinData(object):
    def __init__(self, legacyState, symbol1, symbol2):
        self.bins = []
        for i in xrange(5):
            #self.bins.append(utilClasses.BinVisionObject(legacyState, i))
            self.bins.append(utilClasses.FakeBinVisionObject(legacyState, i))

        self.symbol1 = symbol1
        self.count1 = defaultdict(int)

        self.symbol2 = symbol2
        self.count2 = defaultdict(int)

class BinsTask(utilStates.Task):
    def __init__(self, binSymbol1, binSymbol2, searchDist, hoverTime, success, failure, duration = 300):
        super(BinsTask, self).__init__(StateMachine(), success, failure, duration)

        self.data = BinData(self.getInnerStateMachine().getLegacyState(), binSymbol1, binSymbol2)

        self.getInnerStateMachine().addStates({
            'start'             : utilStates.Start(),
            'forwardsSearch'    : searches.ForwardsSearchPattern(searchDist,
                                                                 self.data.bins[0].isSeen, 
                                                                 'center', 'failure'),
            'center'            : approach.DownCenter(self.data.bins[0],
                                                      'buf1', 'failure'),
            'buf1'              : motion.Forward(0),
            'hover'             : BinHover(self.data, hoverTime),
            'saveCenter'        : checkpoints.SaveCheckpoint('bins-center'),
            'bin1'              : BinSearch(self.data, 1),
            'gotoCenter'        : checkpoints.GotoCheckpoint('bins-center'),
            'bin2'              : BinSearch(self.data, 2),
            'failure'           : utilStates.End(),
            'success'           : utilStates.End()
            })

        self.getInnerStateMachine().addTransitions(
            ('start'            , 'next'    , 'forwardsSearch'   ),
            ('forwardsSearch'   , 'next'    , 'center'           ),
            ('forwardsSearch'   , 'failure' , 'failure'          ),
            ('center'           , 'next'    , 'buf1'             ),
            ('center'           , 'failure' , 'failure'          ),
            ('buf1'             , 'next'    , 'hover'            ),
            ('hover'            , 'next'    , 'saveCenter'       ),
            ('saveCenter'       , 'next'    , 'bin1'             ),
            ('bin1'             , 'complete', 'gotoCenter'       ),
            ('bin1'             , 'failure' , 'failure'          ),
            ('gotoCenter'       , 'next'    , 'bin2'             ),
            ('bin2'             , 'complete', 'success'          ),
            ('bin2'             , 'failure' , 'failure'          ) 
            )

    def enter(self):
        super(BinsTask,self).enter()
        self.getInnerStateMachine().getLegacyState().visionSystem.binDetectorOn()

    def update(self):
        super(BinsTask,self).update()
        if self.getInnerStateMachine().getCurrentState().getName() == 'failure':
            self.doTransition('failure')
        elif self.getInnerStateMachine().getCurrentState().getName() == 'success':
            self.doTransition('complete')

    def leave(self):
        super(BinsTask,self).enter()
        self.getInnerStateMachine().getLegacyState().visionSystem.binDetectorOff()

@require_transitions('next')
class BinHover(State):
    def __init__(self, binData, hoverTime):
        super(BinHover, self).__init__()
        self.data = binData
        self.timer = utilClasses.Timer(hoverTime)

    def enter(self):
        self.timer.reset()

    def update(self):
        if not self.timer.check():
            self.doTransition('next')
            return

        for i in xrange(1,5):
            if self.data.bins[i].symbol == self.data.symbol1:
                self.data.count1[i] += 1
            elif self.data.bins[i].symbol == self.data.symbol2:
                self.data.count2[i] += 1

@require_transitions('complete', 'failure')
class BinSearch(utilStates.NestedState):
    def __init__(self, binData, markerNum):
        super(BinSearch, self).__init__(StateMachine())
        self.data = binData
        self.markerNum = markerNum

    def enter(self):
        count = getattr(self.data, 'count' + str(self.markerNum))
        i = 1
        for k, v in count.iteritems():
            if v > i:
                i = k
        print 'Centering on Bin ' + str(i)
        vo = self.data.bins[i]

        self.getInnerStateMachine().addStates({
            'start'             : utilStates.Start(),
            'center'            : approach.DownCenter(vo, 'buf1', 'end', xBound = .05, yBound = .05),
            'buf1'              : motion.Forward(0),
            'drop'              : BinDrop(self.markerNum),
            'end'               : utilStates.End()
            })

        self.getInnerStateMachine().addTransitions(
            ('start'  , 'next'     , 'center'),
            ('buf1'   , 'next'     , 'drop'  ),
            ('drop'   , 'next'     , 'end'   )
            )

        super(BinSearch, self).enter()

@require_transitions('next')
class BinDrop(State):
    def __init__(self, markerNum):
        super(BinDrop, self).__init__()
        self._markerNum = markerNum

    def enter(self):
        print 'Dropped marker ' + str(self._markerNum)
        StateMachine._LEGACY_STATE.vehicle.dropMarkerIndex(self._markerNum)
        self.doTransition('next')
