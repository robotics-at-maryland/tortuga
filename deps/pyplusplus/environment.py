import os
import sys
import getpass
import platform

class boost:
    libs = ''
    include = ''
    
class python:
    libs = ''
    include = ''

class gccxml:
    executable = ''

class scons:
    compiler = ''
    suffix = ''
    cmd_build = ''
    cmd_clean = ''
    ccflags = []

# Setup the MRBC
scons.cmd_build = 'scons --file=%s'
scons.cmd_clean = 'scons --clean --file=%s'

if platform.system() == 'Linux':
    prefix = os.path.join(os.environ['RAM_ROOT_DIR'])
    lib = os.path.join(prefix, 'lib')
    include = os.path.join(prefix, 'include')
    
    scons.suffix = '.so'
    scons.compiler = 'g++'
    
    boost.libs = [lib]
    boost.include = os.path.join(include, 'boost-1_34_1')
    python.include = '/usr/include/python2.5'
    gccxml.executable = os.path.join(prefix, 'bin','gccxml')
else:
    raise "Environment Not Configured"


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

