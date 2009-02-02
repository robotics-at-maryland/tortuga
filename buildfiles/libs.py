# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  buildfiles/libs.py

"""
This module allows easy inclusion of libraries with there required files.
"""

# Python imports
import os
import sys
import subprocess
import platform

# Project Imports
from buildfiles.common.util import run_shell_cmd

# Constants
if platform.system() == 'Darwin':
    BOOST_PYTHON_LIB = 'boost_python-xgcc40-mt'
    BOOST_THREAD_LIB = 'boost_thread-xgcc40-mt'
    BOOST_SIGNALS_LIB = 'boost_signals-xgcc40-mt'
    BOOST_GRAPH_LIB = 'boost_graph-xgcc40-mt'
    BOOST_FILESYSTEM_LIB = 'boost_filesystem-xgcc40-mt'
    BOOST_SYSTEM_LIB = 'boost_system-xgcc40-mt'
    BOOST_PROGOPT_LIB = 'boost_program_options-xgcc40-mt'
    BOOST_REGEX_LIB = 'boost_regex-xgcc40-mt'
    BOOST_DATE_TIME_LIB = 'boost_date_time-xgcc40-mt'
elif platform.system() == 'Linux':
    BOOST_PYTHON_LIB = 'boost_python-gcc42-mt'
    BOOST_THREAD_LIB = 'boost_thread-gcc42-mt'
    BOOST_SIGNALS_LIB = 'boost_signals-gcc42-mt'
    BOOST_GRAPH_LIB = 'boost_graph-gcc42-mt'
    BOOST_FILESYSTEM_LIB = 'boost_filesystem-gcc42-mt'
    BOOST_SYSTEM_LIB = 'boost_system-gcc42-mt'
    BOOST_PROGOPT_LIB = 'boost_program_options-gcc42-mt'
    BOOST_REGEX_LIB = 'boost_regex-gcc42-mt'
    BOOST_DATE_TIME_LIB = 'boost_date_time-gcc42-mt'
elif platform.system() == 'Windows' or platform.system() == 'Microsoft':
    BOOST_PYTHON_LIB = 'boost_python-vc80-mt-1_34_1'
    BOOST_THREAD_LIB = 'boost_thread-vc80-mt-1_34_1'
    BOOST_SIGNALS_LIB = 'boost_signals-vc80-mt-1_34_1'
    BOOST_GRAPH_LIB = 'boost_graph-vc80-mt-1_34_1'
    BOOST_FILESYSTEM_LIB = 'boost_filesystem-vc80-mt-1_34_1'
    BOOST_PROGOPT_LIB = 'boost_program_options-vc80-mt-1_34_1'
    BOOST_REGEX_LIB = 'boost_regex-vc80-mt-1_34_1'
    BOOST_DATE_TIME_LIB = 'boost_date_time-vc80-mt-1_34_1'
else:
    print '"%s" is an unsupported platform' % platform.system()
    sys.exit(1)

def remove_item(env, key, items):
    """
    Remove given items from the array stored at the key in the given
    environment. It returns the list of removed items.
    """

    removed = []
    if type(items) is not list:
        items = [items]

    # Grab current set of values
    values = env.get(key,[])

    # Remove the items if they are presnet
    for item in items:
        if values.count(item):
            values.remove(item)
            removed.append(item)

    # Reset the value
    args = {key : values}
    env.Replace(**args)

    return removed


# --------------------------------------------------------------------------- #
#                        L I B R A R Y   I N F O                              #
# --------------------------------------------------------------------------- #

EXTERNAL_LIBS = None

def setup_posix_libs():
    global EXTERNAL_LIBS
    EXTERNAL_LIBS = {
        'wxWidgets' : ConfigLibrary('wxWidgets', '2.8', ['wx/wx.h'], 
                                    'wx-config', lib_flag='--libs std,gl'),
        'OpenCV' : PkgConfigLibrary('opencv', '1.0', ['cv.h']),
        
        'log4cpp' : PkgConfigLibrary('log4cpp', '1.0', ['log4cpp/Export.hh']),

        'GTK+ 2.0' : PkgConfigLibrary('gtk+-2.0', '2', ['gtk/gtk.h', 
                                                        'gdk/gdk.h']),
        'Boost' : BoostLibrary('Boost', (1,37,None), []),

        'USB': PkgConfigLibrary('libusb', '0.1', ['usb.h']),

        'Boost.Python' : BoostLibrary('Boost.Python', (1,37,None), [],
                                      [BOOST_PYTHON_LIB], 
                                      ext_deps = ['Python']),

        'Boost.Thread' : BoostLibrary('Boost.Thread', (1,37,None), [],
                                      [BOOST_THREAD_LIB]),

        'Boost.Signals' : BoostLibrary('Boost.Signals', (1,37,None), [],
                                      [BOOST_SIGNALS_LIB]),
        
        'Boost.Graph' : BoostLibrary('Boost.Graph', (1,37,None), [],
                                       [BOOST_GRAPH_LIB]),
        
        'Boost.Filesystem' : BoostLibrary('Boost.Filesystem', (1,37,None), [],
                                          [BOOST_FILESYSTEM_LIB]),

        'Boost.System' : BoostLibrary('Boost.System', (1,37,None), [],
                                          [BOOST_SYSTEM_LIB]),

        'Boost.ProgramOptions' : BoostLibrary('Boost.ProgramOptions',
                                              (1,37,None), [],
                                             [BOOST_PROGOPT_LIB]),

        'Boost.Regex' : BoostLibrary('Boost.Regex', (1,37,None), [],
                                     [BOOST_REGEX_LIB]),

        'Boost.DateTime' : BoostLibrary('Boost.DateTime', (1,37,None), [],
                                     [BOOST_DATE_TIME_LIB]),
        
        'Python' : PythonLib('2.5'),

        'UnitTest++' : PkgConfigLibrary('UnitTest++', '1.3',
                                        ['UnitTest++/UnitTest++.h'])
        }

def setup_windows_libs():
    global EXTERNAL_LIBS
    from distutils import sysconfig
    getvar = sysconfig.get_config_var
    
    python_lib_path = os.path.join(sysconfig.get_config_var('prefix'),'libs')
    
    ram_root = os.environ['RAM_ROOT_DIR']
    ram_include = ram_root + '/include'
    
    EXTERNAL_LIBS = {
        'UnitTest++' : Library('UnitTest++', '1.3', ['UnitTest++/UnitTest++.h'],
                               'UnitTest++.vsnet2005'),   
                               
        'Boost' : Library('Boost', '1.34.1', ['boost/config.hpp'], [], 
                          CPPPATH = ram_include + '/boost-1_34_1',
                          CPPFLAGS = ['/wd4668', '/wd4365', '/wd4619']),
                          
        'Boost.Python' : Library('Boost.Python', '1.34.1',
                                 ['boost/python.hpp'], [BOOST_PYTHON_LIB], 
                                 ext_deps = ['Python', 'Boost']),
                                      
        'Boost.Thread' : Library('Boost.Thread', '1.34.1',
                                 ['boost/thread.hpp'], [BOOST_THREAD_LIB], 
                                 ext_deps = ['Boost']),
                                      
		'Boost.Signals' : Library('Boost.Signals', '1.34.1',
                                 [], [BOOST_SIGNALS_LIB], 
                                 ext_deps = ['Boost']),
                                 
        'Boost.Filesystem' : Library('Boost.Filesystem', '1.34.1',
                                 [], [BOOST_FILESYSTEM_LIB], 
                                 ext_deps = ['Boost']),

        'Boost.Regex' : Library('Boost.Regex', '1.34.1',
                                [], [BOOST_REGEX_LIB],
                                ext_deps = ['Boost']),

                                 
        'Boost.ProgramOptions' : Library('Boost.ProgramOptions','1.34.1', [],
                                         [BOOST_PROGOPT_LIB]),
                                         
        'Boost.DateTime' : Library('Boost.DateTime','1.34.1', [],
                                         [BOOST_DATE_TIME_LIB]),
        
                                 
        'Python' : Library('Python', '2.5', ['Python.h'],
                           'python25', CPPPATH = sysconfig.get_python_inc(),
                           LINKFLAGS = ['/LIBPATH:'+python_lib_path],
                           CPPFLAGS = ['/wd4826']),
        
        'OpenCV' : Library('OpenCV', '1.0', ['opencv/cv.h'],
                           ['cv', 'cxcore','highgui','cxts','cvaux','ml'],
                           CPPPATH = ram_include + '/opencv')
    }                               
        
def _get_external_lib(name):
    """
    Maps public library with the information need to build it
    """

    # This delays creation of these until after the module is loaded so the 
    # classes can be at the bottom of the file
    global EXTERNAL_LIBS
    if EXTERNAL_LIBS is None:
        if os.name == 'posix':
            setup_posix_libs()
        else:
            setup_windows_libs()

    if EXTERNAL_LIBS.has_key(name):
        return EXTERNAL_LIBS[name]
    else:
        print 'Could not find external library named: "%s"' % name
        sys.exit(1)

INTERNAL_LIBS = None

def _get_internal_lib(env, name):
    """
    Maps internal library name with the information needed to build it
    """
    vehicle_int_deps = ['core', 'pattern','math']
    if env.HasFeature('drivers'):
        vehicle_int_deps.extend(['imu', 'carnetix', 'sensor', 'thruster'])
    
    # This delays creation of these until after the module is loaded so the 
    # classes can be at the bottom of the file
    global INTERNAL_LIBS
    if INTERNAL_LIBS is None:
        INTERNAL_LIBS = {
            'vision' : InternalLibrary('vision',
                                       int_deps = ['pattern', 'core', 'math'],
                                       ext_deps = ['OpenCV', 'Boost.Thread']),
            
            'pattern' : InternalLibrary('pattern', int_deps = [],
                                        ext_deps = ['Boost', 'Boost.Thread']),
            
            'core' : InternalLibrary('core', int_deps = [],
                                     ext_deps = ['Boost.Thread',
                                                 'Boost.Python',
                                                 'Boost.Signals',
                                                 'Boost.Filesystem',
                                                 'Boost.DateTime',
                                                 'Boost.System',
                                                 'log4cpp']),
            
            'carnetix' : InternalLibrary('carnetix', int_deps = [],
                                         ext_deps = ['USB']),

            'imu' : InternalLibrary('imu', int_deps = [], ext_deps = []),

            'sensor' : InternalLibrary('sensor', int_deps = [],
                                       ext_deps = []),
            
            'thruster' : InternalLibrary('thruster', int_deps = [],
                                         ext_deps = []),

            'bfin_spartan' : InternalLibrary('bfin_spartan', int_deps = [],
                                             ext_deps = []),

            'math' : InternalLibrary('math', int_deps = [],
                                     ext_deps = []),

            'sonar' : InternalLibrary('sonar',
                                      int_deps = ['math', 'bfin_spartan'],
                                      ext_deps = ['Boost']),

            'network' : InternalLibrary('network', int_deps = ['core'],
                                        ext_deps = []),

            'control' : InternalLibrary('control',
                                        int_deps = ['math', 'core', 'vehicle'],
                                        ext_deps = []),

            'vehicle' : InternalLibrary('vehicle',
                                        int_deps = vehicle_int_deps,
                                        ext_deps = [])
            }

    if INTERNAL_LIBS.has_key(name):
        return INTERNAL_LIBS[name]
    else:
        print 'Could not find internal library named: "%s"' % name
        print 'Please update "_get_instal_lib" in "buildfiles/libs.py"'
        sys.exit(1)

# --------------------------------------------------------------------------- #
#                     P U B L I C  I N T E R F A C E                          #
# --------------------------------------------------------------------------- #

def add_external(env, name):
    """
    Add an external library to the given environment and make sure its properly
    installed and configured.
    """
    if not env.GetOption('clean'):
        _get_external_lib(name).setup_environment(env)
    remove_item(env, 'CPPDEFINES', 'NDEBUG')

def add_internal(env, name):
    """
    Adds the flags need to build and link against one of our internal libraries
    """
    if not env.GetOption('clean'):
        _get_internal_lib(env, name).setup_environment(env)


# --------------------------------------------------------------------------- #
#                      I M P L E M E N T A T I O N                            #
# --------------------------------------------------------------------------- #

class Library(object):
    # Allows us to skip checking if we have already done it once with the 
    # scons run
    CHECKED_LIBS = set()

    def __init__(self, name, version, headers, libnames, CPPPATH = None,
                 CPPFLAGS = None, LINKFLAGS = None, strict_version = False,
                 ext_deps = None):
        """
        @type  name: string
        @param name: The name of the library

        @type  version: string
        @param name: The version string of the library ie: '2.6.20'

        @type  headers: [string]
        @param headers: A list of the headers you wish to ensure exist in order
                        for the library to considered installed and working.

        @type  strict_version: Boolean
        @param strict_version: If true the given version string and the
                               recieved one must match exactly, otherwise they
                               recieved string must start with the given string.
        """
        if CPPPATH is None: CPPPATH = []
        if CPPFLAGS is None: CPPFLAGS = []
        if LINKFLAGS is None: LINKFLAGS = []
        if ext_deps is None: ext_deps = []
        
        self.name = name
        self.version = version.strip()
        self.headers = headers
        self.libraries = libnames

        self.CPPPATH = CPPPATH
        self.LINKFLAGS = LINKFLAGS
        self.CPPFLAGS = CPPFLAGS
        self.strict_version = strict_version

        # Pervent basic errors with cycles
        if ext_deps.count(self.name) != 0:
            print 'Error, library: "%s" cannot depend on its self' % self.name
            sys.exit(1)

        # Store names for loading apon first request
        self._ext_deps_name = ext_deps
        self.ext_deps = None
        
        # This is here to prevent a recursion error
        self._adding_ext_depends = False
        
    def setup_environment(self, env):
        """
        This merges in the libraries to flag to the given environment and then
        calls check_environment to make everything then
        preforms the required checks to make sure the library is properly
        installed and has the right version.
        """
#        print 'LINK',self.LINKFLAGS
        env.AppendUnique(CPPFLAGS = self.CPPFLAGS)
        env.AppendUnique(LINKFLAGS = self.LINKFLAGS)
        env.AppendUnique(CPPPATH = self.CPPPATH)
#        print 'AFTER:',env['LINKFLAGS']
    
        self.setup_dependents(env)
        
        self.check_environment(env)

    def setup_dependents(self, env):
        """
        Brings in settings for dependent libraries
        """

        if self._adding_ext_depends:
            print 'Error cycle in library dependencies for "%s"' % self.name
            print '\tDependents:',self.ext_deps
            sys.exit(1)

        # Setup dependent libraries if it isn't already done
        if self.ext_deps is None:
            self.ext_deps = [_get_external_lib(lib) for lib in self._ext_deps_name]

        self._adding_ext_depends = True
        for lib in self.ext_deps:
            lib.setup_environment(env)
        self._adding_ext_depends = False

    def check_environment(self, env):
        # Only perform the check once and don't do it we are cleaning a build
        if self.do_check(env):
            self.check_version(env)
            self.check_headers(env)
            
            # Record the fact that we have been run
            Library.CHECKED_LIBS.add(self)

        # Add libraries does its own check
        self.add_libs(env)
  

    def check_version(self, env):
        """
        Override this in a subclass to check library versions.
        """
        pass

    def check_headers(self, env):
        """
        Uses the scons functionality to make sure that the headers for the given
        library exist
        """
        conf = env.Configure()

        for header in self.headers:
            if not conf.CheckCXXHeader(header, '<>'):
                print '\nERROR:'
                print '\tCould not find the "%s" header:' % header
                print '\tHeader Search Path:'
                for path in env['CPPPATH']:
                    print '\t\t',path
                print '\n\tPlease make sure %s is installed properly\n' % self.name
                sys.exit(1)
        
        env = conf.Finish()

    def add_libs(self, env):
        """
        Check the validity of any libraries here if desired and add them after
        a succesfull check.
        """

        if self.do_check(env):
            # Create a special test environment without any of our libraries
            libs = env.get('LIBS', [])
            external_libs = [l for l in libs if not l.startswith('ram_')]
            internal_libs = [l for l in libs  if l.startswith('ram_')]
            env.Replace(LIBS = external_libs)
            
            conf = env.Configure()

            self.libraries.reverse()
            for lib in self.libraries:
                if not conf.CheckLib(lib, language='C++', autoadd=1):
                    print '\nERROR:'
                    print '\tCould not link to the "%s" library on:' % lib
                    print '\tLibrary Search Path:'
                    for path in env['LIBPATH']:
                        print '\t\t',path
                    print '\tLinker flags:',env.subst(' '.join(env['LINKFLAGS']))
                    print '\tOther libraries:',env.subst(' '.join(env['LIBS']))
                    print '\n\tPlease make sure %s is installed properly\n' % self.name
                    sys.exit(1)

            # Add back in internal libs
            env.AppendUnique(LIBS =  internal_libs)
            env = conf.Finish()

            # Place self in CHECKED_LIBS
            Library.CHECKED_LIBS.add(self)
        else:
            if 0 != len(self.libraries):
                env.AppendUnique(LIBS = self.libraries)
            
    def do_check(self, env):
        """
        Whether or not to perform the configure type checks on the libraries
        """
         
        if self in Library.CHECKED_LIBS:
            return False
        if env.GetOption('clean'):
            return False
        if not env['check']:
            return False

        return True
     
# class WindowsLibrary(object):
    # def __init__(self, name, version, headers, libnames, CPPPATH = None,
                 # CPPFLAGS = None, LINKFLAGS = None, strict_version = False,
                 # ext_deps = None):
        # Library.__init__(self, name, version, headers, libnames, 
                         # CPPPATH = CPPPATH, LINKFLAGS = LINKFLAGS, 
                         # ext_deps = ext_deps)
                 
    # def setup_environment(self, env):
        # """
        # This merges in the libraries to flag to the given environment and then
        # calls check_environment to make everything then
        # preforms the required checks to make sure the library is properly
        # installed and has the right version.
        # """
#       print 'LINK',self.LINKFLAGS
#        Slight hack Because we control the windows deps
        # env.AppendUnique(CPPPATH = os.path.join(os.environ['RAM_ROOT_DIR'],'include'))
        # env.AppendUnique(LIBPATH = os.path.join(os.enviorn['RAM_ROOT_DIR'],'lib'))
        
        # env.AppendUnique(CPPFLAGS = self.CPPFLAGS)
        # env.AppendUnique(LINKFLAGS = self.LINKFLAGS)
        # env.AppendUnique(CPPPATH = self.CPPPATH)
#       print 'AFTER:',env['LINKFLAGS']
    
        # self.setup_dependents(env)
        
        # self.check_environment(env)
     
class InternalLibrary(Library):
    def __init__(self, name, int_deps, ext_deps, strict_version = False,
                 libraries = None):
        """
        This allows easy inclusion of internal libraries, it automatically pulls
        in all needed settings for libraries it dependends on.

        @todo Maybe move dep. stuff into main library class as keyword args
        @todo Maybe include CPPPATH, etc in this class
        
        See Library.__init__ for the rest of the parameters
        
        @type  int_deps: list of strings
        @param int_deps: Names of internal libraries this library depends on.

        @type  ext_deps: list of strings
        @paraq ext_deps: Names of external libraries this library depends on.
        """
        if libraries is None: libraries = []
        
        Library.__init__(self, name, '', [], libraries,
                         strict_version = strict_version,
                         ext_deps = ext_deps)

        
        # Pervent basic errors with cycles
        if int_deps.count(self.name) != 0:
            print 'Error, library: "%s" cannot depend on its self' % self.name
            sys.exit(1)

        # Store names for loading apon first request
        self._int_deps_name = int_deps
        self.int_deps = None
        
        # This is here to prevent a recursion error
        self._adding_int_depends = False

    def setup_environment(self, env, building_self = False):
        # Include self in library list
        if not building_self:
            variant_suffix = env['VARIANT'].suffix
            env.AppendUnique(LIBS = ['ram_' + self.name + variant_suffix])

        self.setup_dependents(env)

        if self._adding_int_depends:
            print 'Error cycle in library dependencies for "%s"' % self.name
            print '\tDependents:',self.ext_deps
            sys.exit(1)

        # Setup dependent libraries if it isn't already done
        if self.int_deps is None:
            self.int_deps = [_get_internal_lib(env, lib) for lib in self._int_deps_name]
        # Setup the environment for out internal dependedcies
        self._adding_int_depends = True
        for lib in self.int_deps:
            lib.setup_environment(env)
        self._adding_int_depends = False

        # Add information for all dependents
        for lib in self.int_deps:
            lib.setup_environment(env)

        # Add my libaries
        self.add_libs(env);

class ConfigLibrary(Library):
    """
    This class alows library flags to found using tools like wx-config or
    pkg-config.
    """
    
    def __init__(self, name, version, headers, config_tool_name,
                 version_flag = '--version',
                 compiler_flag = '--cflags',
                 lib_flag = '--libs',
                 strict_version = False):
        """
        See Library.__init__ for the rest of the parameters
        
        @type  config_tool: string
        @param config_tool: the name of the configuration tool used for the
                            library, ie: 'pkg-config'.

        @type  version_flag: string
        @param version_flag: The flag to pass the tool when you want the
                             libraries currently installed version

        @type  compiler_flag: string
        @param compiler_flag: The flag to pass the tool when you want the the 
                              which returns the compiler flags needed to use
                              this library.

        @type  lib_flag: string
        @param lib_flag: The flag to pass the tool when you want the the
                         which returns the linker flags needed to this
                         library.
        """
        Library.__init__(self, name, version, headers, [],
                         strict_version = strict_version)
        
        self.tool_name = config_tool_name
        self.version_flag = version_flag
        self.cflag = compiler_flag
        self.libflag = lib_flag

        self._config_cmd_output = None

    def setup_environment(self, env):
        """
        This runs the config tool and merges its result flags into the given
        environment.  It will then check library version and headers.
        """
        old_libs = set(env.get('LIBS', []))

        # Cache the run of the shell command
        if self._config_cmd_output is None:
            setup_cmd = '%s %s %s' % (self.tool_name, self.cflag, self.libflag)
            self._config_cmd_output = ' '
            
            for cmd in setup_cmd.split(';'):
                error_cmd = 'Error running: ' + cmd
                result, returncode = run_shell_cmd(cmd, error_cmd)

                if returncode != 0:
                    print "\tError:"
                    print "\tCould not find '%s' with tool: '%s'" \
                          % (self.name, self.tool_name)
                    sys.exit(1)
                
                self._config_cmd_output += ' '+ result
        
        # Merge actuall command output
        # Stip out flags we don't "like"
        flagsToRemove = ['-O2', '-O3', '-O0', '-DNDEBUG', '-g']
        for flag in flagsToRemove:
            self._config_cmd_output = self._config_cmd_output.replace(flag, '')
        
        env.MergeFlags([self._config_cmd_output])

        # Make sure settings for dependent libraries are set
        self.setup_dependents(env)

        # Determine which libraries were added so we can check that they
        # link properly
        # Disabled because this is a bit excessive
        # TODO: Find a way to turn on and off this mode
        #self.libraries = [lib for lib in set(env['LIBS']).difference(old_libs)]

        self.check_environment(env)
        
    def check_version(self, env):
        """
        Ensures we have the version of the library that is desired
        """
        version_cmd = '%s %s' % (self.tool_name, self.version_flag)
        error_msg = 'Error executing "' + version_cmd + '", please make sure' \
                    '%s and %s are installed' % (self.name, self.tool_name)
                    
        version_str, return_code = run_shell_cmd(version_cmd, error_msg)
        version_str = version_str.strip()

        correct_version = False
        if self.strict_version and (self.version == version_str):
            correct_version = True
        elif not self.strict_version and version_str.startswith(self.version):
            correct_version = True

        if not correct_version:
            print 'Need version: %s of %s, not version: %s' % \
                  (self.version, self.name, version_str)
            sys.exit(1)

class PkgConfigLibrary(ConfigLibrary):
    """
    Provides a specialization of the generic config class to make pkg-config
    compatible libraries easier to add.
    """
    def __init__(self, name, version, headers, strict_version = False):
        ConfigLibrary.__init__(self, name, version, headers,
                               'pkg-config ' + name,'--modversion',
                               strict_version = strict_version)

 
PYTHON_CONFIG = os.path.join(os.environ['RAM_SVN_DIR'], 'scripts',
                             'python2.5-config')
class PythonLib(ConfigLibrary):
    def __init__(self, version):
        ConfigLibrary.__init__(self, 'Python', version, ['Python.h'],
                               'python ' + PYTHON_CONFIG,
                               lib_flag = ' ; python ' + PYTHON_CONFIG + ' --ldflags',
                               version_flag = '--includes')

    def setup_environment(self, env):
#        removed = remove_item(env, 'CCFLAGS', ['-Wall','-Werror'])

        includes, return_code = \
            run_shell_cmd(self.tool_name + ' --includes', 
                          "Could not execute python2.5-config")

        ldflags, return_code = \
            run_shell_cmd(self.tool_name + ' --ldflags', 
                          "Could not execute python2.5-config")
 
        env.MergeFlags([ldflags, includes, '-DBOOST_PYTHON_NO_PY_SIGNATURES'])
#        ConfigLibrary.setup_environment(self, env)
#        env.Append(CCFLAGS = removed)

        # Here we have to remove and non-valid C++ flags
#        remove_item(env, 'CCFLAGS', ['-DNDEBUG',
#                                      '-Wstrict-prototypes',
#                                     ('-isysroot',
#                                      '/Developer/SDKs/MacOSX10.4u.sdk')])

        # If we are on Mac, remove the special arch flags, we are going native
#        remove_item(env, 'LINKFLAGS', [('-arch', 'ppc'), ('-arch', 'i386'),
#                                       ('-isysroot',
#                                        '/Developer/SDKs/MacOSX10.4u.sdk')])
#        remove_item(env, 'CPPDEFINES', 'NDEBUG')

        

    def check_version(self, env):
        version_cmd = '%s %s' % (self.tool_name, self.version_flag)
        error_msg = 'Error executing "' + version_cmd + '", please make sure' \
                    '%s and %s are installed' % (self.name, self.tool_name)
                    
        version_str, return_code = run_shell_cmd(version_cmd, error_msg)
        version_str = version_str.strip()

        correct_version = False
        if version_str.endswith(self.version):
            correct_version = True

        if not correct_version:
            print 'Need version: %s of %s, not version: %s' % \
                  (self.version, self.name, version_str)
            sys.exit(1)

class BoostLibrary(Library):
    def __init__(self, name, version, headers, libraries = None,
                 ext_deps = None):
        """
        @type  version: (numbers)
        @param version: A tuple of major, minor and patch version numbers.  If
                        the lower, minor or patch numbers aren't included, they
                        are not checked.

        @type  libraries: [string]
        @param libaries: A list of the boost libraries that this library
                         instance represents.
        """
        if libraries is None: libraries = []
        if ext_deps is None: ext_deps = []
        
        # Determine Major, Minor and Patch versions from given tuple
        assert (len(version) == 3) or (len(version) == 2)

        self.major_ver = version[0]
        self.minor_ver = version[1]

        if len(version) > 2:
            self.patch_ver = version[2]
        else:
            self.patch_ver = 0

        # Generate the flags to include the needed libraries
        if type(libraries) is not list:
            self.libraries = [libraries]
        else:
            self.libraries = libraries

        if self.patch_ver is None:
            version_str = '%d_%d' % (self.major_ver, self.minor_ver)
        else:
            version_str = '%d_%d_%d' % (self.major_ver, self.minor_ver,
                                        self.patch_ver)
        include_path = os.path.join(os.environ['RAM_ROOT_DIR'], 'include',

                                    'boost-' + version_str)
        # Currently check for boost libraries seem to fail
        # TODO: fix me
        #Library.__init__(self, name, version_str, headers, libraries,
        #                 CPPPATH = [include_path], ext_deps = ext_deps)
        linkflags = ' -l' + ' -l'.join(libraries)
        linkflags = linkflags.split()
        if len(libraries) == 0:
            linkflags = []
            
        Library.__init__(self, name, version_str, headers, [],
                         CPPPATH = [include_path], LINKFLAGS = linkflags,
                         ext_deps = ext_deps)
        
    def check_version(self, env):
        conf = env.Configure()

        patch_ver = self.patch_ver
        if patch_ver is None:
            patch_ver = 0
        ver_num = self.major_ver * 100000 + self.minor_ver * 1000 + patch_ver
        
        conf.TryCompile("""
        #include <boost/version.hpp>
        #if BOOST_VERSION > %(ver)s
        int main(void)
        #endif
        { return 0; }""" % {'ver' : ver_num}, 'cpp')
        
        env = conf.Finish()

