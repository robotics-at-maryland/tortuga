# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  tests/sim/tobject.py

# Ptyhon Imports
import unittest

# Library Imports
import ogre.renderer.OGRE as Ogre

# Project Imporst
import core

# Module to test
import sim.util as util
import sim.object as tobject

class TestObject(unittest.TestCase):
    def setUp(self):
        self.obj = tobject.Object(None, 'John')
	
    def tearDown(self):
        pass

    def test_init(self):
        # No Parent case
        obj = tobject.Object(None, 'Bob')
        self.assertEqual(None, obj.parent)

        # Parent case
        obj = tobject.Object(self.obj, 'Bob')
        self.assertEqual(self.obj, obj.parent)
        self.assertEqual(obj, self.obj.get_child('Bob'))

    def test_name(self):
        self.assertEquals('John', self.obj.name)
        
    def test_get_child(self):
        # Test No Children
        self.assertRaises(util.SimulationError, self.obj.get_child, 'bob')
        
        # Add and then check
        child_obj = tobject.Object(None, 'Jane')
        self.obj.add_child(child_obj)
        self.assertEquals(child_obj, self.obj.get_child('Jane'))
    
    def test_add_child(self):
        child_obj = tobject.Object(None, 'Jane')
        child_obj2 = tobject.Object(None, 'Dick')
        
        self.obj.add_child(child_obj)
        self.assertEquals(child_obj, self.obj.get_child('Jane'))
        self.obj.add_child(child_obj2)
        self.assertEquals(child_obj2, self.obj.get_child('Dick'))
    
    def test_remove_child(self):
        child_obj = tobject.Object(None, 'Dick')
        self.obj.add_child(tobject.Object(None, 'Jane'))
        self.obj.add_child(child_obj)
        
        # Test Remove By Name
        self.obj.remove_child('Jane')
        self.assertRaises(util.SimulationError, self.obj.get_child, 'Jane')
         
        # Test remove by object
        self.obj.remove_child(child_obj)
        self.assertRaises(util.SimulationError, self.obj.get_child, child_obj)

def get_suite():
	"""
	Returns all TestSuites in this module
	"""
	suites = []
	test_loader = unittest.TestLoader()
	
	suites.append(test_loader.loadTestsFromTestCase(TestObject))
	
	return unittest.TestSuite(suites)
	
if __name__ == '__main__':
	unittest.TextTestRunner(verbosity=2).run(get_suite())
