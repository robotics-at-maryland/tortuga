#! /usr/bin/python
# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import sys
sys.path.append( '../..' )

import unittest
import gregorian_tester
import posix_time_tester
import local_time_tester

def create_suite():
    suite = unittest.TestSuite()    
    testers = [ gregorian_tester
                , posix_time_tester 
                , local_time_tester
    ]
    for i in range(10):
        for tester in testers:
            suite.addTest( tester.create_suite() )
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()