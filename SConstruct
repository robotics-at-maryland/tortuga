import os
import sys

EnsureSConsVersion(0, 96, 93)

# --------------------------------------------------------------------------- #
#                              S E T U P                                      #
# --------------------------------------------------------------------------- #

# Add the buildfiles dir to the path
sys.path.insert(1, os.path.join(os.environ['RAM_SVN_DIR'],'buildfiles'))
import helpers

# Options either come from command line of config file
opts = Options('configure.py')
opts.AddOptions(
     ('CC', 'The C compiler to use','gcc'),
     ('CXX', 'The C++ compiler to use', 'g++'),
     ('check', 'Runs checks on dependent libraries to ensure a proper installation',
      'yes'),
     BoolOption('verbose', "Shows full command line build, normally recored to 'build.log'",
                False)
     )

# Setup the build environment
tpath =  os.path.join(os.environ['RAM_SVN_DIR'],'buildfiles', 'tools')
env = Environment(ENV=os.environ, options=opts, toolpath = [tpath])
Help(opts.GenerateHelpText(env))

# Add Root Includes Directory
env.Append(BUILD_DIR = os.path.join(env.Dir('.').abspath, 'build'))
env.Append(PACKAGE_DIR = [os.path.join(env.Dir('.').abspath, 'packages')])
env.Append(LIB_DIR = os.path.join(env['BUILD_DIR'], 'lib'))
env.Append(BIN_DIR = os.path.join(env['BUILD_DIR'], 'bin'))
env.AppendUnique(CPPPATH = [env['PACKAGE_DIR']])
env.AppendUnique(LIBPATH = [env['LIB_DIR'],
                            os.path.join(os.environ['RAM_ROOT_DIR'],'lib')])

# Setup printing
def print_cmd_line(s, target, src, env):
    if env['verbose']:
        sys.stdout.write("%s\n"%s);
    else:
          sys.stdout.write("Making: %s...\n" %(' and '.join([str(x) for x in
                                                             target])))
          # Save real cmd to log file
          open(env['CMD_LOGFILE'], 'a').write("%s\n"%s)

          
env['PRINT_CMD_LINE_FUNC'] = print_cmd_line
env['CMD_LOGFILE'] = 'build.log'

# Add debug flags
env.AppendUnique(CCFLAGS = ['-g', '-Wall','-Werror'])

# Add out helper functions to the environment
helpers.add_helpers_to_env(env)

# --------------------------------------------------------------------------- #
#                              B U I L D                                      #
# --------------------------------------------------------------------------- #

# Our build subdirectory
buildDir = 'build'

from buildfiles import dirs_to_build
for directory in dirs_to_build.get_dirs():
    Export('env')

    print 'Dir:',os.path.join(buildDir,directory)
    # Build seperate directories (this calls our file in the sub directory)
    env.SConscript(os.path.join(directory, 'SConscript'), 
                   build_dir = os.path.join(buildDir, directory), 
                   duplicate = 0)


