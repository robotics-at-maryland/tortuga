import os
env = Environment(ENV = {'PATH' : os.environ['PATH']})
env.Append(CPPPATH = ['/opt/local/include', '/opt/local/include/ice', '#include', '#build/native'])
env.Append(LIBPATH = ['/opt/local/lib'])
env.Append(LIBS = ['Ice', 'IceUtil'])
env.BuildDir('build','src')

# Construct ICE stubs for C++
env.Command(['build/native/ram.cpp','build/native/ram.h'], 
	'ram.ice', 'slice2cpp --output-dir build/native $SOURCES')

# Construct ICE stubs for Python
env.Command('build/python/ram_ice.py', 
	'ram.ice', 'slice2py --output-dir build/python $SOURCES')

# Build the native ICE library
nativeIceLib = env.Library('build/native/ram.cpp')
env.Append(LIBS = [nativeIceLib])

env.Program('build/testserver.cpp')

env.Install('build/python','python/testclient.py')
