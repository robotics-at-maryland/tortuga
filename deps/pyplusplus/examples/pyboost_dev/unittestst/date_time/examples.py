#! /usr/bin/python
# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import unittest
import date_time
from date_time import gregorian
from date_time import posix_time
from date_time import local_time
from date_time import to_iso_string

class examples_t( unittest.TestCase ):
    def __init__( self, *args ):
        unittest.TestCase.__init__( self, *args )
    
    def test_days_alive(self):
        birthday = gregorian.date( 1900, date_time.Mar, 18 )
        today = gregorian.day_clock.local_day()
        alive = today - birthday
        print str(alive)

def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(examples_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()