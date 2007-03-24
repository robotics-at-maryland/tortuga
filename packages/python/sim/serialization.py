# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   sim/serialization.py

"""
This module handles the saving and loading of objects
"""

# Projects Import
import core

class IKMLLoadable(core.Interface):
    """
    Any class that implements this interface can be loaded from a KML file
    """
    
    #@staticmethod
    def kml_load(node):
        """
        @type  node: Dict like object
        @param node: This will contain all the information needeed to generate
                     the parameters.
                     
        @rtype:  Dict
        @return: Maps __init__ parameters to there needed values for this 
                 object.
        """
        pass