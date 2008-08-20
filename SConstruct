import os

# Import path from environment
env = Environment(ENV = {'PATH' : os.environ['PATH']})

env.Append(FRAMEWORKS = ['OpenGL', 'GLUT'])
env.Append(CPPPATH = ['/opt/local/include', '/opt/local/include/ice', '#src', '#build/native'])
env.Append(LIBPATH = ['/opt/local/lib'])
env.Append(LIBS = ['Ice', 'IceUtil'])
env.BuildDir('build','src')


# NATIVE MARSHALING CODE FOR C++
##############################

env.Command(['build/native/ram.cpp','build/native/ram.h'], 
	'ram.ice', 'slice2cpp --output-dir build/native $SOURCES')
nativeIceLib = env.Library('build/native/ram.cpp')
env.Append(LIBS = [nativeIceLib])


# NATIVE MARSHALING CODE FOR PYTHON
##############################

env.Command('build/python/ram_ice.py', 
	'ram.ice', 'slice2py --output-dir build/python $SOURCES')


# MOCK SERVER
##############################

env.Program('build/mockserver', ['build/mock/MockServer.cpp'])


# MOCK CLIENT
##############################

env.Install('build/python','python/mockclient.py')


# SIMULATOR
##############################

simSource = env.Glob('src/sim/*.cpp') + env.Glob('src/sim/vehicle/*.cpp')
env.Append(LIBS = ['GLEW'])
env.Program('build/sim', simSource);
