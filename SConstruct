import os
import sys

EnsureSConsVersion(0, 96, 93)

# --------------------------------------------------------------------------- #
#                              S E T U P                                      #
# --------------------------------------------------------------------------- #

# Add the buildfiles dir to the path
sys.path.insert(1, os.path.join(os.environ['RAM_SVN_DIR'],'buildfiles'))

# Options either come from command line of config file
opts = Options('configure.py')
opts.Add('CC', 'The C compiler to use','gcc')
opts.Add('CXX', 'The C++ compiler to use', 'g++')

# Setup the build environment
env = Environment(ENV=os.environ, options = opts)
Help(opts.GenerateHelpText(env))

# Add Root Includes Directory
env.Append(BUILD_DIR = os.path.join(env.Dir('.').abspath, 'build'))
env.Append(LIB_DIR = os.path.join(env['BUILD_DIR'], 'lib'))
env.Append(BIN_DIR = os.path.join(env['BUILD_DIR'], 'bin'))
env.Append(CPPPATH = [os.path.join(env.Dir('.').abspath, 'packages')])
env.Append(LIBPATH = [env['LIB_DIR']])

# --------------------------------------------------------------------------- #
#                              B U I L D                                      #
# --------------------------------------------------------------------------- #

# Our build subdirectory
buildDir = 'build'

from buildfiles import dirs_to_build
for directory in dirs_to_build.get_dirs():
    Export('env')
    
    # Build seperate directories (this calls our file in the sub directory)
    env.SConscript(os.path.join(directory, 'SConscript'), 
                   build_dir = os.path.join(buildDir, directory), 
                   duplicate = 0)


