# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/tests/core.py

"""
This module contains all the tests for sim.robot module.  The TestSuite for the
module is retreived with tests.sim.robot.get_suite().
"""

# Ptyhon Imports
import unittest
import sys

# Project Import
import core


class TestComponent(unittest.TestCase):
    def setUp(self):
        pass
    
    def tearDown(self):
        pass
    
    def test_get_class(self):
        class I1(core.Interface):
            pass
        class C1(core.Component):
            core.implements(I1)
            
        self.assertEqual(C1, core.Component.get_class(I1, __name__ + '.C1'))
    
    def test_interface_validation(self):
        class I1(core.Interface):
            def my_func(arg1, arg2):
                pass
            
        def missing_func():
            class C1(core.Component):
                core.implements(I1)
        
        def missing_arg():
            class C2(core.Component):
                core.implements(I1)
                def my_func(self, arg1):
                    pass
                
        self.assertRaises(core.BrokenImplementation, missing_func)
        self.assertRaises(core.BrokenMethodImplementation, missing_arg)
        
def get_suite():
    """
    Returns all TestSuites in this module
    """
    suites = []
    test_loader = unittest.TestLoader()
    
    suites.append(test_loader.loadTestsFromTestCase(TestComponent))
    
    return unittest.TestSuite(suites)
    
if __name__ == '__main__':
    unittest.TextTestRunner(verbosity=2).run(get_suite())