#! /usr/bin/python

import sys
import os
import re
import shutil
from os.path import join

def removeable(root, name, patterns, excludes):
    """
    Checks to see whether the name is in the to remove list and ensures that
    we aren't supposed to keep it
    """
    full_path = join(root,name)

    for exclude in excludes:
        if full_path.endswith(exclude):
            return None
    for pattern in patterns:
        m = re.match(pattern, name)
        if m and m.group() == name:
            #print 'Matched %s to %s' %(pattern, name)
            return full_path

    return None


def remove_path_by_name(directory, patterns, excludes):
    """
    Walks the directory the script is in, and removes all files and directories
    that match a name in the given list.
    """
    for root, dirs, files in os.walk(directory, topdown=False):
        for name in files:
            full_path = removeable(root, name, patterns, excludes)
            if full_path:
                #print full_path
                os.remove(full_path)
        for name in dirs:
            full_path = removeable(root, name, patterns, excludes)
            if full_path:
                shutil.rmtree(full_path)
                #print full_path



def main():
    to_remove = ['Makefile.in', '.deps', '.libs', 'Makefile', 'depcomp',
                 'config.guess', 'config.sub', 'ltmain.sh', 'autom4te.cache',
                 'missing', 'aclocal.m4', 'missing', 'install-sh', 
                 'config.h.in', 'configure', 'config.log', 'config.status']
    exclude = ['PlugIns/EXRCodec/src/.libs', 'PlugIns/EXRCodec/src/.deps',
               'Tools/Wings3DExporter/Makefile']

    # Remove a ton of generated files
    remove_path_by_name(sys.path[0], to_remove, exclude)

    return 0


if __name__ == '__main__':
	sys.exit(main())
