# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import autoconfig
from pygccxml import parser
from pygccxml import declarations
from pyplusplus import decl_wrappers
from pyplusplus import code_creators
from pyplusplus import module_creator
from pyplusplus import module_builder
from pyplusplus import utils as pypp_utils
from pyplusplus import function_transformers as ft

class tester_t(unittest.TestCase):
    CODE = """
    namespace ns{
        enum AAA{ aaa=1 };
        struct XXX{
            struct yyy{};
            enum bbb{ b=2 };
            void fff();
        };
        
        int VVV;
        void FFF( int );
    }
    namespace ns_skip{
        enum a123{ a1=1 };
        struct b123{
            struct c123{};
        };
        int fff();
        int i;
    }

    struct yyy{
        struct{ int i;};
    };
    
    struct zzz{
        union{
           int x; float y;
        };
    };
    
    namespace{
        int xxxx;
    }
    """
    def __init__(self, *args ):
        unittest.TestCase.__init__(self, *args)

    def test(self):
        db = pypp_utils.exposed_decls_db_t()
        config = parser.config_t( gccxml_path=autoconfig.gccxml.executable )

        reader = parser.project_reader_t( config, None, decl_wrappers.dwfactory_t() )
        decls = reader.read_files( [parser.create_text_fc(self.CODE)] )
    
        global_ns = declarations.get_global_namespace( decls )
        ns = global_ns.namespace( 'ns' )
        ns_skip = global_ns.namespace( 'ns_skip' )

        global_ns.exclude()        
        ns.include()
        
        db.register_decls( global_ns, [] )
                    
        for x in ns.decls(recursive=True):
            self.failUnless( db.is_exposed( x ) == True )
            
        for x in ns_skip.decls(recursive=True):
            self.failUnless( db.is_exposed( x ) == False )

        db.save( os.path.join( autoconfig.build_dir, 'exposed.db.pypp' ) )

        db2 = pypp_utils.exposed_decls_db_t()
        db2.load( os.path.join( autoconfig.build_dir, 'exposed.db.pypp' ) )
        for x in ns.decls(recursive=True):
            self.failUnless( db.is_exposed( x ) == True )

        ns_skip = global_ns.namespace( 'ns_skip' )
        for x in ns_skip.decls(recursive=True):
            self.failUnless( db.is_exposed( x ) == False )

def create_suite():
    suite = unittest.TestSuite()
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()
