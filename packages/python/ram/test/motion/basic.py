# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/motion.py

# STD Imports
import unittest
import math as pmath

# Project Imports
import ext.math as math
import ext.core as core

import ram.motion as motion
import ram.motion.basic
import ram.test.motion.support as support
from ram.motion.trajectories import StepTrajectory

class TestMotionManager(support.MotionTest):
    def setUp(self):
        support.MotionTest.setUp(self)

    def testSetMotion(self):
        m = support.MockMotion()
        self.motionManager.setMotion(m)
        
        self.assertEquals(self.vehicle.getName(), m.vehicle.getName())
        self.assertEquals(self.controller.getName(), m.controller.getName())
        self.assertEquals(self.estimator.getName(), m.estimator.getName())
        
        m2 = support.MockMotion()
        self.motionManager.setMotion(m2)
        self.assert_(m.stopped)
        self.assertEquals(self.vehicle.getName(), m2.vehicle.getName())
        self.assertEquals(self.controller.getName(), m2.controller.getName())
        self.assertEquals(self.estimator.getName(), m2.estimator.getName())
        
    def testMultiMotion(self):
        m = support.MockMotion()
        mPlane = support.MockMotion(_type = motion.basic.Motion.IN_PLANE)
        mDepth = support.MockMotion(_type = motion.basic.Motion.DEPTH)
        mOrien = support.MockMotion(_type = motion.basic.Motion.ORIENTATION)
        
        _type = motion.basic.Motion.ORIENTATION | motion.basic.Motion.IN_PLANE
        mPlaneOrien = support.MockMotion(_type = _type)
        
        # Place an all enclusive motion
        self.motionManager.setMotion(m)
        self.assertEqual(m, self.motionManager.currentMotion)
        
        # Replace with just a depth one
        self.motionManager.setMotion(mPlane)
        self.assertEqual(mPlane, self.motionManager.currentMotion)
        self.assert_(m.stopped)
        self.assert_(mPlane.started)
        
        # Add a depth one and make sure they are both still present
        self.motionManager.setMotion(mDepth)
        self.assertEqual((mPlane,mDepth,None), 
                         self.motionManager.currentMotion)
        self.assert_(mDepth.started)
        
        # Add orientation one
        self.motionManager.setMotion(mOrien)
        self.assertEqual((mPlane,mDepth,mOrien),
                         self.motionManager.currentMotion)
        self.assert_(mOrien.started)


        # Now replace both in plane and depth with a single new motion
        self.motionManager.setMotion(mPlaneOrien)
        self.assertEqual((mPlaneOrien,mDepth,mPlaneOrien),
                         self.motionManager.currentMotion)
        self.assert_(mOrien.stopped)
        self.assert_(mPlane.stopped)
        self.assert_(mPlaneOrien.started)
        
        # Now stop that multimotion by just doing a single type stop
        self.motionManager.stopMotionOfType(motion.basic.Motion.IN_PLANE)
        self.assertEqual(mDepth, self.motionManager.currentMotion)
        self.assert_(mPlaneOrien.stopped)
        
        
        # Now make sure a single multi motion shows up like that
        self.motionManager.stopCurrentMotion()
        mPlaneOrien = support.MockMotion(_type = _type)
        self.motionManager.setMotion(mPlaneOrien)
        self.assertEqual(mPlaneOrien, self.motionManager.currentMotion)
        
    def testQueuedMotions(self):
        def _handler(event):
            self._finished = True

        self._finished = False
        self.qeventHub.subscribeToType(
            motion.basic.MotionManager.FINISHED, _handler)

        m1 = support.MockMotion(1)
        m2 = support.MockMotion(2)
        
        self.motionManager.setMotion(m1, m2)

        # Make sure it started the first motion
        self.assertEqual(m1, self.motionManager.currentMotion)
        self.assertFalse(self._finished)
        
        # Finish the motion
        m1._finish()
        self.qeventHub.publishEvents()
        self.assertEqual(m2, self.motionManager.currentMotion)
        self.assertFalse(self._finished)

        # Now finish the second motion and make sure it publishes the event
        m2._finish()
        self.qeventHub.publishEvents()
        self.assertEqual(None, self.motionManager.currentMotion)
        self.assert_(self._finished)

    def testStopCurrentMotion(self):
        m = support.MockMotion()
        self.motionManager.setMotion(m)

        self.motionManager.stopCurrentMotion()
        self.assert_(m.stopped)
  
        self.assertEqual(None, self.motionManager.currentMotion)
        
    def testMotionFinished(self):
        m = support.MockMotion()
        self.motionManager.setMotion(m)
        
        self.assertEqual(m, self.motionManager.currentMotion)
        
        event = core.Event()
        event.motion = m
        self.motionManager.publish(motion.basic.Motion.FINISHED, event)
        self.qeventHub.publishEvents()
        
        self.assertEqual(None, self.motionManager.currentMotion)

    def testMotionConfig(self):
        mList = {
            '1' : {
                'type' : 'ram.test.motion.support.MockMotion',
                '_type' : motion.basic.Motion.IN_PLANE
                },
            '2' : {
                'type' : 'ram.test.motion.support.MockMotion',
                '_type' : motion.basic.Motion.ORIENTATION
                }
            }

        # Generate the motion list
        motionList = motion.basic.MotionManager.generateMotionList(mList)

        self.assert_(all([lambda x: type(x) == motion.basic.test.MockMotion
                          for x in motionList]))
        self.assertEqual(motion.basic.Motion.IN_PLANE, motionList[0].type)
        self.assertEqual(motion.basic.Motion.ORIENTATION, motionList[1].type)
  
class TestChangeDepth(support.MotionTest):
    def setUp(self):
        support.MotionTest.setUp(self)
        self.motionFinished = False

    def handleFinished(self, event):
        self.motionFinished = True
        
    def testType(self):
        m = motion.basic.ChangeDepth(StepTrajectory(10, 5))
        self.assertEqual(motion.basic.Motion.DEPTH, m.type)
        
    def testChangeDepth(self):
        # this doesnt need to test how we get to the result, it only
        # tests that we get there without crashing along the way
        m = motion.basic.ChangeDepth(StepTrajectory(initialValue = 10,
                                                    finalValue = 5),
                                     updateRate = 1)

        # set the final position
        self.estimator.depth = 5

        self.qeventHub.subscribeToType(motion.basic.Motion.FINISHED, 
                                       self.handleFinished)
        
        # set the motion
        self.motionManager.setMotion(m)
        print m._timer
        # publish the event to update
        timer = support.MockTimer.LOG[
            motion.basic.ChangeDepth.DEPTH_TRAJECTORY_UPDATE]
        timer.finish()
        self.qeventHub.publishEvents()

        # see if the motion finished
        self.assert_(self.motionFinished)

        # Make sure we have reached the final value
        self.assert_(self.controller.atDepth())

    # def testGenerator(self):
    #     # Test the generator for this motion works
    #     m = motion.basic.MotionManager.generateMotion(
    #         'ram.motion.basic.ChangeDepth',
    #         desiredDepth = 5,
    #         steps = 10)

    #     self.assertEqual(m._desiredDepth, 5)
    #     self.assertEqual(m._steps, 10)

    #     m = motion.basic.MotionManager.generateMotion(
    #         'ram.motion.basic.ChangeDepth', complete = True,
    #         desiredDepth = 5,
    #         steps = 10)

    #     self.assertEqual(m._desiredDepth, 5)
    #     self.assertEqual(m._steps, 10)

class TestTranslate(support.MotionTest):
    def setUp(self):
        support.MotionTest.setUp(self)
        self.motionFinished = False

    def handleFinished(self, event):
        self.motionFinished = True

    def testType(self):
        m = motion.basic.Translate(
            StepTrajectory(initialValue = math.Vector2(10, 10),
                           finalValue = math.Vector2(5, 5)))

        self.assertEqual(motion.basic.Motion.IN_PLANE, m.type)

    def testTranslate(self):
        # this doesnt need to test how we get to the result, it only
        # tests that we get there without crashing along the way
        m = motion.basic.Translate(
            StepTrajectory(initialValue = math.Vector2(10, 10),
                           finalValue = math.Vector2(5, 5),
                           initialRate = math.Vector2(0,0),
                           finalRate = math.Vector2(0,0)),
            updateRate = 1)
        
        self.estimator.position = math.Vector2(5,5)
        self.estimator.velocity = math.Vector2(0,0)

        self.qeventHub.subscribeToType(motion.basic.Motion.FINISHED, 
                                       self.handleFinished)
        
        # set the motion
        self.motionManager.setMotion(m)

        # publish the event to update
        timer = support.MockTimer.LOG[
            motion.basic.Translate.INPLANE_TRAJECTORY_UPDATE]
        timer.finish()
        self.qeventHub.publishEvents()

        # see if the motion finished
        self.assert_(self.motionFinished)

        # Make sure we have reached the final value
        self.assert_(self.controller.atPosition())

    # def testGenerator(self):

            

class TestChangeOrientation(support.MotionTest):
    def setUp(self):
        support.MotionTest.setUp(self)
        self.motionFinished = False

    def handleFinished(self, event):
        self.motionFinished = True

    def testType(self):
        m = motion.basic.ChangeOrientation(\
            StepTrajectory(initialValue = math.Quaternion(1,0,0,0),
                           finalValue = math.Quaternion(0,0,0,1)))
        self.assertEqual(motion.basic.Motion.ORIENTATION, m.type)

    def testChangeOrientation(self):
        # this doesnt need to test how we get to the result, it only
        # tests that we get there without crashing along the way
        m = motion.basic.ChangeOrientation(\
            StepTrajectory(initialValue = math.Quaternion(1,0,0,0),
                           finalValue = math.Quaternion(0,0,0,1)),
            updateRate = 1)
        
        self.estimator.orientation = math.Quaternion(0,0,0,1)

        self.qeventHub.subscribeToType(motion.basic.Motion.FINISHED, 
                                       self.handleFinished)
        
        # set the motion
        self.motionManager.setMotion(m)

        # publish the event to update
        timer = support.MockTimer.LOG[
            motion.basic.ChangeOrientation.ORIENTATION_TRAJECTORY_UPDATE]
        timer.finish()
        self.qeventHub.publishEvents()

        # see if the motion finished
        self.assert_(self.motionFinished)

        # Make sure we have reached the final value
        self.assert_(self.controller.atOrientation())

    # def testGenerator(self):

            
if __name__ == '__main__':
    unittest.main()
        
