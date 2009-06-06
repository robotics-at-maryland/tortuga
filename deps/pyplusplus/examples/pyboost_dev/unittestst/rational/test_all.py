# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import sys
sys.path.append( '../..' )

from pyboost import rational
import unittest

class tester_t( unittest.TestCase ):
    def __init__( self, *args ):
        unittest.TestCase.__init__( self, *args )
    
    def test_rational(self):      
        pyrational = rational.rational
        
        self.failUnless( pyrational( 28, 7) == 4 )
        self.failUnless( pyrational( 28, 7) == pyrational( 4 ) )
        
        r1 = pyrational( 5, 7 )
        
        r1 += pyrational( 4, 11 )
        r2 = pyrational( 5*11 + 7*4, 7*11 )
        self.failUnless( r1 == r2 )
        
        r1 -= pyrational( 5, 7)
        self.failUnless( r1 == pyrational( 4, 11) )
        
        r1 *= 2
        self.failUnless( r1 == pyrational( 8, 11) )
        
        r1 /= 3
        self.failUnless( r1 == pyrational( 8, 33) )
    
        r2 = not r1 
        self.failUnless( r2 == False )
        
        self.failUnless( 0 < r1 )
    
    def test_gcd( self ):
        gcd = rational.gcd
        self.failUnless( gcd(  1,  -1) == 1 )
        self.failUnless( gcd( -1,   1) == 1 )
        self.failUnless( gcd(  1,   1) == 1 )
        self.failUnless( gcd( -1,  -1) == 1 )
        self.failUnless( gcd(  0,   0) == 0 )
        self.failUnless( gcd(  7,   0) == 7 )
        self.failUnless( gcd(  0,   9) == 9 )
        self.failUnless( gcd( -7,   0) == 7 )
        self.failUnless( gcd(  0,  -9) == 9 )
        self.failUnless( gcd( 42,  30) == 6 )
        self.failUnless( gcd(  6,  -9) == 3 )
        self.failUnless( gcd(-10, -10) ==10 )
        self.failUnless( gcd(-25, -10) == 5 )
    
    def test_lcm( self ):
        lcm = rational.lcm
        self.failUnless( lcm(  1,  -1) == 1 )
        self.failUnless( lcm( -1,   1) == 1 )
        self.failUnless( lcm(  1,   1) == 1 )
        self.failUnless( lcm( -1,  -1) == 1 )
        self.failUnless( lcm(  0,   0) == 0 )
        self.failUnless( lcm(  6,   0) == 0 )
        self.failUnless( lcm(  0,   7) == 0 )
        self.failUnless( lcm( -5,   0) == 0 )
        self.failUnless( lcm(  0,  -4) == 0 )
        self.failUnless( lcm( 18,  30) ==90 )
        self.failUnless( lcm( -6,   9) ==18 )
        self.failUnless( lcm(-10, -10) ==10 )
        self.failUnless( lcm( 25, -10) ==50 )

    def test_abs( self ):
        pyrational = rational.rational
        
        self.failUnless( abs( pyrational( 28, 7) ) == pyrational( 28, 7) )
        self.failUnless( abs( pyrational( -28, 7) ) == pyrational( 28, 7) )
        self.failUnless( abs( pyrational( 28, -7) ) == pyrational( 28, 7) )

    def test_conversion( self ):
        pyrational = rational.rational
        
        half = pyrational( 1, 2 )
        self.failUnless( float(half) == 0.5 )
        self.failUnless( int(half) == 0 )
    
def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_t) )
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()