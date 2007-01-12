# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import autoconfig
from pygccxml import declarations
from pyplusplus import module_builder

class tester_t(unittest.TestCase):
    def _get_files( self ):
        files = [
            'enums_to_be_exported.hpp'
            , 'free_functions_to_be_exported.hpp'
            , 'free_operators_to_be_exported.hpp'
            , 'global_variables_to_be_exported.hpp'
            , 'index_operator_to_be_exported.hpp'
            , 'member_functions_to_be_exported.hpp'
        ]
        return map( lambda f: os.path.join( autoconfig.data_directory, f )
                    , files )

    def test(self):
        mb = module_builder.module_builder_t( self._get_files()
                                              , gccxml_path=autoconfig.gccxml.executable
                                              , include_paths=[autoconfig.boost.include]
                                              , undefine_symbols=['__MINGW32__'] )
        classes = filter( lambda d: isinstance( d, module_builder.class_t )
                          , declarations.make_flatten( mb.global_ns ) )
        
        mdw = module_builder.mdecl_wrapper_t( classes )
        #check set to property functionality
        for d in mdw:
            d.always_expose_using_scope = False
            
        mdw.always_expose_using_scope = True
        
        all_true = True
        for d in mdw:
            all_true &= d.always_expose_using_scope
            
        self.failUnless( all_true )
        
        #check call method functionality        
        for d in mdw:
            d.ignore = True
            
        mdw.include()
        
        all_false = False
        for d in mdw:
            all_true |= d.ignore
            
        self.failUnless( not all_false )
        
        #check for exception:
        try:
            mdw.call_policies = None
            self.fail( "Runtime error has not been raised." )
        except RuntimeError, err:
            pass
    
    def test__getitem__( self ):
        mb = module_builder.module_builder_t( self._get_files()
                                              , gccxml_path=autoconfig.gccxml.executable
                                              , include_paths=[autoconfig.boost.include]
                                              , undefine_symbols=['__MINGW32__'] )
        
        public_bases = mb.classes( 'public_base_t' )
        self.failUnless( 1 == len( public_bases ) )

        public_bases.include()
        self.failUnless( public_bases[0].ignore == False )
            
        mb.global_ns[ 'public_base_t' ].exclude()
                
        self.failUnless( public_bases[0].ignore == True )

    #def test__getitem__2( self ):
        #mb = module_builder.module_builder_t( self._get_files()
                                              #, gccxml_path=autoconfig.gccxml.executable
                                              #, include_paths=[autoconfig.boost.include]
                                              #, undefine_symbols=['__MINGW32__'] )
        
        #mem_funs = mb.classes( 'public_base_t' ).member_functions('regular')
        #print len(mem_funs)
        ##self.failUnless( 1 == len( public_bases ) )

        #mem_funs.include()
        #for mf in mem_funs:
            #self.failUnless( mf.ignore == False )
        
        #mb.global_ns[ 'public_base_t' ]['regular'].exclude()
                
        #for mf in mem_funs:
            #self.failUnless( mf.ignore == False )


def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()
