# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  wrappers/math/gen_math.py

# STD Imports
import os

# Project Imports
import buildfiles.wrap as wrap
#from wrap import make_already_exposed

def generate(local_ns, global_ns):
    """
    global_ns: is the module builder for the entire library
    local_ns: is the namespace that coresponds to the given namespace
    """
    base_path = os.path.join(os.environ['RAM_SVN_DIR'],'packages','math',
                             'include')


    # Find all the classes to wrap
    Radian = local_ns.class_('Radian')
    Degree = local_ns.class_('Degree')
    Vector3 = local_ns.class_('Vector3')

    # Include them
    Radian.include()
    Degree.include()
    Vector3.include()

    # Handle the 'ptr' functions
    wrap.fix_pointer_returns([Vector3], ignore_names = ['ptr'])
    wrap.fix_pointer_args([Vector3])

    # Append the approaite include files
    wrap.add_needed_includes([Radian, Degree, Vector3])

    # Remove implicit conversions
    wrap.set_implicit_conversions([Radian, Degree, Vector3], False)
