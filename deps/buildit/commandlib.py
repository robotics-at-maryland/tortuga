"""
Commands with more explicit purposes than a generic ShellCommand;
it's debatable whether these are useful or more readable than just inlining
the shell command text, but they seem to be a reasonable way to factor things
so the implementation of common operations be changed more easily from a
central place.

A command instance must have at least these methods:

represent(task): Display a human-readable represention of the command.
    It accepts a Task instance as an argument.

execute(task): Perform the action asssociated with the command.  It
    accepts a Task instance as an argument.

check(task): Ensure that the command could be run properly, but do not run it.
    It accepts a Task instance as an argument.
"""

import os
import sys
import re
import shutil
import tempfile

from task import ShellCommand
from resolver import MissingDependencyError

class Download(ShellCommand):
    def __init__(self, filename, url, remove_on_error=True):
        self.filename = filename
        self.url = url
        self.remove_on_error = remove_on_error

    def represent(self, task):
        filename = task.interpolate(self.filename)
        url = task.interpolate(self.url)
        try:
            return 'wget -O %s %s' % (filename, url)
        except:
            if self.remove_on_error:
                try:
                    os.remove(filename)
                except:
                    pass
                raise

    check = represent

class CVSCheckout:
    def __init__(self, repo, dir, module, tag=''):
        self.repo = repo
        self.dir = dir
        self.module = module
        self.tag = tag

    def represent(self, task):
        resolved = task.interpolate_kw(**self.__dict__)
        return 'cvs -d %(repo)s co -d %(dir)s %(tag)s %(module)s' % resolved

    def execute(self, task):
        env_munged = False
        try:
            if not os.environ.has_key('CVS_RSH'):
                os.environ['CVS_RSH'] = 'ssh'
                env_munged = True
            os.system(self.represent(task))
        finally:
            if env_munged:
                del os.environ['CVS_RSH']

    check = represent

SYMLINK = (
    'expected_frm=%(frm)s; link_to=%(to)s; existing_frm=""; '
    '[ -e "$link_to" ] && x=$(ls -l "$link_to"); existing_frm="${x#*-> }"; '
    '[ "$expected_frm" = "$existing_frm" ] || rm -f "$link_to"; '
    '[ -L %(to)s ] || ln -sf %(frm)s %(to)s'
    )

class Fetch(ShellCommand):
    SVN_COMMAND = 'svn co %(url)s %(name)s%(versionstr)s'
    CVS_COMMAND = 'cvs -d %(url)s co -d %(name)s%(versionstr)s %(path)s'
    def __init__(self, url, name, using, path, version=''):
        self.url = url
        self.name = name
        self.using = using
        self.version = version
        self.path = path

    def get_versionstr(self):
        if self.version:
            versionstr = '-%s' % self.version
        else:
            versionstr = ''
        return versionstr

    def represent(self, task):
        stuff = task.interpolate_kw(**self.__dict__)
        using = stuff['using']
        stuff['versionstr'] = task.interpolate(self.get_versionstr())
        if using == 'download':
            return '<Download fetcher for "%(url)s">' % stuff
        elif using == 'svn':
            return self.SVN_COMMAND % stuff
        elif using == 'cvs':
            return self.CVS_COMMAND % stuff
        return '<Unknown fetcher type>'

    def execute(self, task):
        stuff = task.interpolate_kw(**self.__dict__)
        using = stuff['using']
        stuff['versionstr'] = task.interpolate(self.get_versionstr())

        if using == 'svn':
            result = os.system(self.SVN_COMMAND % stuff)
            if result == 256:
                # try 'svn cleanup' once, this doesn't work if the
                # package has externals though; subversion utterly blows
                result = self.svn_cleanup(task)
                if result:
                    task.output("svn cleanup failed")
                    return result
                result = os.system(self.SVN_COMMAND % stuff)
            
        elif using == 'cvs':
            return os.system(self.CVS_COMMAND % stuff)

        elif using == 'download':
            stuff['tempfile'] = tempfile.mktemp()
            remove = 'rm -rf %(name)s%(versionstr)s' % stuff
            task.output(remove)
            result = os.system(remove)
            if not result:
                download = 'wget -q -O %(tempfile)s %(url)s' % stuff
                task.output(download)
                result = os.system(download)
                if not result:
                    try:
                        ftype = 'file %(tempfile)s' % stuff
                        task.output(ftype)
                        fp = os.popen(ftype)
                        text = fp.read()
                        result = fp.close()
                        if not result:
                            if text.lower().find('gzip') != -1:
                                untgz = 'tar xzf %(tempfile)s' % stuff
                                task.output(untgz)
                                result = os.system(untgz)
                            elif text.lower().find('zip') != -1:
                                unzip = 'unzip %(tempfile)s' % stuff
                                task.output(unzip)
                                result = os.system(unzip)
                            else:
                                result = 1

                            if not result and stuff['versionstr']:
                                move = ('mv %(name)s %(name)s%(versionstr)s' %
                                        stuff)
                                task.output(move)
                                result = os.system(move)
                    finally:
                        os.unlink(stuff['tempfile'])

        else:
            raise ValueError(
                'unknown "using" value %s for Fetch in task %s' %
                (using, task)
                )
        return result


    check = represent

    def svn_cleanup(self, task):
        stuff = task.interpolate_kw(**self.__dict__)
        stuff['versionstr'] = task.interpolate(self.get_versionstr())
        command = 'svn cleanup %(name)s%(versionstr)s' % stuff
        task.output("cleanup required: running %r" % command )
        result = os.system(command)
        return result

        
class Symlink(ShellCommand):
    def __init__(self, frm, to):
        self.frm = frm
        self.to = to

    def represent(self, task):
        resolved = task.interpolate_kw(**self.__dict__)
        return SYMLINK % resolved

    check = represent

PATCH = (
    'test -e %(file)s.origorig || (cp %(file)s %(file)s.aside && '
    'patch -f -p%(level)s < %(patch)s && mv %(file)s.aside %(file)s.origorig)'
    )

class Patch(ShellCommand):
    def __init__(self, file, patch, level):
        self.file = file
        self.patch = patch
        self.level = level

    def represent(self, task):
        resolved = task.interpolate_kw(**self.__dict__)
        return PATCH % resolved

    check = represent

hugger = re.compile(r'<<([\w\.-]+)>>')

class InFileWriter:
    """Helper class that reads *infile*, and writes it to *outfile*
    with string interpolation values obtained from the task's repl mapping.

    outfile is created with permissions *mode* (default 0755).
    """
    def __init__(self, infile, outfile, mode=0755):
        self.infile = infile
        self.outfile = outfile
        self.mode = mode

    def execute(self, task):
        infilename = task.interpolate(self.infile)
        outfilename = task.interpolate(self.outfile)
        text = open(infilename).read()
        needs_replacement = hugger.findall(text)
        # XXX fix me to not combine global and local lookups and not ignore
        # case
        for item in needs_replacement:
            lower = item.lower()
            try:
                # try local
                rvalue = task.interpolate('${./%s}' % lower)
            except MissingDependencyError:
                # try global; if this fails, fail.
                rvalue = task.interpolate('${%s}' % lower)
            text = text.replace('<<%s>>' % item, rvalue)
                
        outfile = open(outfilename, 'w')
        outfile.write(text)
        outfile.close()
        
        os.chmod(outfilename, self.mode)

    def represent(self, task):
        return '<InFileWriter instance for %s>' % task.interpolate(self.outfile)

    def check(self, task):
        infilename = task.interpolate(self.infile)
        if not os.path.exists(infilename):
            raise ValueError('%r does not exist to read as input '
                             'file' % infilename)
        return task.interpolate(self.outfile)

class Substitute:
    def __init__(self, filename, search_re, replacement_string,
                 backup_ext='.~subst'):
        self.filename = filename
        self.search_re = search_re
        self.replacement_string = replacement_string
        self.backup_ext = backup_ext

    def execute(self, task):
        search_re = task.interpolate(self.search_re)
        repl = task.interpolate(self.replacement_string)
        filename = task.interpolate(self.filename)
        backup_ext = task.interpolate(self.backup_ext)

        try:
            search = re.compile(search_re, re.MULTILINE)
        except:
            print 'search_re %s could not be compiled' % search_re
            raise

        bakfilename = '%s%s' % (filename, backup_ext)
        shutil.copy(filename, bakfilename)

        text = open(filename).read()
        text = repl.join(search.split(text))

        newfilename = '%s%s' % (filename, '.~new')
        newfile = open(newfilename, 'w')
        newfile.write(text)
        shutil.copymode(filename, newfilename)
        shutil.move(newfilename, filename)

    def represent(self, task):
        repl = task.interpolate(self.replacement_string)
        filename = task.interpolate(self.filename)
        return 'Substituter for %s in %s' % (repl, filename)

    check = represent

OMIT_DIRS = [os.path.normcase("CVS"), os.path.normcase('.svn')]

class SkelCopier:

    """ Makes an exact copy of one directory to another.  If a file
    in the source directory has a .in extension, replace its values
    with task interpolated values, and write it to the target directory
    without the .in extension """

    def __init__(self, skeldir, tgtdir):
        self.skeldir = skeldir
        self.tgtdir = tgtdir

    def represent(self, task):
        stuff = task.interpolate_kw(**self.__dict__)
        return 'SkelCopier from %(skeldir)s to %(tgtdir)s' % stuff

    check = represent

    def execute(self, task):
        skeldir = task.interpolate(self.skeldir)
        tgtdir = task.interpolate(self.tgtdir)

        if not os.path.exists(tgtdir):
            os.makedirs(tgtdir)

        for dirpath, dirnames, filenames in os.walk(skeldir):

            for filename in filenames:
                # Copy the file:
                src = os.path.join(skeldir, filename)
                dst = os.path.join(tgtdir, filename)
                if os.path.exists(dst):
                    continue
                sn, ext = os.path.splitext(filename)
                if os.path.normcase(ext) == ".in":
                    infilecopier = InFileWriter(src, dst)
                    infilecopier.execute(task)
                else:
                    shutil.copyfile(src, dst)

                shutil.copymode(src, dst)

            for dirname in dirnames:
                # make the directories
                if os.path.normcase(dirname) in OMIT_DIRS:
                    continue
                dn = os.path.join(tgtdir, dirname)
                if not os.path.exists(dn):
                    os.mkdir(dn)
                    shutil.copymode(os.path.join(skeldir, dirname), dn)
