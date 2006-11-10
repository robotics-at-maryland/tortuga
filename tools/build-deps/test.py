import os
import sys
import shutil
import unittest

import build_deps
from ConfigParser import ConfigParser

class TestBuild_Deps(unittest.TestCase):
    def setUp(self):
        # Create our test directory
        self.base_dir = sys.path[0]
        self.test_dir = os.path.join(self.base_dir, 'test_loc')
        if os.path.exists(self.test_dir):
            shutil.rmtree(self.test_dir)
        os.mkdir(self.test_dir)
        
        # Create our config file
        self.config = ConfigParser()
        self.config.optionxform = lambda x: x
        self.config.add_section('test')
        self.config.set('test', 'touch', 'A B C D E F')
        self.config.set('test', 'mv', 'A BOB')
        self.config.set('test', 'rm', 'C D')

    def tearDown(self):
        shutil.rmtree(self.test_dir)
        
    def test_run_location(self):
        build_deps.run_location(('test','tools/build-deps/test_loc'), self.config)
        
        self.assertEquals(['B', 'BOB', 'E', 'F'], os.listdir(self.test_dir))
        self.assertEquals(self.base_dir, os.getcwd())
        
    def test_run_config(self):
        build_deps.run_config(os.path.join(self.base_dir, 'test.cfg'))
        self.assertEquals(['A'], os.listdir(self.test_dir))
        self.assertEquals(self.base_dir, os.getcwd())
        
    def test_run_config_location(self):
        build_deps.run_config(os.path.join(self.base_dir, 'test.cfg'), 'test')
        self.assertEquals(['A'], os.listdir(self.test_dir))
        self.assertEquals(self.base_dir, os.getcwd())
        

if __name__ == '__main__':
    unittest.main()
