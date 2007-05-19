# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/tests/core.py

# Ptyhon Imports
import unittest
import sys

# Project Import
import core
import module


class SuperModule(object):
    core.implements(module.IModule)
    
    def __init__(self, config):
        self. config = config
        

class TestModuleManager(unittest.TestCase):
    pass


def get_suite():
    """
    Returns all TestSuites in this module
    """
    suites = []
    test_loader = unittest.TestLoader()
    
    suites.append(test_loader.loadTestsFromTestCase(TestComponent))
    
    return unittest.TestSuite(suites)
    
if __name__ == '__main__':
    unittest.TextTestRunner(verbosity=2).run(get_suite()