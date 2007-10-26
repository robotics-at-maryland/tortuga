# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  buildfiles/features.py

"""
Turns and off features of out codebase to make it easier to only work with part
of the system at one time.
"""

import os

# --------------------------------------------------------------------------- #
#                            F E A T U R E S                                  #
# --------------------------------------------------------------------------- #

# Global feature map
FEATURES = None

def get_features():
    global FEATURES

    # Only generate the list once
    if not FEATURES is None:
        return FEATURES

    FEATURES = {}

    # List of features in the build
    
    # See the "add_feature" function in the "SUPPORT" section of the file below
    # for more information    
    add_feature('core', dirs = ['packages/core'])
    add_feature('pattern', dirs = ['packages/pattern'])
    add_feature('math', dirs = ['packages/math'])
    add_feature('vision', dirs = ['packages/vision'],
                deps = ['pattern', 'core'])

    if os.name == 'posix':
        add_feature('drivers', dirs = ['packages/sensorapi',
                                       'packages/thrusterapi',
                                       'packages/imu',
                                       'packages/carnetix'])
        
        add_feature('control', dirs = ['packages/control'],
                    deps = ['math', 'core', 'vehicle'])

        add_feature('calib_tools', dirs = ['tools/MagInclination',
                                           'tools/BiasFinder'])

        add_feature('vehicle', dirs = ['packages/vehicle'],
                    deps = ['math', 'core', 'pattern', 'drivers'])


        add_feature('jaus', dirs = ['sandbox/jaus'])
    return FEATURES

# --------------------------------------------------------------------------- #
#                           I N T E R F A C E                                 #
# --------------------------------------------------------------------------- #

def available():
    """
    Gives the list of available features on this plaform
    
    @rtype : list of strings
    @return: A list of the avaible features on this plaform
    """
    return get_features().keys()

def setup_environment(env):
    """
    Apply the nesesary changes to given environment to enable use of the rest
    of the feature system. This add the RAM_WITH_<feature> defines, and the
    'HasFeature' function.

    You must have env['FEATURES'] set to a list of strings of the selected
    features.
    """

    # Check inputs
    if not env.has_key('with_features'):
        raise Exception('Environment does not have list of selected features')

    if not env.has_key('without_features'):
        raise Exception('Environment does not have list of unwanted features')

    has_with = len(env['with_features']) != len(available())
    has_without = len(env['without_features']) != 0
    if has_with and has_without:
        raise Exception('Can only use "with_features" or "without_features"' +
                        ' not both at the same time')

    # Determine list of actual features
    features = env['with_features']
    if has_without:
        features = available()
        for f in env['without_features']:
            features.remove(f)

    # Find and add the extra features needed by deps
    global FEATURES
    current = set(features)
    added = set()

    for f in features:
        added.update(FEATURES[f].all_deps(added, FEATURES))
    added = added.difference(current)

    if len(added) > 0:
        print '\nWarning: the following features were added because the were'
        print 'needed by the selected features: "' + '", "'.join(added) + '"\n'

    features.extend(added)
    env['features'] = features

    # Place defines in the environement
    env.AppendUnique(CPPDEFINES = ['RAM_WITH' + f for f in env['features']])

    # Place Feature list in the envrionment
    features = {}
    for f in env['features']:
        features[f] = FEATURES[f]
    env['FEATURES'] = features

    # Place 'HasFeature function in the enviornment
    from SCons.Script.SConscript import SConsEnvironment # just do this once
    SConsEnvironment.HasFeature = has_feature

def dirs_to_build(env):
    """
    Returns the directories to build based on the given environments selected
    features
    """
    
    ensure_feature_map(env)

    dirs = []
    for f in env['FEATURES'].itervalues():
        dirs.extend(f.get_dirs(env['features']))

    return dirs

# --------------------------------------------------------------------------- #
#                            S U P P O R T                                    #
# --------------------------------------------------------------------------- #

class Feature(object):
    def __init__(self, name, dirs, deps = []):
        self.name = name
        self.dirs = dirs
        self.deps = deps

    def get_dirs(self, selected_features):
        return self.dirs

    def all_deps(self, added, feature_map):
        for dep_f in self.deps:
            new = True
            # Ensure we aren't in a recursive loop
            if dep_f in added:
                new = False
            
            added.add(dep_f)
            if new:
                added.update(feature_map[dep_f].all_deps(added, feature_map))
        return added

def add_feature(name, dirs, deps = []):
    """
    @type  name: string
    @param name: The name of the feature

    @type  dirs: list of strings
    @param dirs: The directories to build

    @type  deps: list of strings
    @param deps: The features this feature depends on
    """
    global FEATURES
    FEATURES[name] = Feature(name, dirs, deps)

def ensure_feature_map(env):
    """
    Makes sure the environment given has the feature map created by
    setup_enviornment
    """
    if not env.has_key('FEATURES'):
        raise Exception('Environment does not have map of selected features')

def has_feature(env, name):
    """
    returns true if we have this feature in the enviornment
    """
    ensure_feature_map(env)
    return env['FEATURES'].has_key(name)
