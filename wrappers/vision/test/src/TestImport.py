# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File: wrapper/vision/test/src/TestImport.py

# STD Imports
import unittest

class VisionImport(unittest.TestCase):
    def testImport(self):
        import ext.vision

if __name__ == '__main__':
    unittest.main()