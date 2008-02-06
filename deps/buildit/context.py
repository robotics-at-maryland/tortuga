import re
import os
import sys
import logging

import parser
import resolver

try:
    from distutils.util import get_platform
    platform = get_platform()
except ImportError:
    platform = None

def select_one(filename, section_name):
    sections = parser.parse(filename)
    section  = sections.get(section_name)
    if section is None:
        raise ValueError(
            'No such section %r in file %r' % (section_name, filename)
            )
    return section

def select(selections):
    raw = {}
    for ns_name, filename, section_name in selections:
        section = select_one(filename, section_name)
        raw[ns_name] = section
    return raw

def resolve_file_section(filename, section_name, defaults=None):
    if defaults is None:
        defaults = {}

    namespace = select_one(filename, section_name)
    namespaces = {'whatever':namespace}
    sections = resolver.resolve(namespaces, defaults)
    return sections['whatever']

VERSIONFINDER = re.compile(r'\[([\w\.-]+)\]$').search

class Context:

    _resolved = None

    def __init__(self, inifile, buildoutdir=None, logger=None,
                 namespace_overrides=None):

        if buildoutdir is None:
            # assume that the caller is the main makefile
            caller_globals = sys._getframe(1).f_globals
            modulename = caller_globals.get('__file__', sys.argv[0])
            buildoutdir = os.path.abspath(os.path.dirname(modulename))

        username = None
        if os.name == 'nt':
            username = os.environ['USERNAME']
        else:
            import pwd
            username = pwd.getpwuid(os.getuid())[0]
            
        builtins = {
            'cwd':os.path.abspath(os.getcwd()),
            'username': username,
            'buildoutdir':buildoutdir,
            'platform':platform
            }

        self.inifile = inifile
        self.globals = builtins
        self.namespace_selections = []

        if namespace_overrides is None:
            namespace_overrides = {}
        self.namespace_overrides = namespace_overrides

        if logger is None:
            logger = logging.getLogger()
            logger.handlers[:] = []
            handler = logging.StreamHandler()
            formatter = logging.Formatter('%(message)s')
            handler.setFormatter(formatter)
            logger.addHandler(handler)
        self.logger = logger

        try:
            fileglobals = resolve_file_section(inifile, 'globals', builtins)
            self.globals.update(fileglobals)
        except ValueError:
            # tolerate missing [globals] section
            self.warn('No [globals] section in inifile %s ' % inifile)

        try:
            namespaces = resolve_file_section(inifile, 'namespaces', builtins)
        except ValueError:
            # tolerate missing [namespaces] section
            self.warn('No [namespaces] section in inifile %s ' % inifile)
            namespaces = {}

        for ns_name, value in namespaces.items():
            mo = VERSIONFINDER(value)
            if mo is None:
                raise ValueError(
                    'section name required in namespace declaration %s' % value
                    )
            section_name = mo.groups()[0]
            begin, end = mo.span()
            filename = value[0:begin].strip()
            self.namespace_selections.append((ns_name, filename, section_name))

    def set_section(self, ns_name, new_section_name):
        for name, file, section_name in self.namespace_selections[:]:
            if ns_name == name:
                self.namespace_selections.append((ns_name, file,
                                                  new_section_name))
                return
        raise ValueError('No such namespace to override: %s' % ns_name)

    def set_file(self, ns_name, filename, section_name):
        self.namespace_selections.append((ns_name, filename, section_name))

    def set_override(self, ns_name, k, v):
        self._resolved = None
        override_dict = self.namespace_overrides.setdefault(ns_name, {})
        override_dict[k] = v

    def resolve(self):
        if self._resolved is None:
            selections = select(self.namespace_selections)
            for ns_name, D in selections.items():
                overrides = self.namespace_overrides.get(ns_name, {})
                D.update(overrides)
            self._resolved = resolver.resolve(selections, self.globals)
        return self._resolved

    def interpolate(self, s, default_ns, task=None, overrides=None):
        resolved = self.resolve()
        if overrides is None:
            overrides = {}
        # XXX i wish we didn't need to make a copy here
        overrides = overrides.copy()
        overrides.update(self.globals)
        result = resolver.resolve_value(s, default_ns, resolved, overrides,
                                        task)
        return result

    def warn(self, msg):
        self.logger.warn(msg)

    def debug(self, msg):
        self.logger.debug(msg)

    def info(self, msg):
        self.logger.info(msg)

    def check(self, task):
        """ Perform a preflight sanity check """
        self.warn('buildit: context.check() starting with root task named '
                  '"%s"' % task.getName())
        for task in postorder(task, self):
            # these will raise errors if there are interpolation problems
            task.getWorkDir()
            for command in task.getCommands():
                command.check(task)
            task.getTargets()
            if task.condition:
                task.condition(task)

        return True

    def install(self):
        filename = self.inifile
        sections = parser.parse(filename)
        tasks = []
        for sname, section in sections.items():
            if sname.endswith(':instance'):
                instancename = sname[:-9]
                task = section.get('buildit_task')
                if task is None:
                    raise ValueError(
                        'buildit: no buildit_task for instance %r' % sname)
                order = section.get('buildit_order', None)
                if order is None:
                    self.warn('buildit: WARNING instance %r has no '
                              'buildit_order' % sname)
                mo = VERSIONFINDER(task)
                if mo is not None:
                    ns_name = mo.groups()[0]
                    begin, end = mo.span()
                    dottedname = task[0:begin].strip()
                    task = importable_name(dottedname)
                else:
                    dottedname = task
                    task = importable_name(dottedname)
                    ns_name = task.namespaces[0]
                overrides = []
                for name, value in section.items():
                    if name != 'buildit_task':
                        overrides.append((ns_name, name, value))
                tasks.append((order, instancename, task, overrides))
        tasks.sort()
        for order, instancename, task, overrides in tasks:
            for ns_name, name, value in overrides:
                self.set_override(ns_name, name, value)
            self.warn('buildit: running instance definition %r' % instancename)
            if self.check(task):
                self.run(task)

    def run(self, task):
        """ Run the task and all of its dependents in dependency order """
        self.warn('buildit: context.run() starting with root task named "%s"' %
                  task.getName())
        for task in postorder(task, self):
            if task.hasCommands() and task.needsCompletion():
                task.attemptCompletion()
        self.warn('buildit: done with task')

class Software:
    def __init__(self, task, context):
        self.task = task
        self.context = context
        self.overrides = {}

    def set(self, k, v, ns_name=None):
        if ns_name is None:
            ns_name = self.task.namespace
        self.overrides[(ns_name, k)] = v

    def install(self):
        for (ns_name, k), v in self.overrides.items():
            self.context.set_override(ns_name, k, v)
        if self.context.check(self.task):
            self.context.run(self.task)

def postorder(startnode, context):
    """ Postorder depth-first traversal of the dependency graph implied
    by startnode and its children; set context along the way """
    seen = {}

    def visit(node):
        namespace = node.namespace
        seen[node] = True
        for child in node.dependencies:
            if not isinstance(child, node.__class__):
                raise TypeError("Can't use non-Task %r as a dependency"
                                  % child)
            if child not in seen:
                for result in visit(child):
                    for namespace in result.getNamespaces():
                        result.setNamespace(namespace)
                        yield result

        node.setContext(context)
        yield node

    startnode.setContext(context)
    for namespace in startnode.getNamespaces():
        startnode.setNamespace(namespace)
        for node in visit(startnode):
            yield node

# A datatype that converts a Python dotted-path-name to an object

def importable_name(name):
    try:
        components = name.split('.')
        start = components[0]
        g = globals()
        package = __import__(start, g, g)
        modulenames = [start]
        for component in components[1:]:
            modulenames.append(component)
            try:
                package = getattr(package, component)
            except AttributeError:
                n = '.'.join(modulenames)
                package = __import__(n, g, g, component)
        return package
    except ImportError:
        import traceback, cStringIO
        IO = cStringIO.StringIO()
        traceback.print_exc(file=IO)
        raise ValueError(
            'The object named by %r could not be imported\n%s' %  (
            name, IO.getvalue()))
