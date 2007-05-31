# This is a library of commands that can be reused during our build out

import os
import sys
import time
import platform
import subprocess
from StringIO import StringIO
from urllib2 import urlopen

class CommandError(Exception):
    pass

def safe_system(command):
    retcode = subprocess.call(command, shell=True)
    if retcode != 0:   
        raise CommandError, "Command '%s' with status %d" % \
             (command, retcode)
         
       

class Download(object):
    """
    This is downloads a given url to a filename.  urllib.urlretrieve can replace
    some of this functionality
    """
    
    CHUNK_SIZE = 4096
    SPINRATE = 32768 / CHUNK_SIZE
    
    def __init__(self, filename, url, remove_on_error=True):
        self.filename = filename
        self.url = url
        self.remove_on_error = remove_on_error

        # Setup our spining cursor
        self.spinning_cursor = []
        for ch in '-\\|/-\\|/':
            self.spinning_cursor.extend([ch] * Download.SPINRATE)
        self.cursor_pos = 0

    def represent(self, task):
        """
    	Called by builtit to report the command we are running
    	"""
    	self.filename = task.interpolate(self.filename)
    	self.url = task.interpolate(self.url)
    	return "Download %s to %s" % (self.url, self.filename)

    def execute(self, task = None):
	"""
	Runs the command
	"""
	if task is not None:
	    self.represent(task)

        try:
            # Open up remote and local file
            self.outfile = file(self.filename, 'wb')
            urlfile = urlopen(self.url)
            self.filesize = int(urlfile.info().get('Content-Length', None))
            
            # Report beginning of download
            print 'Beginning download of: %s' % url
            print '\tSize:',
            if self.filesize is None:
                print 'Unknown'
            else:
                print "%d KB" % (self.filesize / 1024)
            print '\tTo Local File:',self.filename
            start_time = time.time()
            
            # Download our file
            total_read = 0
            while 1:
                bytes = urlfile.read(Download.CHUNK_SIZE)
                bytes_read = len(bytes)
                
                # If its zero we have hit the end of the file
                if 0 == bytes_read:
                    break
                # Write our data to the file
                self.outfile.write(bytes)
                
                # Update status information
                total_read += bytes_read
                self.print_status(total_read)
            
            # Report download stats
            total_time = time.time() - start_time
            total_read /= 1024
            print '\nDownloaded: %d KB in %.2f seconds for %.2f KBs' % (
                  total_read, total_time, total_read/total_time)
        
        except:
            self.cleanup()
            raise
        self.outfile.close()

    def print_status(self, total_read):
        """
        Prints a line of status text about the download
        """
        if self.filesize != None:
            status = '\r\tProgress: %4.2f%%' % \
                (total_read/float(self.filesize) * 100)
        else:
            # This creates a little sprinning cursor so users knows something
            # is still happening
            if self.cursor_pos == len(self.spinning_cursor):
                self.cursor_pos = 0
            status = '\r\tDownloading: %s' % \
                self.spinning_cursor[self.cursor_pos]
            self.cursor_pos += 1
            
        print status,
        sys.stdout.flush()

    def cleanup(self):
        if self.remove_on_error:
            try:
                self.outfile.close()
                os.remove(self.filename)
            except:
                pass
    check = represent

class UpdatePkgConfig(object):
    """
    This changes the prefix line in pkgconfig files to point to given location
    
    Note: The 'mode' flag is kind of hack but its add some flexibility
    """

    def __init__(self, pkg_config_file, prefix, mode = 'pkgconfig'):
        """
        Updates pkg_config_file to use prefix.
        """
        self.pkg_file_path = pkg_config_file
        self.prefix = prefix

        if 'pkgconfig' == mode:
            self.fixline = self.pkgconfig
        elif 'wx' == mode:
            self.fixline = self.wx
        else:
            raise CommandError, "'%s' is not a valid mode" % mode

    def represent(self, task):
        """
        Called by buildit with the task the command is working on
        """
        self.pkg_file_path = task.interpolate(self.pkg_file_path)
        self.prefix = task.interpolate(self.prefix)
	return 'Updating pkg-config file: %s to use prefix: "%s"' % \
            (self.pkg_file_path, self.prefix)

    def execute(self, task = None):
	if task is not None:
            # Make sure variables are properly interpolated
            self.represent(task)

        # Open up the original file and read it into memory
        original_file = file(self.pkg_file_path, 'r')
        lines = original_file.readlines()
        original_file.close()

        # Correct File In Memory (creates a copy, not optimal)
        output = StringIO()
        for line in lines:
            # Call the function to fix the file
            output.write(self.fixline(line))

        # Write back corrected file
        corrected_file = file(self.pkg_file_path, 'w')
        corrected_file.write(output.getvalue())
        corrected_file.close()

    check = represent
    
    def pkgconfig(self, line):
        if line.startswith('prefix='):
            return 'prefix=%s\n' % self.prefix
        else:
            return line
            
    def wx(self, line):
        sentinal = 'prefix=${input_option_prefix-${this_prefix:'
        if line.startswith(sentinal):
            return '%s-%s}}\n' % (sentinal, self.prefix)
        else:
            return line


class ArchiveBase(object):

    def __init__(self, compressing = True):
	cmd = 'a'
	if not compressing:
	    cmd = 'x'
	self.archive_cmd = '%s %s ' % (ArchiveBase.get_7zip_name(), cmd)

    @staticmethod
    def get_7zip_name():
	if platform.system() == 'Linux':
	    return '7zr'
	elif platform.system() == 'Darwin':
	    return '7za'
	else:
	    return '7ZIP_FIXME'


class Archive(ArchiveBase):
    def __init__(self, filename, *file_list):
        """
        @type  filename: string
        @param filename: The name of the archive to create
        """
        ArchiveBase.__init__(self)

        self.filename = filename

        if 0 == len(file_list):
            self.files = '<Entire Working Directory>'
        else:
            self.file =  ' '.join(file_list)

    def check(self, task):
        self.filename = task.interpolate(self.filename)
        self.files = task.interpolate(self.files)

    def represent(self, task):
        # Interpolate our values
        self.check(task)

        if '<Entire Working Directory>' == self.files:
            self.files =  ' '.join(os.listdir(os.getcwd()))
        return self.getcmd()

    def execute(self, task = None):
        if task is not None:
            self.check(task)
        safe_system(self.getcmd())
			     
    def getcmd(self):
        return '%s %s %s' % (self.archive_cmd, self.filename, self.files)
    
class UnArchive(ArchiveBase):
    def __init__(self, filename):
        """
        @type  filename: string
        @param filename: The name of the archive to create
        """
        ArchiveBase.__init__(self, False)

        self.filename = filename

    def check(self, task):
        self.filename = task.interpolate(self.filename)

    def represent(self, task):
        # Interpolate our values
        self.check(task)

        return self.getcmd()

    def execute(self, task = None):
        if task is not None:
            self.check(task)
        safe_system(self.getcmd())
                 
    def getcmd(self):
        return '%s %s' % (self.archive_cmd, self.filename)
