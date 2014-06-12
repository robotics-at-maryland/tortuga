# Tests
from backup import *
import unittest
from datetime import datetime

class TestBackup(unittest.TestCase):
    test_dir = os.tempnam()
    
    def cleandirs(self):
        if os.path.exists(self.test_dir):
            for d in os.listdir(self.test_dir):
                os.rmdir(os.path.join(self.test_dir, d))
            os.rmdir(self.test_dir)
                
    def setUp(self):
        self.cleandirs()
        os.mkdir(self.test_dir)
        for i in xrange(1,11):
            os.mkdir(self.test_dir + os.sep + 
                     datetime(2006, i, 10).strftime('%F_%T'))
    
    def tearDown(self):
        self.cleandirs()
        
    def test_remove_old(self):
        remove_old(self.test_dir, datetime(2006, 5, 10))
        self.assertEqual(len(os.listdir(self.test_dir)), 6)
        
        for i in xrange(5,11):
            self.assertTrue(os.path.exists(os.path.join(self.test_dir,
                            datetime(2006,i,10).strftime('%F_%T'))))
    
    def test_remove_path(self):
        os.mkdir(os.path.join(self.test_dir, os.listdir(self.test_dir)[0], 'tmp'))
        remove_path(self.test_dir)
        self.assertFalse(os.path.exists(self.test_dir))
        
if __name__ == '__main__':
    unittest.main()