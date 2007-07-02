9# Copyright (C) 2007 Maryland Robotics Club
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
from common.util import run_shell_cmd


# Constants
if platform.system() == 'Darwin':
    BOOST_PYTHON_LIB = 'boost_python-1_35'
    BOOST_THREAD_LIB = 'boost_thread-1_35'
else:
    BOOST_PYTHON_LIB = 'boost_python-gcc'
    BOOST_THREAD_LIB = 'boost_thread-gcc-mt'

# --------------------------------------------------------------------------- #
#                        L I B R A R Y   I N F O                              #
# --------------------------------------------------------------------------- #

EXTERNAL_LIBS = None

def _get_external_lib(name):
    """
    Maps public library with the information need to build it
    """

    # This delays creation of these until after the module is loaded so the 
    # classes can be at the bottom of the file
    global EXTERNAL_LIBS
    if EXTERNAL_LIBS is None:
        EXTERNAL_LIBS = {
            'wxWidgets' : ConfigLibrary('wxWidgets', '2.8', ['wx/wx.h'], 
                                        'wx-config'),
            'OpenCV' : PkgConfigLibrary('opencv', '1.0', ['cv.h']),

            'GTK+ 2.0' : PkgConfigLibrary('gtk+-2.0', '2', ['gtk/gtk.h', 
                                                            'gdk/gdk.h']),
            'Boost' : BoostLibrary('Boost', (1,35), []),

            'USB': PkgConfigLibrary('libusb', '0.1', ['usb.h']),

            'Boost.Python' : BoostLibrary('Boost.Python', (1,35), [],
                                          [BOOST_PYTHON_LIB], 
                                          ext_deps = ['Python']),

            'Boost.Thread' : BoostLibrary('Boost.Thread', (1,35), [],
                                          [BOOST_THREAD_LIB]),

            'Python' : PythonLib('2.5')
            }

    if EXTERNAL_LIBS.has_key(name):
        return EXTERNAL_LIBS[name]
    else:
        print 'Could not find external library named: "%s"' % name
        sys.exit(1)

INTERNAL_LIBS = None

def _get_internal_lib(name):
    """
    Maps internal library name with the information needed to build it
    """
    # This delays creation of these until after the module is loaded so the 
    # classes can be at the bottom of the file
    global INTERNAL_LIBS
    if INTERNAL_LIBS is None:
        INTERNAL_LIBS = {
            'vision' : InternalLibrary('vision',
                                       int_deps = ['pattern', 'core'],
                                       ext_deps = ['OpenCV', 'Boost.Thread']),
            
            'pattern' : InternalLibrary('pattern', int_deps = [],
                                        ext_deps = ['Boost']),
            
            'core' : InternalLibrary('core', int_deps = [],
                                     ext_deps = ['Boost.Thread',
                                                 'Boost.Python']),
            
            'carnetix' : InternalLibrary('carnetix', int_deps = [],
                                         ext_deps = ['USB']),

            'imu' : InternalLibrary('imu', int_deps = [], ext_deps = []),

            'vehicle' : InternalLibrary('vehicle',
                                        int_deps = ['core', 'imu', 'pattern'],
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
    # Don't do anything if we are cleaning
    if not env.GetOption('clean'):
        _get_external_lib(name).setup_environment(env)

def add_internal(env, name):
    """
    Adds the flags need to build and link against one of our internal libraries
    """
    if not env.GetOption('clean'):
        _get_internal_lib(name).setup_environment(env)


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
        env.Append(CPPFLAGS = self.CPPFLAGS)
        env.Append(LINKFLAGS = self.LINKFLAGS)
        env.Append(CPPPATH = self.CPPPATH)

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
        if env['check'].lower() != 'yes':
            return False

        return True
     
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
            env.AppendUnique(LIBS = ['ram_' + self.name])

        self.setup_dependents(env)

        if self._adding_int_depends:
            print 'Error cycle in library dependencies for "%s"' % self.name
            print '\tDependents:',self.ext_deps
            sys.exit(1)

        # Setup dependent libraries if it isn't already done
        if self.int_deps is None:
            self.int_deps = [_get_internal_lib(lib) for lib in self._int_deps_name]
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
                error_cmd = 'Error running:', cmd
                self._config_cmd_output += ' '+ run_shell_cmd(cmd, error_cmd)
        
        # Merge actuall command output
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
                    
        version_str = run_shell_cmd(version_cmd, error_msg).strip()

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

 

class PythonLib(ConfigLibrary):
    def __init__(self, version):
        ConfigLibrary.__init__(self, 'Python', version, ['Python.h'],
                               'python2.5-config',
                               lib_flag = ' ; python2.5-config --ldflags',
                               version_flag = '--includes')

    def setup_environment(self, env):
        ConfigLibrary.setup_environment(self, env)

        # Here we have to remove and non-valid C++ flags
        cflags = env.get('CCFLAGS', [])
        if cflags.count('-Wstrict-prototypes'):
            cflags.remove('-Wstrict-prototypes')
        if cflags.count(('-isysroot', '/Developer/SDKs/MacOSX10.4u.sdk')):
            cflags.remove(('-isysroot', '/Developer/SDKs/MacOSX10.4u.sdk'))
        #print 'Python CC Flags', env['CCFLAGS']
        #print 'Python LD Flags', env['LINKFLAGS']
        env.Replace(CCFLAGS = cflags)

        # If we are on Mac, remove the special arch flags, we are going native
        ldflags = env.get('LINKFLAGS', [])
        if ldflags.count(('-arch', 'ppc')):
            ldflags.remove(('-arch', 'ppc'))
        if ldflags.count(('-arch', 'i386')):
            ldflags.remove(('-arch', 'i386'))
        if ldflags.count(('-isysroot', '/Developer/SDKs/MacOSX10.4u.sdk')):
            ldflags.remove(('-isysroot', '/Developer/SDKs/MacOSX10.4u.sdk'))

    def check_version(self, env):
        version_cmd = '%s %s' % (self.tool_name, self.version_flag)
        error_msg = 'Error executing "' + version_cmd + '", please make sure' \
                    '%s and %s are installed' % (self.name, self.tool_name)
                    
        version_str = run_shell_cmd(version_cmd, error_msg).strip()

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

        version_str = '%d_%d' % (self.major_ver, self.minor_ver)
        include_path = os.path.join(os.environ['RAM_ROOT_DIR'], 'include',

                                    'boost-' + version_str)
        # Currently check for boost libraries seem to fail
        # TODO: fix me
        #Library.__init__(self, name, version_str, headers, libraries,
        #                 CPPPATH = [include_path], ext_deps = ext_deps)
        linkflags = ' -l' + ' -l'.join(libraries)
        if len(libraries) == 0:
            linkflags = []
            
        Library.__init__(self, name, version_str, headers, [],
                         CPPPATH = [include_path], LINKFLAGS = linkflags,
                         ext_deps = ext_deps)
        
    def check_version(self, env):
        conf = env.Configure()

        ver_num = self.major_ver * 100000 + self.minor_ver * 1000 + \
                  self.patch_ver
        
        conf.TryCompile("""
        #include <boost/version.hpp>
        #if BOOST_VERSION > %(ver)s
        int main(void)
        #endif
        { return 0; }""" % {'ver' : ver_num}, 'cpp')
        
        env = conf.Finish()

