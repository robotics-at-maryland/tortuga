import os
import sys

def add_sys_path(new_path):
    """
    Taken from The Python Cookbook
    Author: Robin Parmar
    Version: 1.2
    Website: http://aspn.activestate.com/ASPN/Cookbook/Python/Recipe/52662
    
    Adds the specified path to the Python system path if it is not already there.
    Takes into account terminating slashes and case (on Windows).

    Returns -1 if the path does not exist, 1 if it was added, and 0 if it was not
    (because it is already present).
    """

    # standardise
    new_path = os.path.abspath(new_path)

    # MS-Windows does not respect case
    if sys.platform == 'win32':
        new_path = new_path.lower()

    # disallow bad paths
    do = -1
    if os.path.exists(new_path):
        do = 1

        # check against all paths currently available
        for x in sys.path:
            x = os.path.abspath(x)

            if sys.platform == 'win32':
                x = x.lower()
            if new_path in (x, x + os.sep):
                do = 0

        # add path if we don't already have it
        if do:
            sys.path.append(new_path)
            pass

    return do
