# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  wrappers/control/gen_control.py

import os

import buildfiles.wrap as wrap

def generate(module_builder, local_ns, global_ns):

    wrap.make_already_exposed(global_ns, 'ram::core', ['Subsystem'])

    # Include controller classes
    IController = local_ns.class_('IController')
    IController.include()
    
    IController.include_files.append(os.environ['RAM_SVN_DIR'] +
                                     '/packages/control/include/IController.h')

#    wrap.make_already_exposed(global_ns, 'ram::pattern', 'Subject')

