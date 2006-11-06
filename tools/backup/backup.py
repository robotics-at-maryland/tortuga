#! /usr/bin/python

import os
import sys
import logging
from datetime import datetime
from ConfigParser import ConfigParser

def setup_logging():
    """
    Create a system logging and make it the root logger
    """
    
    syslog = logging.SysLogHandler()
    formatter = logging.Formatter('%(asctime)s %(name)-12s %(levelname)-8s'
                                  '%(message)s')
    syslog.setFormatter(formatter)
    logging.getLogger('').addHandler(syslog)

def remove_path(path):
    """
    Remove directories and files recursively
    """
    if os.path.exists(path):
       if os.path.isdir(path):
           #sub_dirs 
           for d in os.listdir(path):
               remove_path(os.path.join(path, d))
           os.rmdir(path)
       else:
           os.remove(path)
    
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
           remove_path(old)
       else:
           break

def get_backup_dirs(configfile):
    """
    @type  configfile: string
    @param configfile: The absolute path to the config file

    @rtype:  list
    @return: The list of directories
    """
    config = ConfigParser()
    config.read(configfile)

def main(argv=None):
    """
    Config file format:
    # Name directory pair
    [locations]
    trac = /var/backup/trac
    
    # Specifies a weekly and inremental
    # With 10 days of incremental and 4 weeks of weekly backups
    [trac]
    type = inremental,weekly
    days = 10
    weeks = 4
    """
    backup_dir = './test_bkp'
    incr_dir = backup_dir + '/incremental'
    week_dir = backup_dir + '/weekly'

    # Weekly backup

if __name__ == "__main__":
    sys.exit(main())

        
    
    
