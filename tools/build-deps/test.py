import os
import sys
import shutil
import unittest

import build_deps
from configobj import ConfigObj

class TestBuild_Deps(unittest.TestCase):
    def setUp(self):
        # Create our test directory
        self.base_dir = sys.path[0]
        self.test_dir = os.path.join(self.base_dir, 'test_loc')
        if os.path.exists(self.test_dir):
            shutil.rmtree(self.test_dir)
        os.mkdir(self.test_dir)
        
        # Create our config file
        self.config = ConfigObj()
        self.config['test'] = {
           'touch': 'A B C D E F',
           'mv' : 'A BOB',
           'rm' : 'C D'
            }
        self.config.filename = os.path.join(self.base_dir, 'test-tmp.cf')

    def tearDown(self):
        dep_file = os.path.join(build_deps.get_svn_root(), 'deps', 'local', 
                               'test.depend')
        if os.path.exists(dep_file):
            os.remove(dep_file)
        shutil.rmtree(self.test_dir)
        
    def test_run_location(self):
        build_deps.run_location(('test','test_loc'), self.config)
        
        self.assertEquals(['B', 'BOB', 'E', 'F'], os.listdir(self.test_dir))
        self.assertEquals(self.base_dir, os.getcwd())
        
    def test_standard_run(self):
        build_deps.main(['-c',os.path.join(self.base_dir, 'test.cfg')])
        self.assertEquals(['A'], os.listdir(self.test_dir))
        self.assertEquals(self.base_dir, os.getcwd())
        
    #def test_run_config_location(self):
    #    build_deps.run_config(os.path.join(self.base_dir, 'test.cfg'), 'test')
    #    self.assertEquals(['A'], os.listdir(self.test_dir))
    #    self.assertEquals(self.base_dir, os.getcwd())
        

if __name__ == '__main__':
    unittest.main()
