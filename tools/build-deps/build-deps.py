#! /usr/bin/python

import os
import sys
import logging
import subprocess
from cfgparse.compat import ConfigParser
from optparse import OptionParser


def setup_logging():
    """
    Create a system logging and make it the root logger
    """
    
    console = logging.StreamHandler()
    console.setLevel(logging.INFO)

    formatter = logging.Formatter('%(levelname)-8s %(message)s')
    console.setFormatter(formatter)
    logging.getLogger('').setLevel(logging.INFO)
    logging.getLogger('').addHandler(console)
    

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

def run_location(location, config, mode='file'):
    """
    @type  location: tuple
    @param location: Location name + path pair
    
    @type  config: ConfigParser object
    @param config: An already loaded config parser
    """
    current_dir = os.getcwd()
    location_path = os.path.join(get_svn_root(), location[1])
    deps_dir = os.path.join(get_svn_root(), 'deps')
    
    # Check to make sure the file isn't already built
    dep_file_path = os.path.join(deps_dir, 'local', '%s.depend' % location[0])
    if os.path.exists(dep_file_path):
        logging.info('%s already built' % location[0])
        return
    
    if mode is 'file':
        runlog = open(os.path.join(deps_dir, '%s.log' % location[0]), 'w')
    else:
        runlog = None
    
    logging.info('Building %s' % location[0])
    logging.info('Entering: %s' % location_path)
    os.chdir(location_path)
    
    # Set current location so it can be expanded in the config file
    config.set(location[0], 'deps', deps_dir)
    
    for command in config.items(location[0]):
        # Run everything but the 'hidden' deps command
        if command[0] is not 'deps':
            logging.info('Running \'%s %s\'' % command)
            ret = subprocess.call('%s %s' % command, stdout=runlog, 
                                  stderr=runlog, shell=True) 
            if ret is not 0:
                logging.error('executing %s %s. Exiting.' % command)
                sys.exit(1)
    
    # Leave directory and clean up
    logging.info('Leaving: %s' % location_path)
    os.chdir(current_dir)
    logging.info('Finished building %s' % location[0])
    
    if mode is 'file':
        runlog.close()
        
    # create dependency file to make sure we aren't build twice
    dep_file = open(dep_file_path,'w')
    dep_file.close()
    

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
    else:
        for loc in config.items('locations'):
            run_location(loc, config)
        
def main():
    setup_logging()
    
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
    run_config(config_path, options.location)

    

if __name__ == '__main__':
    try:
        sys.exit(main())
    except KeyboardInterrupt:
        print 'Stopping...'
        sys.exit(-1)
        
