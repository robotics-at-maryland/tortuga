import os
import SCons.Builder

def generate(env):
	hex = SCons.Builder.Builder(action = '/opt/pic/bin/pic30-bin2hex $SOURCE')
	pickitProg = SCons.Builder.Builder(action = 'cd /opt/pic/bin;/opt/pic/bin/pk2cmd -PP${MCPU} -M -R -F $SOURCE')
	env.Append(BUILDERS = {'Bin2Hex': hex, 'PicKit2' : pickitProg})
	env.Replace(CC = '/opt/pic/bin/pic30-gcc')
	env.Append(CFLAGS = ['-mcpu=$MCPU', '-Wall'])
	env.Append(LINKFLAGS = '-Wl,--script=\'p${MCPU}.gld\',--report-mem')

def exists(env):
	return os.exists('~/.wine/drive_c')
