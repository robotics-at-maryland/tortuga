# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  buildfiles/libs.py

"""
This module allows easy inclusion of libraries with there required files.
"""

import os
import sys
import subprocess

# --------------------------------------------------------------------------- #
#                        L I B R A R Y   I N F O                              #
# --------------------------------------------------------------------------- #

def __get_lib_map():
    return {
    'wxWidgets' : ConfigLibrary('wxWidgets', '2.8', ['wx/wx.h'], 'wx-config'),
    'OpenCV' : PkgConfigLibrary('opencv', '1.0', ['cv.h']),
    'GTK+ 2.0' : PkgConfigLibrary('gtk+-2.0', '2', ['gtk/gtk.h', 'gdk/gdk.h']),
    'Boost' : BoostLibrary('Boost', (1,35), [])
    }

# --------------------------------------------------------------------------- #
#                     P U B L I C  I N T E R F A C E                          #
# --------------------------------------------------------------------------- #

def add_external(name, env):
    """
    Add an external library to the given environment and make sure its properly
    installed and configured.
    """
    # Don't do anything if we are cleaning
    lib_map = __get_lib_map()
    if not env.GetOption('clean'):
        if lib_map.has_key(name):
            lib_map[name].setup_environment(env)
        else:
            print 'Could not find library name: "%s"' % name
            sys.exit(1)


# --------------------------------------------------------------------------- #
#                      I M P L E M E N T A T I O N                            #
# --------------------------------------------------------------------------- #

class Library(object):
    def __init__(self, name, version, headers, libnames, CPPPATH = [],
                 CPPFLAGS = [], LINKFLAGS = [], strict_version = False):
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
        self.name = name
        self.version = version.strip()
        self.headers = headers
        self.libraries = libnames

        self.CPPPATH = CPPPATH
        self.LINKFLAGS = LINKFLAGS
        self.CPPFLAGS = CPPFLAGS
        self.strict_version = strict_version

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

        self.check_environment(env)

    def check_environment(self, env):
        self.check_version(env)
        self.check_headers(env)
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
                print '\tCould not find the "%s" header' % header
                print '\tPlease make sure %s is installed properly\n' % self.name
                sys.exit(1)
        
        env = conf.Finish()

    def add_libs(self, env):
        """
        Check the validity of any libraries here if desired and add them after
        a succesfull check.
        """
        conf = env.Configure()

        for lib in self.libraries:
            if not conf.CheckLib(lib, language='C++', autoadd=1):
                print '\nERROR:'
                print '\tCould not link to the "%s" library' % lib
                print '\tPlease make sure %s is installed properly\n' % self.name
                sys.exit(1)
        
        env = conf.Finish()


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

    def setup_environment(self, env):
        """
        This runs the config tool and merges its result flags into the given
        environment.  It will then check library version and headers.
        """
        old_libs = set(env.get('LIBS', []))
        setup_cmd = '!%s %s %s' % (self.tool_name, self.cflag, self.libflag)
        env.MergeFlags([setup_cmd])

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

class BoostLibrary(Library):
    def __init__(self, name, version, headers, libraries = []):
        """
        @type  version: (numbers)
        @param version: A tuple of major, minor and patch version numbers.  If
                        the lower, minor or patch numbers aren't included, they
                        are not checked.

        @type  libraries: [string]
        @param libaries: A list of the boost libraries that this library
                         instance represents.
        """
    def __init__(self, name, version, headers, libraries = []):

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
                                    'boost_' + version_str)
        
        Library.__init__(self, name, version_str, headers, libraries,
                         CPPPATH = [include_path])
        
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
    
def run_shell_cmd(command, error_msg):
    """
    Run the given shell command return the stdout result
    """
    if type(command) is not list:
        command = command.split()
    try:
        proc = subprocess.Popen(command, stdout=subprocess.PIPE)
        return proc.communicate()[0]
    except OSError:
        print error_msg
        sys.exit(1)
