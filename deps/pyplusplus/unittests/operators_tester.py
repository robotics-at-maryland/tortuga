# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base
from pygccxml import declarations
from pyplusplus.module_builder import call_policies

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'operators'
    
    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__( 
            self
            , tester_t.EXTENSION_NAME
            , *args )

    def customize( self, mb ):
        mb.global_ns.exclude()

        xxx = mb.class_( 'XXX' )
        xxx.include()
        xxx_ref = declarations.reference_t( declarations.const_t( declarations.declarated_t( xxx ) ) )
        oper = mb.global_ns.free_operator( '<<', arg_types=[None, xxx_ref] )
        oper.include()
            
        mb.class_( 'YYY' ).include()
    
        rational = mb.class_('rational<long>')
        rational.include()
        rational.alias = "pyrational"
        
        #Test query api.
        rational.operator( '=' )
        rational.operator( name='operator=' )
        rational.operator( symbol='=' )
        rational.operators( '=' )
        rational.operators( name='operator=' )
        rational.operators( symbol='=' )
        rational.member_operator( '=' )
        rational.member_operator( name='operator=' )
        rational.member_operator( symbol='=' )
        rational.member_operators( '=' )
        rational.member_operators( name='operator=' )
        rational.member_operators( symbol='=' )
        mb.global_ns.free_operators( '<<' )
        mb.global_ns.free_operators( name='operator<<' )
        mb.global_ns.free_operators( symbol='<<' )
        
        r_assign = rational.calldef( 'assign', recursive=False )
        r_assign.call_policies = call_policies.return_self()

        foperators = mb.free_operators( lambda decl: 'rational<long>' in decl.decl_string )
        foperators.include()
            
        bad_rational = mb.class_('bad_rational' )
        bad_rational.include()

    def run_tests(self, module):      
        pyrational = module.pyrational
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
        
        r1 = pyrational( 5, 7 )
        self.failUnless( r1.assign( 17 ) == pyrational( 17, 1 ) )
    
        x = module.XXX()
        print str( x )
        
        y = module.YYY()
        print str( y )
        
def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()
