# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import code_creator
import algorithm
import include_directories

class include_t(code_creator.code_creator_t):
    """
    Creates C++ code for include directive
    """
    def __init__( self, header ):
        code_creator.code_creator_t.__init__(self)
        self._header = include_directories.include_directories_t.normalize( header )
        self._include_dirs_optimization = None #This parameter will be set from module_t.create function
    
    def _get_header(self):
        return self._header
    def _set_header(self, header):
        self._header = include_directories.include_directories_t.normalize( header )
    header = property( _get_header, _set_header )
       
    def _get_include_dirs_optimization(self):
        return self._include_dirs_optimization
    def _set_include_dirs_optimization(self, include_dirs):
        self._include_dirs_optimization = include_dirs
    include_dirs_optimization = property( _get_include_dirs_optimization, _set_include_dirs_optimization )       
       
    def _create_impl(self):
        if not self.include_dirs_optimization:
            return '#include "%s"' % self.header
        else:
            normalize_header = self.include_dirs_optimization.normalize_header( self.header )
            if self.include_dirs_optimization.is_std(self.header):
                return '#include <%s>' % normalize_header
            else:
                return '#include "%s"' % normalize_header