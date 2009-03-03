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
        
        xml_file = parser.create_gccxml_fc( os.path.join( autoconfig.data_directory, 'particleuniverse.xml' ) )

        mb = module_builder.module_builder_t(
                [ xml_file ]
                , gccxml_path=autoconfig.gccxml.executable 
                , indexing_suite_version=2)
    
        mb.global_ns.exclude()
        mb.namespace('ParticleUniverse').include()
        mb.namespace('Ogre').include()
        mb.namespace('Ogre').classes().already_exposed = True

        target_dir = os.path.join( autoconfig.build_directory, 'particle_universe' )
        #~ if os.path.exists( target_dir ):
            #~ shutil.rmtree( target_dir )
        #~ os.mkdir( target_dir )
        psp = mb.class_( '::ParticleUniverse::ParticleScriptParser' )
        declarations.print_declarations( psp )
        mb.build_code_creator( 'PU' )
        mb.split_module( target_dir ) 



def create_suite():
    suite = unittest.TestSuite()
    suite.addTest( unittest.makeSuite(ogre_generate_tester_t))  
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()
