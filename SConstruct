import os

# Import path from environment
env = Environment(ENV = {'PATH' : os.environ['PATH']})

env.Append(FRAMEWORKS = ['OpenGL', 'GLUT'])
env.Append(CPPPATH = ['/opt/local/include', '/opt/local/include/ice', '#build/native', '#src'])
env.Append(LIBPATH = ['/opt/local/lib'])
env.Append(LIBS = ['Ice', 'IceUtil'])
# env.Append(CPPFLAGS = ['-O3'])
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

env.Append(FRAMEWORKS = ['Cocoa'])
env.Append(LIBS = ['SDL'])
env.Append(LIBS = ['SDLmain'])
env.Append(CPPPATH = ['/opt/local/include/SDL'])
env.Append(CPPPATH = ['#deps/bullet-2.70/src'])
env.Append(LIBPATH = ['#deps/bullet-2.70/src/BulletDynamics/Debug'])
env.Append(LIBS = ['LibBulletDynamics'])
env.Append(LIBPATH = ['#deps/bullet-2.70/src/BulletCollision/Debug'])
env.Append(LIBS = ['LibBulletCollision'])
env.Append(LIBPATH = ['#deps/bullet-2.70/src/LinearMath/Debug'])
env.Append(LIBS = ['LibLinearMath'])
env.Append(LIBPATH = ['#deps/bullet-2.70/Demos/OpenGL/Debug'])
env.Append(LIBS = ['LibOpenGLSupport'])
env.Append(CPPPATH = ['#deps/bullet-2.70/Demos/OpenGL'])
simSource = env.Glob('src/sim/*.cpp') + env.Glob('src/sim/vehicle/*.cpp') + env.Glob('src/math/src/*.cpp'), env.Glob('src/sim/physics/*.cpp')
env.Append(LIBS = ['GLEW'])
env.Program('build/sim', simSource);
