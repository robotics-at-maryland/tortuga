import os
import platform

def get_dirs():
    """
    Returns the directories to build for the current platform, uses pythons 
    platform.system() function.
    """
    dirs = [os.path.join('packages','pattern')]

    if platform.system() == 'Linux':
        dirs.append(os.path.join('packages','extensions', 'wxogre'))

    return dirs
