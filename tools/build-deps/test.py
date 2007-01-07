import os
import sys
import shutil
import unittest

import build_deps
from configobj import ConfigObj

# The mock function takeing from Trac
def Mock(bases=(), *initargs, **kw):
    if not isinstance(bases, tuple):
        bases = (bases,)
    cls = type('Mock', bases, {})
    mock = cls(*initargs)
    for k,v in kw.items():
        setattr(mock, k, v)
    return mock


class TestBuild_Deps(unittest.TestCase):
    def setUp(self):
        # Create our test directory
        self.base_dir = sys.path[0]
        self.test_dir = os.path.join(self.base_dir, 'test_loc')
        self.deps_dir = os.path.join(self.base_dir, 'depends')
        if os.path.exists(self.test_dir):
            shutil.rmtree(self.test_dir)
        os.mkdir(self.test_dir)

        if os.path.exists(self.deps_dir):
            shutil.rmtree(self.deps_dir)
        os.mkdir(self.deps_dir)
        
        # Create our config file
        self.config = ConfigObj()
        self.config['test'] = {
           'touch': 'A B C D E F',
           'mv' : 'A BOB',
           'rm' : 'C D'
            }
        self.config['test2'] = { 'touch' : 'John' }
        self.config['locations'] = {'test' : 'test_loc', 'test2' : 'test_loc'}
        self.config['groups'] = {'simgrp' : 'test', 'mygroup' : 'test test2'}
        
        self.config.filename = os.path.join(self.base_dir, 'test-tmp.cf')

        # Create fake options
        self.opts = Mock(root = self.base_dir,
                         force_install = False,
                         group_mode = False)

    def tearDown(self):
        dep_file = os.path.join(build_deps.get_build_root(self.opts), 'local', 
                               'test.depend')
        if os.path.exists(dep_file):
            os.remove(dep_file)
        shutil.rmtree(self.test_dir)
        if os.path.exists(self.deps_dir):
            shutil.rmtree(self.deps_dir)
        
    def test_run_location(self):
        build_deps.run_location(('test','test_loc'), self.config, self.opts)
        
        self.assertEquals(['B', 'BOB', 'E', 'F'], os.listdir(self.test_dir))
        self.assertEquals(self.base_dir, os.getcwd())

    def test_run_group(self):
        build_deps.setup_logging()
        build_deps.run_group('simgrp', self.config, self.opts)

        self.assertEquals(['B', 'BOB', 'E', 'F'], os.listdir(self.test_dir))
        self.assertEquals(self.base_dir, os.getcwd())

        build_deps.run_group('mygroup', self.config, self.opts)
        self.assertEquals(['John', 'B', 'BOB', 'E', 'F'],
                          os.listdir(self.test_dir))
        self.assertEquals(self.base_dir, os.getcwd())
        
    def test_standard_run(self):
        build_deps.main(['-c',os.path.join(self.base_dir, 'test.cfg'),
                         '-r', self.base_dir])
        self.assertEquals(['A'], os.listdir(self.test_dir))
        self.assertEquals(self.base_dir, os.getcwd())
        
    def test_translate_command(self):
        cur_dir = os.path.abspath(os.getcwd())
        self.assertFalse(build_deps.translate_command('make my-target'))
        
        self.assertTrue(build_deps.translate_command('cd /tmp'))
        self.assertEquals('/tmp', os.getcwd())
        os.chdir(cur_dir)
        
    #def test_run_config_location(self):
    #    build_deps.run_config(os.path.join(self.base_dir, 'test.cfg'), 'test')
    #    self.assertEquals(['A'], os.listdir(self.test_dir))
    #    self.assertEquals(self.base_dir, os.getcwd())
        

if __name__ == '__main__':
    unittest.main()
