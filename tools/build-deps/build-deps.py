#! /usr/bin/python

import os
import sys
import logging
import subprocess
from configobj import ConfigObj
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
    """Work from the the scripts start location to the root"""
    script_dir = os.path.split(sys.path[0])[0]   # <root>/scripts
    repo_root = os.path.split(script_dir)[0]     # <root>
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

def translate_command(_command):
    """
    If the command is one like 'cd' that won't work directly we need to 
    translate it the proper python call.  This just currently work for 'cd'
    """
    command = ''
    if type(_command) is tuple:
        for item in _command:
            print 'Adding "%s"' % item
            command = command + str(item)
    else:
        command = _command
    
    if command.startswith('cd'):
        target_dir = os.path.abspath(command[2:len(command)].strip())
        logging.info('Translating "%s" to an os.chdir("%s") call' % (command,target_dir))
        os.chdir(target_dir)
        return True
    else:
        return False

def run_location(location, config, mode='file'):
    """
    @type  location: tuple
    @param location: Location name + path pair
    
    @type  config: ConfigObj instance
    @param config: An already loaded config parser
    """
    current_dir = os.getcwd()
    build_cfg_dir = os.path.split(config.filename)[0]
    location_path = os.path.join(build_cfg_dir, location[1])
    deps_dir = os.path.join(get_svn_root(), 'deps')
    
    # Check to make sure the file isn't already built
    dep_file_path = os.path.join(deps_dir, 'local', '%s.depend' % location[0])
    if os.path.exists(dep_file_path):
        logging.info('%s already built' % location[0])
        return
    
    print 'Build Dir',build_cfg_dir
    if mode is 'file':
        runlog = open(os.path.join(build_cfg_dir, '%s.log' % location[0]), 'w')
    else:
        runlog = None
    
    logging.info('Building %s' % location[0])
    logging.info('Entering: %s' % location_path)
    os.chdir(location_path)
    
    # Set current location so it can be expanded in the config file
    proc_lines = file('/proc/cpuinfo').readlines()
    cpus = len([line for line in proc_lines if line.startswith('processor')])
    variables = {'deps' : deps_dir, 'cpus': str(cpus), 'j' : str(cpus + 1)}
    config[location[0]]['DEFAULT'] = variables

    for command in config[location[0]].iteritems():
        # Run everything but the 'hidden' deps command
        if command[0] is not 'DEFAULT':
            # We must do this so we can interpolate properly
            #command = (command[0], config[location[0]][command[0]])
            if not translate_command(command):
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
    
def main(argv = None):
    if argv is None:
        argv = sys.argv
    setup_logging()
    
    # Parse are command line
    parser = OptionParser(description= \
      'This runs a config file that is basically a mapping of locations to'\
      'commands.  Each command name for each location must be unique, see'\
      'test config in the tools/build-deps for an example.  Anything on the'\
      'command line a flag is considered a location to run.')
    parser.add_option("-c", "--config", dest="configfile",
                      help="The location of config", 
                      default='<mrbc-root>/deps/build.cfg')
    #parser.add_option("-l", "--location", dest="location",
    #              help="If given, run just this location from the configfile")
    
    # TODO: Change the location thing to the positional options instead of
    #       a flag
    (options, args) = parser.parse_args(argv)
    
    config_path = get_config_path(options.configfile)
    
    # Parse config file
    config = ConfigObj(config_path)
    if not config.has_key('locations'):
        print 'ERROR: Configuration file: %s does not have a \'locations\'' \
              ' section' % config_path
        sys.exit(1)
    
    # Run sections given on the command line, or all in the config file
    if len(args) > 1:
        for location in args[1:len(args)]:
                loc_path = config['locations'][location]
                run_location((location,loc_path), config)
    else:
        for loc in config['locations'].iteritems():
            run_location(loc, config)

if __name__ == '__main__':
    try:
        sys.exit(main())
    except KeyboardInterrupt:
        print 'Stopping...'
        sys.exit(-1)
        
