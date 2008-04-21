import os
import SCons.Builder

def generate(env):
	hex = SCons.Builder.Builder(action = '/opt/local/bin/wine \'C:\\Program Files\\Microchip\\MPLAB C30\\bin\\pic30-elf-bin2hex\' < $SOURCE > $TARGET')
	env.Append(BUILDERS = {'Hex': hex})
	env.Replace(CC = '/opt/local/bin/wine \'C:\\Program Files\\Microchip\\MPLAB C30\\bin\\pic30-elf-gcc\'')
	env.Append(CFLAGS = ['-mcpu=$MCPU', '-Wall'])
	env.Append(LINKFLAGS = '-Wl,--script="C:\\Program Files\\Microchip\\MPLAB C30\\support\\gld\\p${MCPU}.gld",--report-mem')

def exists(env):
	return os.exists('~/.wine/drive_c')
