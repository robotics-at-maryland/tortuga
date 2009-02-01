#! /usr/bin/python
# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest

#__pychecker__ = 'limit=1000'
#import pychecker.checker

this_module_dir_path = os.path.abspath ( os.path.dirname( sys.modules[__name__].__file__) )

data_directory = os.path.join( this_module_dir_path, 'data' )
build_directory = os.path.join( this_module_dir_path, 'temp' )
build_dir = build_directory 

sys.path.append( os.path.dirname( this_module_dir_path ) )

from environment import scons, boost, python, gccxml

class scons_config:
    libs = ['boost_python']
    libpath = [ python.libs ] + boost.libs
    cpppath = [ boost.include, python.include ]
    include_dirs = cpppath + [data_directory]

    @staticmethod
    def create_sconstruct():
        code = [
              "env = Environment()"
            , "env.SharedLibrary( target=r'%(target)s'"
            , "    , source=[ %(sources)s ]"
            , "    , LIBS=[ %s ]" % ','.join( [ 'r"%s"' % lib for lib in scons_config.libs ] )
            , "    , LIBPATH=[ %s ]" % ','.join( [ 'r"%s"' % path for path in scons_config.libpath ] )
            , "    , CPPPATH=[ %s ]" % ','.join( [ 'r"%s"' % path for path in scons_config.include_dirs] )
            , "    , CCFLAGS=[ %s ]" % ','.join( [ 'r"%s"' % flag for flag in scons.ccflags ] )
            , "    , SHLIBPREFIX=''"
            , "    , SHLIBSUFFIX='%s'" % scons.suffix #explicit better then implicit
            , ")"
            , "env.AddPostAction('%(target)s', 'mt.exe -nologo -manifest %(target)s.pyd.manifest -outputresource:%(target)s.pyd;2'  )" ]          
        return os.linesep.join( code )

#I need this in order to allow Python to load just compiled modules
sys.path.append( build_dir )

os.chdir( build_dir )

#~ if sys.platform == 'linux2':
    #~ LD_LIBRARY_PATH = 'LD_LIBRARY_PATH'
    #~ if not os.environ.has_key( LD_LIBRARY_PATH ) \
       #~ or not set( scons_config.libpath ).issubset( set( os.environ[LD_LIBRARY_PATH].split(':') ) ):
        #~ #see http://hathawaymix.org/Weblog/2004-12-30
        #~ print 'error: LD_LIBRARY_PATH has not been set'
if sys.platform == 'win32':
    PATH = os.environ.get( 'PATH', '' )
    PATH=PATH + ';' + ';'.join( scons_config.libpath )
    os.environ['PATH'] = PATH


#~ try:
    #~ import pydsc
    #~ pydsc.include( r'D:\pygccxml_sources\sources\pyplusplus_dev' )
    #~ pydsc.ignore( [ 'Yakovenko'
             #~ , 'Bierbaum'
             #~ , 'org'
             #~ , 'http'
             #~ , 'bool'
             #~ , 'str'
             #~ , 'www'
             #~ , 'param'
             #~ , 'txt'
             #~ , 'decl'
             #~ , 'decls'
             #~ , 'namespace'
             #~ , 'namespaces'
             #~ , 'enum'
             #~ , 'const'
             #~ , 'GCC'
             #~ , 'xcc'
             #~ , 'TODO'
             #~ , 'typedef'
             #~ , 'os'
             #~ , 'normcase'
             #~ , 'normpath'
             #~ , 'scopedef'
             #~ , 'ira'#part of Matthias mail address
             #~ , 'uka'#part of Matthias mail address
             #~ , 'de'#part of Matthias mail address
             #~ , 'dat'#file extension of directory cache
             #~ , 'config'#parameter description
             #~ , 'gccxml'#parameter description
             #~ , 'Py++'
             #~ , 'pygccxml'
             #~ , 'calldef'
             #~ , 'XXX'
             #~ , 'wstring'
             #~ , 'py'
             #~ ] )
#~ except ImportError:
    #~ pass
