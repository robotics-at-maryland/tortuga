import os
import sys
import getpass

this_module_dir_path = os.path.abspath ( os.path.dirname( sys.modules[__name__].__file__) )

class boost:
    libs = ''
    include = ''

class python:
    libs = ''
    include = ''

class gccxml:
    gccxml_09_path = os.path.join( this_module_dir_path, '..', 'gccxml_bin', 'v09', sys.platform, 'bin' )

    gccxml_path = gccxml_09_path
    gccxml_version = '__GCCXML_09__'

    executable = gccxml_path

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
        scons.ccflags = ['/MD', '/EHsc', '/GR', '/Zc:wchar_t', '/Zc:forScope', '-DBOOST_PYTHON_NO_PY_SIGNATURES' ]
        boost.libs = [ 'd:/dev/boost_svn/bin.v2/libs/python/build/msvc-7.1/release' ]
        boost.include = 'd:/dev/boost_svn'
        python.libs = 'e:/python25/libs'
        python.include = 'e:/python25/include'
    else:
        scons.suffix = '.so'
        scons.ccflags = ['-DBOOST_PYTHON_NO_PY_SIGNATURES' ]
        boost.libs = ['/home/roman/include/libs' ]
        boost.include = '/home/roman/boost_svn'
        python.include = '/usr/include/python2.5'

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

