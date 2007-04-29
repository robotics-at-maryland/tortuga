#! /usr/bin/python

import os
import sys
import shutil
import logging
import subprocess

from datetime import datetime, timedelta
from ConfigParser import ConfigParser
from optparse import OptionParser

def setup_logging():
    """
    Create a system logging and make it the root logger
    """
    
    #syslog = logging.SysLogHandler()
    # Borken for some reason
    if os.access('/root/backup.py', os.R_OK):
        syslog = logging.FileHandler('/root/backup.py')
    else:
        syslog = logging.StreamHandler()
    formatter = logging.Formatter('%(asctime)s %(name)-12s %(levelname)-8s'
                                  '%(message)s')
    syslog.setFormatter(formatter)
    logging.getLogger('').addHandler(syslog)

def weekly_backup(now, src_path, dest_path):
    """
    type  now: datetime
    param now: The time at the start of the backup

    type  src_path: string
    param src_path: The directory to be baced up

    type  dest_path: string
    param dset_path: The directory where the weekly backups go
    """

    # Remove all older backups
    delta = timedelta(opts[2] * 7)
    cutoff = now - delta;
    remove_old(weekly_path, cutoff)

    last_backup = now - timedelta(7)
    last_backup = last_backup.strftime('%F_%T')

    # Grab and sort directories (newest first)
    backups = os.listdir(path)
    backups.append(last_backup)
    backups.sort(reverse=True)

    # If our time one week in the past is at the head of that list it means
    # that we haven't made a backup in 7 days, so lets do a new one
    if (last_backup == backups[0]):
        shutil.copytree(src_path, weekly_path + os.sep + date_name(src_path)) 
    
def remove_old(path, date):
    """
    Removes any file who's name is older than the give date. Files names must
    have the format: %F_%T use strftime

    @type  dir: string
    @param dir: The absolute path to the directory to clean

    @type  date: dateime object
    @param date: A datetime object which represents the oldest date
    """
    
    logging.info('Removing all backups older than %s' % date.ctime())
    date_cutoff = date.strftime('%F_%T')
    
    # Grab and sort directories (oldest first)
    backups = os.listdir(path)
    backups.sort()
 
    # Trim off the old ones
    for backup in backups:
       if backup < date_cutoff:
           old = os.path.join(path, backup)
           logging.info('Removing: ' + old) 
           shutil.rmtree(old)
       else:
           break

def get_backup_info(config):
    """
    @type  configfile: ConfigParser
    @param configfile: The config file to read from

    @rtype:  dictionary of tuples
    @return: Maps directory to a tuple of (src, days, weeks)
    
    Config file format:
    # Name directory pair
    [locations]
    trac = /var/backup/trac
    
    # Specifies a weekly and inremental
    # With 10 days of incremental and 4 weeks of weekly backups
    # Days and weeks can be missing and default values will be used
    [trac]
    days = 10
    weeks = 4
    src = /tmp/trac-backup
    """
    if config.has_section('locations') is False:
        raise 'Congfig file must have a \'locations\' section'
    
    backup_info = {}
    locations = config.options('locations')
    
    # Read in the days and weeks for each slection (10 and 4 if not given)
    for location in locations:
        # Where to do the backup
        directory = config.get('locations', location)

        # Change values if options are set
        def get_option(key, default):
            if config.has_section(location):
                if config.has_option(location, key):
                    return config.get(location, key)
                else:
                    return default
              
        weeks = get_option('weeks', 10)
        days = get_option('days', 4)
        src = get_option('src', os.path.join('/tmp', location + '-backup'))
            
        backup_info[directory] = (src, days, weeks)
        
    return backup_info
        
def date_name(path):
    """
    Takes a file specified by path and returns its name in strftime format 
    %F_%T which is its last modified time
    """
    # Get the modification time from the timestamp of the fill
    mod_time = datetime.fromtimestamp(os.stat(path).st_mtime)
    # Grab our parent directory
    #base_dir = os.path.split(path)[0]
    # Join the two and rename the file
    return mod_time.strftime('%F_%T')    

    return dated_path

def ensure_path(path):
    """
    Makes sure a path exists, and if it doesn't it will create it
    """
    if os.path.exists(path) is False:
        os.makedirs(path);

def main(argv=None):
    setup_logging()

    # Parse are command line
    parser = OptionParser()
    parser.add_option("-c", "--config", dest="configfile",
                  help="The location of config", default='etc/opt/backup.cfg')
    
    (options, args) = parser.parse_args()
    
    # Read the config file and pull out usefull information
    config = ConfigParser()
    config.read(options.configfile)
    backup_tasks = get_backup_info(config)

    now = datetime.now()
    for path in backup_tasks.iterkeys():
	# Convert options
	raw_opts = backup_tasks[path]
	opts = (raw_opts[0], int(raw_opts[1]), int(raw_opts[2]))
	
        # Make sure src and destination are properly formatted
        src_path = opts[0]
        if os.path.exists(src_path) is False:
            raise "Could not find source directory %s" % src_path
        
        weekly_path = os.path.join(path, 'weekly')
        incremental_path = os.path.join(path, 'incremental')
        ensure_path(weekly_path)
        ensure_path(incremental_path)
        
        # Incremental Backup
        try:
            subprocess.call(['rdiff-backup', src_path, incremental_path])
            subprocess.call(['rdiff-backup', '--remove-older-than',
                             str(opts[1]) + 'D', incremental_path])
        except OSError:
            raise "Please install rdiff-backup and put it in on your PATH"


if __name__ == "__main__":
    sys.exit(main())

        
    
    
