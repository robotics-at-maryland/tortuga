# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import shutil
import logging
import unittest
import autoconfig
from pygccxml import parser
from pygccxml import declarations
from pyplusplus import messages
from pyplusplus import code_creators
from pyplusplus import module_creator
from pyplusplus import module_builder
from pyplusplus import utils as pypp_utils
from pyplusplus import function_transformers as ft

class ogre_generate_tester_t(unittest.TestCase):        
    def test(self):
        module_builder.set_logger_level( logging.CRITICAL )
        messages.disable( *messages.all_warning_msgs )
        
        ogre_file = autoconfig.data_directory.replace( 'pyplusplus_dev', 'pygccxml_dev' )
        ogre_file = parser.create_gccxml_fc( os.path.join( ogre_file, 'ogre.1.7.xml' ) )

        mb = module_builder.module_builder_t(
                [ ogre_file ]
                , gccxml_path=autoconfig.gccxml.executable 
                , indexing_suite_version=2)
    
        mb.global_ns.exclude()
        mb.namespace('Ogre').include()
        
        x = mb.global_ns.decls( lambda d: 'Animation*' in d.name and 'MapIterator' in d.name )
        for y in x:
            print y.name
            print y.partial_name
            print declarations.full_name( y, with_defaults=False )
        
        target_dir = os.path.join( autoconfig.build_directory, 'ogre' )
        #~ if os.path.exists( target_dir ):
            #~ shutil.rmtree( target_dir )
        #~ os.mkdir( target_dir )
        
        mb.build_code_creator( 'Ogre3d' )
        mb.split_module( target_dir ) 



def create_suite():
    suite = unittest.TestSuite()
    suite.addTest( unittest.makeSuite(ogre_generate_tester_t))  
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()
