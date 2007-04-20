# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

"""defines a class that writes L{code_creators.module_t} to single file"""

import os
import writer

class single_file_t(writer.writer_t):
    """generates all code into single cpp file"""

    def __init__(self, extmodule, file_name):
        writer.writer_t.__init__(self, extmodule)
        self.__fname = file_name

    def _get_file_name(self):
        return self.__fname
    file_name = property( _get_file_name )
    
    def write(self):        
        headers = self.get_user_headers( [self.extmodule] )        
        map( lambda header: self.extmodule.add_include( header )
             , headers )
        self.write_code_repository( os.path.split( self.file_name )[0] )
        self.write_file( self.file_name, self.extmodule.create() )
        
