#! /usr/bin/python
# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import sys
sys.path.append( '../..' )

from pyboost import boost_random
import unittest

class tester_t( unittest.TestCase ):
    def __init__( self, *args ):
        unittest.TestCase.__init__( self, *args )
    
    def randomness_tester( self, generator, count=1000 ):
        generated = set()
        for i in range( count ):
            value = generator()
            generated.add( value )
        self.failUnless( len( generated ) >= count * 0.9 )

    def generator_tester( self, gclass, *args ):
        gen = gclass()
        self.randomness_tester( gen )
        print '\nname : ', gen.__class__.__name__
        print '  max: ', gen.max()
        print '  min: ', gen.min()
        gen = gclass( *args )
        self.randomness_tester( gen )
        gen.seed()
        self.randomness_tester( gen )
    
    def test_generators(self):
        for gclass in boost_random.generators:
            self.generator_tester(gclass)
        
    def instantiate_dist( self, urng, dist ):
        if not boost_random.variate_generator_exists( urng, dist ):
            return
        gen1 = boost_random.variate_generator( urng, dist)
        gen2 = boost_random.variate_generator( urng, dist)
        self.randomness_tester( gen1 )
        self.randomness_tester( gen2 )
        self.failUnless( abs( gen1() - gen2() ) < 1e-6 )
        gen1.engine()
        gen1.distribution().reset()
        gen2.engine()
        gen2.distribution().reset()

    def instantiate_real_dist( self, urng ):
        self.instantiate_dist(urng, boost_random.uniform_real(0, 2.1) )
        self.instantiate_dist(urng, boost_random.triangle_distribution(1, 1.5, 7) )
        self.instantiate_dist(urng, boost_random.exponential_distribution(5) )
        self.instantiate_dist(urng, boost_random.normal_distribution(0,1) )
        #self.instantiate_dist(urng, boost_random.normal_distribution() )
        self.instantiate_dist(urng, boost_random.lognormal_distribution(1, 1) )
        self.instantiate_dist(urng, boost_random.cauchy_distribution(1, 1) )
        self.instantiate_dist(urng, boost_random.gamma_distribution(1) )

    def test_distributions( self ):
        for gclass in boost_random.generators:
            self.instantiate_real_dist( gclass() )

        
def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_t) )
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()