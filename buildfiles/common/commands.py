# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  buildfiles/common/commands.py

"""
This is a library of commands that can be reused during our build out
"""

import os
import sys
import time
import platform
import os
import os.path
from StringIO import StringIO
from urllib2 import urlopen

from util import CommandError, safe_system

__all__ = ['Download', 'Template', 'UpdatePkgConfig', 'Mkdir', 'Archive', 
           'UnArchive']  

# If On Windows import modules to update registry
if os.name == 'nt':
    import _winreg
    import win32gui
    import win32con
    __all__.append('EditEnvironment')
           
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

class Template(object):
    def __init__(self, in_filename, out_filename, **kwargs):
        """
        Templates a file using the python keyword subsitution formating. Note
        this changes the user to the same as the containing directory.
        """
        self.in_filename = in_filename
        self.out_filename = out_filename
        self.parameters = kwargs
        
    def represent(self, task):
        ret = 'Templating: "%s" to "%s" with paramters:\n' % \
            (task.interpolate(self.in_filename), 
             task.interpolate(self.out_filename))

        for k,v in self.parameters.iteritems():
            ret += '\t%s : %s\n' % (k, task.interpolate(v))
            
        return ret
    
    def check(self, task):
        self.represent(task)
        
    def execute(self, task):
        if task is not None:
            self.in_filename = task.interpolate(self.in_filename)
            self.out_filename = task.interpolate(self.out_filename)

            for k,v in self.parameters.items():
                self.parameters[k] = task.interpolate(v)
        
        # Read file into memory
        in_file = file(self.in_filename, 'r')
        template = ''.join(in_file.readlines())
        in_file.close()
        
        # Replace parameters and write results
        output_text = template % self.parameters
        out_file = file(self.out_filename, 'w')
        out_file.write(output_text)
        out_file.close()
        
        if os.name == 'posix':
            # Make user same as containing directory
            dir_info = os.stat(os.path.dirname(self.out_filename))
            os.chown(self.out_filename, dir_info.st_uid, dir_info.st_gid)
        

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


class Mkdir(object):
    """ 
    Takes a single string or a list which will be joined with os.join
    
    creates missing directories for the given path and
    returns a normalized absolute version of the path.

    - if the given path already exists in the filesystem
      the filesystem is not modified.

    - otherwise makepath creates directories along the given path
      using the dirname() of the path. You may append
      a '/' to the path if you want it to be a directory path.

    from holger@trillke.net 2002/03/18
    """
    def __init__(self, path):
        if type(path) is list:
            self.path = str(os.sep).join(item)
        else:
            self.path = path
        
    def represent(self, task):
        return 'Making directory: %s' % task.interpolate(self.path)
    
    def check(self, task):
        self.represent(task)
        
    def execute(self, task):
        filepath = self.path
        if task is not None:
            filepath = task.interpolate(self.path)
        
        path = os.path.expanduser(filepath)
        dpath = os.path.normpath(os.path.dirname(path))
        if not os.path.exists(dpath): 
            os.makedirs(dpath)

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

if os.name == 'nt':
    class EditEnvironment(object):
        APPEND = 1
        """Append to a list like environment variable"""
        PREPEND = 2
        """Prepend to a list like environment variable"""
        REPLACE = 3
        """Replaces the given value from a list like environment variable, 
           passing None will remove it from the list"""
        OVERWRITE = 4
        """Fully replace the enviornment variable with the given value, None
        will delete the variable completely.
        """
    
        def __init__(self, key, value = None, value2 = None, 
                     mode = APPEND):
            """
            @type  key: string
            @param key: The key you wish to edit
            
            @type  value: string
            @param value: The value you wish to relate to the key
            
            @type  mode: int
            @param mode: One of the modes specified by the class. 
            """
            
            self.key = key
            self.value = value
            self.value2 = value2
            self.mode = mode
            
        def represent(self, task):
            """
            Called by builtit to report the command we are running
            """
            self.key = task.interpolate(self.key)
            self.value = task.interpolate(self.value)
            self.value2 = task.interpolate(self.value2)
            
            actions = {
                EditEnvironment.APPEND : 
                    "Appending environment variable '%(key)s' with '%(val)s'",
                EditEnvironment.PREPEND : 
                    "Prepending environment variable '%(key)s' with '%(val)s'",
                EditEnvironment.REPLACE : 
                    "Replacing '%(val)s' in environment variable '%(key)s' with '%(val2)s'",
                EditEnvironment.OVERWRITE : 
                    "Overwriting environment variable '%(key)s' with '%(val)s'"
            }
            return actions[self.mode] % {'key' : self.key, 'val' : self.value,
                                         'val2' : self.value2}
        
        def queryValue(self, key, name):
            try:
                value, type_id = _winreg.QueryValueEx(key, name)
            except WindowsError, e:
                if 2 == e.winerror:
                    return ''
                raise e
            return value
        
        def setValue(self, key, name, value):
            _winreg.SetValueEx(key, name, 0, REG_EXPAND_SZ, value)
            print 'For regkey "%s" setting "%s" to "%s"' % (str(key), name, value)
        
        def deleteValue(self, key, name):
            _winreg.DeleteValue(regkey, self.key)
            print 'For regkey "%s" removing "%s"' % (str(key), name)
        
        def execute(self, task = None):
            """
            Runs the command
            """
            if task is not None:
                self.represent(task)
            
            regkey = None
            registry = None
            try:
                regpath = r'SYSTEM\CurrentControlSet\Control\Session Manager\Environment'
                registry = _winreg.ConnectRegistry(None, _winreg.HKEY_LOCAL_MACHINE)
                regkey = _winreg.OpenKey(registry, regpath, 0, _winreg.KEY_ALL_ACCESS)
                
                # Handles setting the whole value
                if self.mode == EditEnvironment.OVERWRITE:
                    if self.value is None:
                        self.deleteValue(regkey, self.key)
                    else:
                        self.setValue(regkey, self.key, self.value)
                        
                # Works with manipulating the value in list form
                else:
                    current_value = self.queryValue(regkey, self.key)
                    value_list = current_value.split(os.path.pathsep)
                    
                
                    if self.mode == EditEnvironment.APPEND:
                        value_list.append(self.value)
                    elif self.mode == EditEnvironment.PREPEND:
                        value_list.insert(0, self.value)
                    else:
                        # Either remove or replace every instance of the value
                        while value_list.count(self.value) != 0:
                            if self.value2 == None:                            
                                value_list.remove(self.value)
                            else:
                                index = value_list.index(self.value)
                                value_list[index] = self.value2
                                
                    # Join the values back into a single string
                    new_value = os.path.pathsep.join(value_list).strip(';')
                    self.setValue(regkey, self.key, new_value)

                # Notify the rest of the OS about the settings change
                win32gui.SendMessage(win32con.HWND_BROADCAST, win32con.WM_SETTINGCHANGE, 0, 'Environment')
                                    
            finally:
                if registry is not None:
                    _winreg.CloseKey(registry)
                if registry is not None:
                    _winreg.CloseKey(regkey)   
                