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
import types
import SCons.Node.Python

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
	
    add_feature('vehicle', dirs = ['packages/vehicle'],
                    deps = ['math', 'core', 'pattern'])
    
    add_feature('control', dirs = ['packages/control'],
                    deps = ['math', 'core', 'vehicle'])
    
    add_feature('wrappers', dirs = ['wrappers/samples'], opt_dirs =
                    {'control' : ['wrappers/control'],
                     'math' : ['wrappers/math'],
                     'vehicle' : ['wrappers/vehicle'],
                     'core' : ['wrappers/core']})

    add_feature('vision_tools', dirs = ['tools/vision_viewer'],
                deps = ['vision'])

    if os.name == 'posix':
        add_feature('drivers', dirs = ['packages/sensorapi',
                                       'packages/thrusterapi',
                                       'packages/imu',
                                       'packages/carnetix'])

        add_feature('calib_tools', dirs = ['tools/MagInclination',
                                           'tools/BiasFinder'])

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
    of the feature system. This adds a Features.h file the RAM_WITH_<feature>
    defines, and the 'HasFeature' function.

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
    feature_list = env['with_features']
    if 'all' == str(feature_list):
        feature_list = available()
    if has_without:
        feature_list = available()
        for f in env['without_features']:
            feature_list.remove(f)

    # Find and add the extra features needed by deps
    global FEATURES
    current = set(feature_list)
    added = set()

    for f in current:
        added.update(FEATURES[f].all_deps(added, FEATURES))
    added = added.difference(current)

    if len(added) > 0:
        print '\nWarning: the following features were added because the were'
        print 'needed by the selected features: "' + '", "'.join(added) + '"\n'

    feature_list.extend(added)
    env['features'] = feature_list

    # Place Feature list in the envrionment
    feature_map = {}
    for f in env['features']:
        feature_map[f] = FEATURES[f]
    env['FEATURES'] = feature_map

    # Generate Feature.h file
    env.Command('packages/core/include/Feature.h',
                SCons.Node.Python.Value(feature_list),
                write_feature_h)

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
    def __init__(self, name, dirs, deps = [], opt_dirs = {}):
        """
        See documentation for 'add_feature' below
        """
        self.name = name
        self.dirs = dirs
        self.deps = deps
        self.opt_dirs = opt_dirs

    def get_dirs(self, selected_features):
        dirs = self.dirs

        # Only add optionaly dirs if the needed features are present
        for f in selected_features:
            if self.opt_dirs.has_key(f):
                dirs.extend(self.opt_dirs[f])

        return dirs

    def all_deps(self, added, feature_map):
        for dep_f in self.deps:
            # Ensure we aren't in a recursive loop
            new = True
            if dep_f in added:
                new = False
            
            added.add(dep_f)
            if new:
                added.update(feature_map[dep_f].all_deps(added, feature_map))
        return added

def write_feature_h(env, target, source):
    f = open(target[0].abspath, 'w')
    f.write('// DO NOT EDIT, This is a generated header\n')
    f.write('#ifndef RAM_CORE_FEATURES\n#define RAM_CORE_FEATURES\n\n')

    # Make sure we only have one source
    if type(source) is types.ListType:
        source = source[0]

    # Make sure we have a list
    if not type(source) is types.ListType:
        # For some reason this is a string, need to eval it
        str_val = source.get_contents()
        if str_val.find("'") > 0:
            features = eval(source.get_contents())
        else:
            features = str_val.split(',')
    #    features = [features]

    for feature in features:
        f.write('#define RAM_WITH_%s\n' % feature.upper())
    
    f.write('\n#endif // RAM_CORE_FEATURES\n\n')

def add_feature(name, dirs = [], deps = [], opt_dirs = {}):
    """
    @type  name: string
    @param name: The name of the feature

    @type  dirs: list of strings
    @param dirs: The directories to build

    @type  deps: list of strings
    @param deps: The features this feature depends on

    @type  opt_dirs: string to string list map
    @param opt_dirs: options directories to build if a feature is selected
    """
    global FEATURES
    FEATURES[name] = Feature(name, dirs, deps, opt_dirs)

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
