import os
import platform

COMMON_DIRS = [
    os.path.join('packages', 'vision'),
    os.path.join('packages','pattern')]

LINUX_DIRS = [
    os.path.join('packages','extensions', 'wxogre')]

MAC_DIRS = []

def get_dirs():
    """
    Returns the directories to build for the current platform, uses pythons 
    platform.system() function.
    """
    dirs = []
    dirs.extend(COMMON_DIRS)

    if platform.system() == 'Linux':
        dirs.extend(LINUX_DIRS)
    elif platform.system() == 'Darwin':
        dirs.extend(MAC_DIRS)

    return dirs
