import os
import sys
import getpass

class boost:
    libs = ''
    include = ''
    
class python:
    libs = ''
    include = ''

class gccxml:
    executable = ''

class scons:
    suffix = ''
    cmd_build = ''
    cmd_clean = ''
    ccflags = []
    
if 'roman' in getpass.getuser():
    
    scons.cmd_build = 'scons --file=%s'
    scons.cmd_clean = 'scons --clean --file=%s'

    if sys.platform == 'win32':
        scons.suffix = '.pyd'
        scons.ccflags = ['/MD', '/EHsc', '/GR', '/Zc:wchar_t', '/Zc:forScope' ]
        boost.libs = [ 'd:/dev/boost_cvs/libs/python/build/bin-stage' ]
        boost.include = 'd:/dev/boost_cvs'
        python.libs = 'e:/python25/libs'
        python.include = 'e:/python25/include'
        gccxml.executable = r'd:/dev/gccxml_cvs/gccxml-bin/bin/release/gccxml.exe'
    else:
        scons.suffix = '.so'
        boost.libs = [ '/home/roman/boost_cvs/libs/python/build/bin-stage' ]
        boost.include = '/home/roman/boost_cvs'
        python.include = '/usr/include/python2.4'
        gccxml.executable = '/home/roman/gccxml-build/bin/gccxml'


_my_path = None
try:
    import environment_path_helper
    environment_path_helper.raise_error()
except Exception, error:
    _my_path = os.path.abspath( os.path.split( sys.exc_traceback.tb_frame.f_code.co_filename )[0] )
    if not os.path.exists( os.path.join( _my_path, 'environment.py' ) ):
        #try another guess
        if sys.modules.has_key('environment'):
            _my_path = os.path.split( sys.modules['environment'].__file__ )[0]

try:
    import pygccxml
    print 'pygccxml INSTALLED version will be used'
except ImportError:
    sys.path.append( os.path.join( _my_path, '../pygccxml_dev' ) )
    import pygccxml
    print 'pygccxml DEVELOPMENT version will be used'

import pyplusplus

