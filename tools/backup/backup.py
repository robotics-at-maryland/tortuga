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



def remove_old(dir, date):
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
    backups = os.listdirs(dir)
    backups.sort().reverse()

    # Trim off the old ones
    for backup in backups:
       if backup < date_cutoff:
           old = dir + '/' + backu[
           logging.info('Removing: ' + old) 
           os.rmdir(old)
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
    backup_dir = './test_bkp'
    incr_dir = backup_dir + '/incremental'
    week_dir = backup_dir + '/weekly'

    # Weekly backup

if __name__ == "__main__":
    sys.exit(main())
