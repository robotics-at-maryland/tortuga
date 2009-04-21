import sys
import os.path
import SCons.Builder
import SCons.Tool.cc
import SCons.Tool.ar
import SCons.Tool.link

toolfamily = 'bfin-linux-uclibc-'

compilers = [toolfamily+'gcc']

def exists(env):
    checkEnv = env.Clone()
    return(checkEnv.Detect(toolfamily+'gcc'))

def generate(env):
    SCons.Tool.cc.generate(env)
    SCons.Tool.ar.generate(env)
    SCons.Tool.link.generate(env)
    env['CC'] = toolfamily+'gcc'
    env['CXX'] = toolfamily+'g++'
    env['AS'] = toolfamily+'gcc'
    env['RANLIB'] = toolfamily+'ranlib'
    env['AR'] = toolfamily+'ar'
    env['BIN2HEX'] = toolfamily+'bin2hex'
    env['CCFLAGS'] = []
    env['OBJSUFFIX'] = '.o'
    env['PROGSUFFIX'] = '.coff'
    env['ASCOM']     = '$AS $ASFLAGS -o $TARGET -c $SOURCES'
