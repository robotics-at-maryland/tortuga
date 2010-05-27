
import ram.ai.buoy as buoy
import ram.motion as motion
import ram.motion.basic

import ram.test.ai.support as support

class TestStart(support.AITestCase):
    def setUp(self):
        pass

    def testNoBuoys(self):
        print 'test no buoys'
        cfg = {
            'Ai' : {
                'config' : {
                    'lightDepth' : 5,
                    'targetBuoys' : []
                    }
                }
            }

        support.AITestCase.setUp(self, cfg = cfg)
        self.machine.start(buoy.Start)

        self.qeventHub.publishEvents()
        self.assertCurrentState(buoy.Continue)

    def testOneBuoy(self):
        print 'test one buoy'
        cfg = {
            'Ai' : {
                'config' : {
                    'buoyDepth' : {
                        'red' : 8
                        },
                    'targetBuoys' : ['red']
                    }
                }
            }

        support.AITestCase.setUp(self, cfg = cfg)
        self.machine.start(buoy.Start)

        # Make sure start is diving
        #self.qeventHub.publishEvents()
        print self.motionManager.currentMotion
        self.assertCurrentState(buoy.Start)
        self.assertCurrentMotion(motion.basic.RateChangeDepth)
        self.assertEqual(8, self.motionManager.currentMotion.desiredDepth)
