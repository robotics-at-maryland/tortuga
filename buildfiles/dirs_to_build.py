import os
import platform

COMMON_DIRS = [
    os.path.join('packages', 'vision'),
    os.path.join('packages', 'pattern'),
    os.path.join('packages', 'core'),
    os.path.join('wrappers', 'pattern')]

LINUX_DIRS = [
    os.path.join('packages','extensions', 'wxogre'),
    os.path.join('packages', 'carnetix'),]

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
