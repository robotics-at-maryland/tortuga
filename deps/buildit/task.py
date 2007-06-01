import UserDict
import os
import warnings
import sys
import resolver

class TaskError(ValueError):
    pass

class TargetNotCreatedError(TaskError):
    pass

VERBOSE = 0
STRICT_TARGET_CHECKING = 1
MAKEFILE_TIMESTAMP_CHECKING = 0
DEPENDENCY_TARGET_TIMESTAMP_CHECKING = 0
FALSE_CONDITION = ()
FALSE_TARGETS = (FALSE_CONDITION,)

class Task:

    namespace = ''
    context = None

    def __init__(self, name, namespaces=(), targets=(), dependencies=(),
                 commands=(), workdir=None, condition=None, mglobals=None):
        self.name = name
        self.namespaces = sequence_helper(namespaces)
        self.targets = sequence_helper(targets)
        self.dependencies = sequence_helper(dependencies)
        self.commands = sequence_helper(commands)
        self.workdir = workdir
        self.condition = condition

        if mglobals is None:
            # Get a hold of caller's globals around so we can figure out
            # whether the source file has changed or not.  This is sneaky
            # but it's more convenient than needing to explicitly pass in
            # globals to each task's constructor.
            caller_globals = sys._getframe(1).f_globals
        else:
            caller_globals = mglobals

        makefile = caller_globals.get('__file__', sys.argv[0])
        if makefile.endswith('pyc') or makefile.endswith('.pyo'):
            makefile = makefile[:-1]

        self.makefile = os.path.abspath(makefile)
        self.builtins = {'makefile':self.makefile,
                         'makefiledir':os.path.dirname(self.makefile),}

    # getters

    def getMakefile(self):
        return self.makefile

    def getWorkDir(self):
        if self.workdir:
            return self.interpolate(self.workdir)
        return None

    def getTargets(self):
        targets = []
        if self.condition is not None:
            if not self.condition(self):
                return FALSE_TARGETS
                
        if self.targets:
            for target in self.targets:
                target =  self.interpolate(target)
                workdir = self.getWorkDir()
                if target and not os.path.isabs(target):
                    if workdir:
                        target = os.path.join(workdir, target)
                    else:
                        self.context.warn(
                            'Relative target %r specified without a working '
                            'directory' % target
                            )
                targets.append(target)
        return targets

    def getName(self):
        return self.name

    def getNamespaces(self):
        L = []
        for namespace in self.namespaces:
            val = self.interpolate(namespace)
            if not hasattr(val, '__iter__'):
                # allow either lists or space-separated tokens in a single
                # string
                val = val.strip().split()
            L.extend(val)
        if not L:
            # a task without any namespaces has a single namespace, None
            L = [None]
        return L

    def setNamespace(self, namespace):
        self.namespace = namespace

    # helpers
    
    def __repr__(self):
        return '<%s %s named "%s">' % (self.__class__, id(self), self.name)

    def interpolate_kw(self, **kw):
        result = {}
        for k, v in kw.items():
            result[k] = self.interpolate(v)
        return result

    def interpolate(self, value):
        if self.context is None:
            raise TaskError('Tasks require a context to perform interpolation')
        return self.context.interpolate(value, self.namespace, self,
                                        self.builtins)

    def output(self, s):
        if self.context is None:
            raise TaskError('Tasks require a context to perform logging')
        self.context.warn('buildit: ' + str(s))

    def setContext(self, context):
        self.context = context

    def getContext(self):
        return self.context

    # meat

    def needsCompletion(self):

        """ If our target doesnt exist or any of our dependency's
        targets are newer than our target, return True, else return
        False """

        name = self.getName()

        if not self.targets:
            # if we don't have a target, we always need completion
            if VERBOSE:
                self.output('"%s" has a null set of targets' % name)
            return 1

        if self.condition is not None:

            if not self.condition(self):
                self.output('"%s" had a condition which prevents this '
                                  'target from needing completion' % name)
                return 0

        missing = self.missingTargets()
        if missing:
            errormsg = []
            for target in missing:
                # if one of our targets doesnt exist we definitely need
                # completion
                errormsg.append('"%s" is missing target %s' % (name, target))
            self.output('\n'.join(errormsg))
            return 1

        # otherwise, we *might* need to recomplete if one of our
        # dependencies' target's timestamps is newer than our target's
        # timestamp or our makefile is newer than any of our targets
        if DEPENDENCY_TARGET_TIMESTAMP_CHECKING:
            if self.dependencyTargetIsNewer():
                self.output('"%s" has a dependency with a newer target' %
                                  name)
                return 1
        if MAKEFILE_TIMESTAMP_CHECKING:
            olders = self.targetsOlderThanMakefile()
            if olders:
                errormsg = []
                for older in olders:
                    errormsg.append(
                        'makefile "%s" is newer than target "%s" of task "%s"'
                        % (self.makefile, older, self.getName())
                      )
                self.output('\n'.join(errormsg))
                return 1
        return 0

    def targetsOlderThanMakefile(self):

        """ If the makefile in which the task instance has been defined
        is newer than our target, return true """

        name = self.getName()
        targets = self.getTargets()
        makefile = self.getMakefile()
        olders = []
        for target in targets:
            if os.path.getmtime(makefile) > os.path.getmtime(target):
                olders.append(target)
        return olders

    def dependencyTargetIsNewer(self):
        
        """ If any of our dependencies' targets are newer than our
        target, return True, else return False """

        targets = self.getTargets()
        name = self.getName()

        for dep in self.dependencies:
            deptargets = dep.getTargets()
            depname = dep.getName()

            if not deptargets:
                # dependencies with no target are always newer, shortcut
                if VERBOSE:
                    self.output('%s: dependency "%s" has a null target '
                                      'set so is '
                                      'newer'% (name, depname)
                                      )
                return 1

            missingdeptarget = None
            for deptarget in deptargets:
                if deptarget is not FALSE_CONDITION:
                    if not os.path.exists(deptarget):
                        # nonexistent dependency target, it will definitely need
                        # completion
                        self.output('%s: "%s" missing dependency target '
                                          '%s' %
                                          (name, depname, deptarget)
                                          )
                        missingdeptarget = 1

            if missingdeptarget:
                return 1

            newerdeptarget = None
            for deptarget in deptargets:
                for target in targets:
                    if (target is not FALSE_CONDITION and
                        deptarget is not FALSE_CONDITION):
                        if os.path.getmtime(deptarget) > os.path.getmtime(
                            target):
                            self.output('%s: dependency "%s" has a newer '
                                              'tgt "%s"' %
                                              (name, depname, deptarget)
                                              )
                            newerdeptarget = 1
            if newerdeptarget:
                return 1

        return 0

    def getCommands(self):
        return command_helper(self.commands)

    def attemptCompletion(self):

        """ Do the work implied by the task (presumably create the
        target file)"""

        name = self.getName()

        old_workdir = os.getcwd()
        try:
            self.output('executing %s' % name)
            workdir = self.getWorkDir()
            if workdir:
                os.chdir(workdir)
                if VERBOSE:
                    self.output('changed working directory to %s' %
                                      workdir)
            for command in self.getCommands():
                self.output('running "%s"' % command.represent(self))
                status = command.execute(self)
                if status:
                    raise TaskError, (
                        'Task "%s": command "%s" failed with status code '
                        '"%s"' % (name, command, status)
                        )
        finally:
            os.chdir(old_workdir)
            if VERBOSE:
                self.output('reset working directory to %s' % old_workdir)
        if not self.getTargets():
            # it's completed if we don't have a target
            return 1
        if STRICT_TARGET_CHECKING:
            missing_targets = self.missingTargets()
            if missing_targets:
                errormsg = []
                for missing in missing_targets:
                    errormsg.append('target of %s (%s) was not created and '
                        'STRICT_TARGET_CHECKING is turned on' % (name,
                                                                 missing)
                        )
                raise TargetNotCreatedError('\n'.join(errormsg))
            for target in self.getTargets():
               if target is not FALSE_CONDITION:
                   # touch the target file
                   os.utime(target, None)
                   
        return not self.needsCompletion()

    def missingTargets(self):

        """ Check for the existence of our target files """

        targets = self.getTargets()
        missing = []

        for target in targets:
            exists = os.path.exists(target)
            if not exists:
                missing.append(target)

        return missing

    def hasCommands(self):
        return not not self.commands

def sequence_helper(val):
    if not isinstance(val, (list, tuple)):
        val = [val]
    return list(val)

class ShellCommand:
    def __init__(self, shell_command):
        self.shell_command = shell_command

    def represent(self, task):
        return task.interpolate(self.shell_command)

    def execute(self, task):
        return os.system(self.represent(task))

    def check(self, task):
        return self.represent(task)

def command_helper(seq):
    L = []
    for item in seq:
        if isinstance(item, basestring):
            # if the item is a string, it's a shell command
            item = ShellCommand(item)
        # otherwise its assumed to be a callable that has represent, execute,
        # and check methods
        L.append(item)
    return L
