# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  buildfiles/variants.py

# --------------------------------------------------------------------------- #
#                            V A R I A N T S                                  #
# --------------------------------------------------------------------------- #
    
# Global variant map
VARIANTS = None

def get_variants():
    global VARIANTS

    # Only generate the list once
    if not VARIANTS is None:
        return VARIANTS

    VARIANTS = {}

    # Release
    #  No debug symbols
    #  Medium - High level of optimization
    add_variant(name = 'release', cflags = { 'linux' : ['-O2'],
                                             'darwin' : ['-O2'],
                                             'windows' : ['/O2'] })

    # Release - Debug
    #  Same as Release, but has debug symbols
    add_variant(name = 'release_dbg', suffix = '_rd',
                cflags = { 'linux' : ['-O2', '-g'],
                           'darwin' : ['-O2', '-g'] })

    # Profile
    #  Same as release, but has additional profiling information added
    add_variant(name = 'profile', suffix = '_p',
                cflags = { 'linux' : ['-O2', '-g', '-pg'],
                           'darwin' : ['-O2', '-g', '-pg']},
                ldflags = { 'linxu' : ['-pg'],
                            'darwin' : ['-pg'] })

    # Debug
    #  No optimization, and debug symbols on
    add_variant(name = 'debug', suffix = '_d',
                cflags = { 'linux' : ['-g', '-O0'],
                           'darwin' : ['-g', '-O0'] })

    return VARIANTS



# --------------------------------------------------------------------------- #
#                           I N T E R F A C E                                 #
# --------------------------------------------------------------------------- #

def available():
    """
    Gives the list of available variants on this plaform
    
    @rtype : list of strings
    @return: A list of the avaible variants on this plaform
    """
    return get_variants().keys()

def setup_environment(env):
    """
    Adjusts the environment to based on the current variant.

    This basically changes the suffix of all the generated object, libraries,
    and exacutables.  This allows multiple variants to exist at the same time.
    
    """
    # Check inputs
    if not env.has_key('variant'):
        raise Exception('Environment does not have a selected variant')

    variantName = str(env['variant'])

    # Ensure its a valid variant
    variants = get_variants()
    if not variants.has_key(variantName):
        raise Exception('"%s" is not a valid varaint' % variantName)
    
    # Adjust enviornment as needed
    variant = variants[variantName]

    env.AppendUnique(CCFLAGS = variant.cflags)
    env.AppendUnique(LDFLAGS = variant.ldflags)
    suffix = variant.suffix

    # Suffixes we have to adjust
    variables = ['OBJSUFFIX', 'SHOBJSUFFIX', 'SHLIBSUFFIX', 'PROGSUFFIX',
                 'LIBSUFFIX']

    for variable in variables:
        env[variable] = suffix + env[variable]
        
    # Set variant name
    env['VARIANT'] = variant

# --------------------------------------------------------------------------- #
#                            S U P P O R T                                    #
# --------------------------------------------------------------------------- #

def add_variant(name, cflags, ldflags = None, suffix = ''):
    if ldflags is None:
        ldflags = {}
    
    global VARIANTS
    VARIANTS[name] = Variant(name, cflags.get(get_platform(), []),
                             ldflags.get(get_platform(), []), suffix)


class Variant(object):
    def __init__(self, name, cflags, ldflags, suffix):
        self.name = name
        self.cflags = cflags
        self.ldflags = ldflags
        self.suffix = suffix

def get_platform():
    import platform
    
    plat = platform.system()
    convert = {'Darwin' : 'darwin',
               'Linux' : 'linux',
               'Windows' : 'windows',
               'Microsoft' : 'microsoft' }

    try:
        return convert[plat]
    except KeyError,e:
        key = e.args[0]
        raise Exception('Plaform "%s" is unsupported' % plat)
        
