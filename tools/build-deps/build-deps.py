#! /usr/bin/python

import os
import sys
from cfgparse.compat import ConfigParser
from optparse import OptionParser

def get_svn_root():
     # Move up to directories to the root, and then down the default
     # location
    repo_root = os.path.split(sys.path[0])[0]
    repo_root = os.path.split(repo_root)[0]
    return repo_root

def get_config_path(start_path):
    """
    Gets the location of the config file, either in the deps directory or the 
    user specified one, and ensures that it exists
    """
    if start_path == '<mrbc-root>/deps/build.cfg':
        config_path = os.path.join(get_svn_root(), 'deps','build.cfg')
    else:
        config_path = os.path.abspath(start_path)
        
    if not os.path.exists(config_path):
        print "ERROR: Configuration file: %s does not exist" % config_path
        sys.exit(1)
        
    return config_path

def run_location(location, config):
    """
    @type  location: tuple
    @param location: Location name + path pair
    
    @type  config: ConfigParser object
    @param config: An already loaded config parser
    """
    current_dir = os.getcwd()
    location_path = os.path.join(get_svn_root(), location[1])
    
    # Change to our location, run commands then change back
    print '\nEntering: %s\n' % location_path
    os.chdir(location_path)
    config.set(location[0], 'deps', os.path.join(get_svn_root(), 'deps'))
    for command in config.items(location[0]):
        if command[0] is not 'deps':
            print '%s %s' % command
            if os.system('%s %s' % command) is not 0:
                print 'ERROR executing %s %s. Exiting.' % command
                sys.exit(1)
    print '\nLeaving: %s' % location_path
    os.chdir(current_dir)

def run_config(configfile, location = None):
    """
    Loads the config file and call run_location
    """    
    # Parse config file
    config = ConfigParser()
    config.optionxform = lambda x: x
    config.read(configfile)
    if not config.has_section('locations'):
        print 'ERROR: Configuration file: %s does not have a \'locations\'' \
              ' section' % config_path
        sys.exit(1)
        
    # Run sections
    if location:
        loc_path = config.get('locations', location)
        run_location((location,loc_path), config)
    for loc in config.items('locations'):
        run_location(loc, config)
        
def main():
    # Parse are command line
    parser = OptionParser(description= \
      'This runs a config file that is basically a mapping of locations to'\
      'commands.  Each command name for each location must be unique, see'\
      'test config in the tools/build-deps for an example')
    parser.add_option("-c", "--config", dest="configfile",
                  help="The location of config", 
                  default='<mrbc-root>/deps/build.cfg')
    parser.add_option("-l", "--location", dest="location",
                  help="If given, run just this location from the configfile")
    
    (options, args) = parser.parse_args()
    
    config_path = get_config_path(options.configfile)
    run_config(config_path)

    

if __name__ == '__main__':
    sys.exit(main())
