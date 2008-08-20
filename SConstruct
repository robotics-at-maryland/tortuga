import os

# Import path from environment
env = Environment(ENV = {'PATH' : os.environ['PATH']})

env.Append(FRAMEWORKS = ['OpenGL', 'GLUT'])
env.Append(CPPPATH = ['/opt/local/include', '/opt/local/include/ice', '#include', '#build/native'])
env.Append(LIBPATH = ['/opt/local/lib'])
env.Append(LIBS = ['Ice', 'IceUtil'])
env.BuildDir('build','src')

# Construct ICE stubs for C++
env.Command(['build/native/ram.cpp','build/native/ram.h'], 
	'ram.ice', 'slice2cpp --output-dir build/native $SOURCES')


# NATIVE MARSHALING CODE
##############################

# Construct ICE stubs for Python
env.Command('build/python/ram_ice.py', 
	'ram.ice', 'slice2py --output-dir build/python $SOURCES')


# NATIVE MARSHALING CODE
##############################

nativeIceLib = env.Library('build/native/ram.cpp')
env.Append(LIBS = [nativeIceLib])


# MOCK SERVER
##############################

env.Program(['build/mockserver.cpp'])


# MOCK CLIENT
##############################

env.Install('build/python','python/mockclient.py')


# SIMULATOR
##############################

env.Program('build/sim', env.Glob('src/sim/*.cpp'));
