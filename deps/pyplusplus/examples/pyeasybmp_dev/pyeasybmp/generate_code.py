#! /usr/bin/python
# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
from environment import settings
from pyplusplus import module_builder


license = \
"""
// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
"""

def export():
    global license
    header_file = os.path.join( settings.easybmp_path, 'EasyBMP.h' )
    #create configuration for GCC-XML parser
    #initialize module builder
    mb = module_builder.module_builder_t( [ header_file ]
                                          , gccxml_path=settings.gccxml_path
                                          , working_directory=settings.easybmp_path  )
    
    bmp_class = mb.class_( 'BMP' )
    call_operator = bmp_class.operator( symbol='()', recursive=False )
    call_operator.alias = 'GetRGBAPixel'
    call_operator.call_policies = module_builder.call_policies.return_internal_reference()
    #customizing code, before generation
    mb.build_code_creator( settings.module_name )
    mb.code_creator.license = license
    mb.code_creator.user_defined_directories.append( settings.easybmp_path )
    mb.code_creator.precompiled_header = 'boost/python.hpp'
    mb.code_creator.replace_included_headers( [header_file] )
    
    mb.write_module( os.path.join( settings.generated_files_dir, settings.module_name + '.cpp') )

if __name__ == '__main__':
    export()
    print 'done'