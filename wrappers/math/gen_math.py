# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  wrappers/math/gen_math.py

# STD Imports
import os

# Library Imports
from pyplusplus.module_builder import call_policies
from pygccxml import declarations

# Project Imports
import buildfiles.wrap as wrap

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
    Quaternion = local_ns.class_('Quaternion')
    Matrix3 = local_ns.class_('Matrix3')

    # Include them
    Radian.include()
    Degree.include()
    Vector3.include()
    Quaternion.include()
    Matrix3.include()

    # Map operator<< to __str__
    wrap.str_from_ostream(local_ns)


    # Fix '[]' operators on matrices
    c = Matrix3.operators('[]')
    c.call_policies= call_policies.convert_array_to_tuple(3, \
        call_policies.memory_managers.none)
    c.include()
    c.documentation = wrap.docit("Return Type Change", "None",
                                 "Tuple with 3 floats's (the matrix 'line')")

    # Handle the 'ptr' functions
    wrap.fix_pointer_returns([Vector3, Quaternion, Matrix3],
                             ignore_names = ['ptr'])
    wrap.fix_pointer_args([Vector3, Quaternion, Matrix3])

    # Append the approaite include files
    wrap.add_needed_includes([Radian, Degree, Vector3, Quaternion, Matrix3])
    Quaternion.include_files.append(Matrix3.location.file_name)
    # Remove implicit conversions
    wrap.set_implicit_conversions([Radian, Degree, Vector3, Quaternion, Matrix3],
                                  False)
