import os
import os.path

EnsureSConsVersion(0, 96, 93)

# --------------------------------------------------------------------------- #
#                              S E T U P                                      #
# --------------------------------------------------------------------------- #

# Options either come from command line of config file
opts = Options('configure.py')
opts.Add('CC', 'The C compiler to use','gcc-4.0')
opts.Add('CXX', 'The C++ compiler to use', 'g++-4.0')

# Setup the build environment
env = Environment(ENV=os.environ, options = opts)
Help(opts.GenerateHelpText(env))



# --------------------------------------------------------------------------- #
#                              B U I L D                                      #
# --------------------------------------------------------------------------- #
install = False

dirs_to_build = [os.path.join('packages','extensions', 'wxogre')]

# Our build subdirectory
buildDir = 'build'

for directory in dirs_to_build:
    # Pass down which directory the script is building from
    env['base_dir'] = os.path.abspath(directory)
    Export('env')
    
    # Build seperate directories (this calls our file in the sub directory)
    env.SConscript(os.path.join(directory, 'SConscript'), 
                   build_dir = os.path.join(buildDir, directory), 
                   duplicate = 0)


# --------------------------------------------------------------------------- #
#                             I N S T A L L                                   #
# --------------------------------------------------------------------------- #

if install:
    # Handle the install
    header_dir = 'OgreNewt_Main/inc'
    headers = [os.path.join(header_dir,f) for f in os.listdir(header_dir) 
               if f.endswith('.h')]
    library = os.path.join(buildDir, 'OgreNewt_Main', 'libOgreNewt.so')
    
    # This creats a  'install' target which copies in the files
    header_dir = os.path.join(env['prefix'], 'include', 'OgreNewt')
    lib_dir = os.path.join(env['prefix'], 'lib')
    

    env.Alias('install', env.Install(dir = header_dir, 
                                     source = headers))
    env.Alias('install', env.Install(dir = lib_dir,
                                     source = library))
