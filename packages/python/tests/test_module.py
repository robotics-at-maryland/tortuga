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


#class DummyModule():
#    def start(self):
#        pass
#    def pause(self):
#        pass
#    def backgrounded(self):
#        pass
#    def update(self, time):
#        pass
#    def shutdown(self):
#        pass

def assert_module_count(_type, count):
    assert count == len(module.ModuleManager.get().get_module(_type))
            
class SuperModule(module.Module):
    core.implements(module.IModule)
    
    def __init__(self, config):
        self.config = config
        
        # Verify that sub modules are not yet loaded
        assert_module_count(Sub1Module, 0)
        assert_module_count(Sub2Module, 0)
        
        module.Module.__init__(self, config)
        
class Sub1Module(module.Module):
    core.implements(module.IModule)
    
    def __init__(self, config):
        self.config = config
        
        # Verify that super module is loaded
        assert_module_count(SuperModule, 1)
        
        module.Module.__init__(self, config)
        
class Sub2Module(module.Module):
    core.implements(module.IModule)
    
    def __init__(self, config):
        self.config = config
        
        # Verify that super module is loaded
        assert_module_count(SuperModule, 1)
        
        module.Module.__init__(self, config)
        
class SubSubModule(module.Module):
    core.implements(module.IModule)
    
    def __init__(self, config):
        self.config = config
        
         # Verify that super modules are loaded
        assert_module_count(Sub1Module, 1)
        assert_module_count(Sub2Module, 1)
        
        module.Module.__init__(self, config)

class TestModuleManager(unittest.TestCase):

    def setUp(self):
        # Create singleton log manager
        self.mod_mgr = module.ModuleManager()
        
    def tearDown(self):
        # Free all references to singleton log manager
        del self.mod_mgr
        module.ModuleManager.get().delete()

    def test_load_config(self):
        test_config = {
           'Modules': {
               'Manager' :
                   {'type' : 'tests.test_module.SuperModule',
                    'test' : 10 }, 
               'Servant1' :
                   {'type' : 'tests.test_module.Sub1Module',
                    'depends_on' : ['Manager'],
                    'test' : 5 } ,
               'Servant2' :
                   {'type' : 'tests.test_module.Sub2Module',
                    'depends_on' : ['Manager'],
                    'test' : 3 },
               'SubServant' :
                   {'type' : 'tests.test_module.SubSubModule',
                    'depends_on' : ['Servant1', 'Servant2'],
                    'test' : 11 }
               }
           }
        self.mod_mgr.load_modules(test_config)
        self.assertEqual(SuperModule, type(self.mod_mgr.get_module('Manager')))
        
        super_mod = self.mod_mgr.get_module(SuperModule)[0]
        self.assertEqual('Manager', super_mod.name)
        self.assertEqual(10 , super_mod.config['test'])
        
    def test_load_func(self):
        test_config = {
           'Modules': {
               'Manager' :
                   {'type' : 'tests.test_module.SuperModule',
                    'test' : 10 }
                   }
           }
        
        types = []
        def test_load(mod_type, mod_config):
            mod_type(mod_config)
            types.append(mod_type)
        
        self.mod_mgr._load_func = test_load
        self.mod_mgr.load_modules(test_config)
        
        self.assertEqual([SuperModule], types)
    
def get_suite():
    """
    Returns all TestSuites in this module
    """
    suites = []
    test_loader = unittest.TestLoader()
    
    suites.append(test_loader.loadTestsFromTestCase(TestModuleManager))
    
    return unittest.TestSuite(suites)
    
if __name__ == '__main__':
    unittest.TextTestRunner(verbosity=2).run(get_suite())