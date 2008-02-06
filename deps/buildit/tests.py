import os
import tempfile
import unittest
import shutil

class ResolverTests(unittest.TestCase):

    def test_resolve_options_localonly(self):
        options = {'name1':'hello ${./name2}',
                   'name2':'${./name3} goodbye',
                   'name3':'this is name3',
                   'name4':'this is name4',
                   'another':'another',
                   'repeated':'${./another} ${./another} ${./name3}'}
        from resolver import resolve_options
        new = resolve_options('foo', {'foo':options})
        self.assertEqual(new, {'name1':'hello this is name3 goodbye',
                               'name2':'this is name3 goodbye',
                               'name3':'this is name3',
                               'name4':'this is name4',
                               'repeated':'another another this is name3',
                               'another':'another'})

    def test_resolve_options_directloop(self):
        options = {'name1':'${./name2}',
                   'name2':'${./name1}',
                   'name3':'this is name3',
                   'name4':'this is name4',
                   'another':'another'}
        from resolver import resolve_options
        from resolver import CyclicDependencyError

        try:
            resolve_options('foo', {'foo':options})
        except CyclicDependencyError, why:
            self.assertEqual(why[0],
                             {
                ('foo', 'name2'): ['name1'],
                ('foo', 'name1'): ['name2'],
                })
            self.assertEqual(str(why),
                  ("<CyclicDependencyError: In section 'foo', option 'name1' "
                   "depends on options ['name2']; In section 'foo', option "
                   "'name2' depends on options ['name1']>"))
        else:
            raise AssertionError('did not raise')

    def test_resolve_options_indirectloop(self):
        options = {'name1':'${./name2}',
                   'name2':'${./name3}',
                   'name3':'${./name1}this is name3',
                   'name4':'this is name4',
                   'another':'another'}
        from resolver import resolve_options
        from resolver import CyclicDependencyError

        try:
            resolve_options('foo', {'foo':options})
        except CyclicDependencyError, why:
            self.assertEqual(why[0],
                 {
                ('foo', 'name2'): ['name1'],
                ('foo', 'name3'): ['name2'],
                ('foo', 'name1'): ['name3']
                })
        else:
            raise AssertionError('did not raise')

    def test_resolve_options_defaults(self):
        options = {'name1':'hello ${./name2}',
                   'name2':'${./name3} goodbye ${default1}',
                   'name3':'this is name3 ${default2}',
                   'name4':'this is name4',
                   'another':'another'}
        from resolver import resolve_options
        new = resolve_options('foo', {'foo':options},
                              defaults={'default1':'singsong',
                                        'default2':'JAMMA'})
        self.assertEqual(new,
                         {'name1':'hello this is name3 JAMMA goodbye singsong',
                          'name2':'this is name3 JAMMA goodbye singsong',
                          'name3':'this is name3 JAMMA',
                          'name4':'this is name4',
                          'another':'another'})

    def test_resolve_options_missing_locals(self):
        options = {'name1':'hello ${./localmissing1}',
                   'name2':'${./localmissing2} goodbye ${defaultmissing1}',
                   'name3':'this is name3 ${defaultmissing2}',
                   'name4':'this is name4',
                   'another':'another'}
        from resolver import resolve_options
        from resolver import MissingDependencyError
        from resolver import LOCAL
        from resolver import DEFAULT
        try:
            new = resolve_options('foo', {'foo':options})
        except MissingDependencyError, why:
            self.assertEqual(why.section_name, 'foo')
            self.assertEqual(why.option_name, 'localmissing1')
            self.assertEqual(why.value, 'hello ${./localmissing1}')
            self.assertEqual(why.offender, 'name1')
        else:
            raise AssertionError('didnt raise!')

    def test_resolve_ok(self):
        options1 = {'name1_options1':'hello ${./name2_options1}',
                   'name2_options1':'${./name3_options1} goodbye',
                   'name3_options1':'${default_options1}',
                   'name4_options1':'this is name4',
                   'another_options1':'another'}
        options2 = {'name1_options2':'hello ${./name2_options2}',
                    'name2_options2':'${./name3_options2} goodbye',
                    'name3_options2':'${default_options2}',
                    'name4_options2':'this is name4',
                    'another_options2':'another',
                    'external1':'${options1/name3_options1}',
                    'external2':'${options1/name2_options1}'}
        

        defaults = {'default_options1':'default_for_options1',
                    'default_options2':'default_for_options2'}
        sections = {'options1':options1, 'options2':options2}

        from resolver import resolve
        resolved = resolve(sections, defaults)

        self.assertEqual(resolved['options1'],
                       {'name1_options1':'hello default_for_options1 goodbye',
                        'name2_options1': 'default_for_options1 goodbye',
                        'name3_options1': 'default_for_options1',
                        'name4_options1': 'this is name4',
                        'another_options1': 'another'},
                         )
        self.assertEqual(resolved['options2'],
                         {
            'name1_options2':'hello default_for_options2 goodbye',
            'name2_options2': 'default_for_options2 goodbye',
            'name3_options2': 'default_for_options2',
            'name4_options2': 'this is name4',
            'another_options2': 'another',
            'external1':'default_for_options1',
            'external2':'default_for_options1 goodbye'
            }
                         )

    def test_resolve_missingexternal(self):
        options1 = {'name1_options1':'hello ${./name2_options1}',
                   'name2_options1':'${./name3_options1} goodbye',
                   'name3_options1':'${default_options1}',
                   'name4_options1':'this is name4',
                   'another_options1':'another'}
        options2 = {'name1_options2':'hello ${./name2_options2}',
                    'name2_options2':'${./name3_options2} goodbye',
                    'name3_options2':'${default_options2}',
                    'name4_options2':'this is name4',
                    'another_options2':'another',
                    'external1':'${options1/missing1}',
                    'external2':'${options1/missing2}'}

        defaults = {'default_options1':'default_for_options1',
                    'default_options2':'default_for_options2'}
        sections = {'options1':options1, 'options2':options2}

        from resolver import resolve
        from resolver import resolve_options
        from resolver import MissingDependencyError
        from resolver import LOCAL
        from resolver import DEFAULT

        try:
            resolve(sections, defaults)
        except MissingDependencyError, why:
            self.assertEqual(why.section_name, 'options2')
            self.assertEqual(why.option_name, 'external1')
            self.assertEqual(why.value, '${options1/missing1}')
            self.assertEqual(why.offender, 'missing1')
        else:
            raise AssertionError('No raise')

    def test_resolve_directloop(self):
        options1 = {'name1':'hello ${options2/name1}'}
        options2 = {'name1':'hello ${options1/name1}'}

        sections = {'options1':options1, 'options2':options2}

        from resolver import resolve
        from resolver import CyclicDependencyError

        try:
            resolve(sections)
        except CyclicDependencyError, why:
            self.assertEqual(
                why[0],
                {(None, 'options1'): ['options2'],
                 (None, 'options2'): ['options1']}
                )
        else:
            raise AssertionError('No raise')

    def test_resolve_indirectloop(self):
        options1 = {'name1':'hello ${options2/name1}'}
        options2 = {'name1':'hello ${options3/name1}'}
        options3 = {'name1':'hello ${options1/name1}'}

        sections = {'options1':options1, 'options2':options2,
                    'options3':options3}

        from resolver import resolve
        from resolver import CyclicDependencyError

        try:
            resolve(sections)
        except CyclicDependencyError, why:
            self.assertEqual(
                why[0],
                {(None, 'options3'): ['options2'],
                 (None, 'options1'): ['options3'],
                 (None, 'options2'): ['options1']}
                )
        else:
            raise AssertionError('No raise')

    def test_resolve_with_overrides(self):
        options1 = {'name1_options1':'hello ${./name2_options1}',
                   'name2_options1':'${./name3_options1} goodbye',
                   'name3_options1':'${default_options1}',
                   'name4_options1':'this is name4',
                   'another_options1':'another'}
        options2 = {'name1_options2':'hello ${./name2_options2}',
                    'name2_options2':'${./name3_options2} goodbye',
                    'name3_options2':'${default_options2}',
                    'name4_options2':'this is name4',
                    'another_options2':'another',
                    'external1':'${options1/name3_options1}',
                    'external2':'${options1/name2_options1}'}
        

        defaults = {'default_options1':'default_for_options1',
                    'default_options2':'default_for_options2'}
        sections = {'options1':options1, 'options2':options2}

        from resolver import resolve
        overrides = {'options1':{'name1_options1':'hello ${./name4_options1}'}}
        resolved = resolve(sections, defaults, overrides)

        self.assertEqual(resolved['options1'],
                       {'name1_options1':'hello this is name4',
                        'name2_options1': 'default_for_options1 goodbye',
                        'name3_options1': 'default_for_options1',
                        'name4_options1': 'this is name4',
                        'another_options1': 'another'},
                         )
        self.assertEqual(resolved['options2'],
                         {
            'name1_options2':'hello default_for_options2 goodbye',
            'name2_options2': 'default_for_options2 goodbye',
            'name3_options2': 'default_for_options2',
            'name4_options2': 'this is name4',
            'another_options2': 'another',
            'external1':'default_for_options1',
            'external2':'default_for_options1 goodbye'
            }
            )


class ParserTests(unittest.TestCase):

    def setUp(self):
        self.tempdir = tempfile.mkdtemp()
        
    def tearDown(self):
        shutil.rmtree(self.tempdir)

    def _makeFile(self, filename, text):
        filename = os.path.join(self.tempdir, filename)
        f = open(filename, 'w')
        f.write(text)
        return filename

    def test_parse_empty_nodefaults(self):
        filename = self._makeFile('empty', '')
        from parser import parse
        sections = parse(filename)
        self.assertEqual(sections, {})

    def test_parse_empty_with_global_defaults(self):
        filename = self._makeFile('empty', '')
        from parser import parse
        sections = parse(filename, defaults={'a':1})
        self.assertEqual(sections, {})

    def test_parse_onesection_global_defaults(self):
        body = """[foo]\nbar=1\nbaz=2"""
        filename = self._makeFile('onesection', body)
        from parser import parse
        sections = parse(filename, defaults={'a':1})
        self.assertEqual(sections,
                         {'foo': {'a': 1, 'baz': '2', 'bar': '1'}}
                         )
        
    def test_parse_onesection_no_global_defaults(self):
        body = """[foo]\nbar=1\nbaz=2"""
        filename = self._makeFile('onesection', body)
        from parser import parse
        sections = parse(filename)
        self.assertEqual(sections,
                         {'foo': {'baz': '2', 'bar': '1'}}
                         )

    def test_parse_onesection_body_defaults(self):
        body = """[DEFAULT]\na=1\n[foo]\nbar=1\nbaz=2"""
        filename = self._makeFile('onesection', body)
        from parser import parse
        sections = parse(filename)
        self.assertEqual(sections,
                         {'foo': {'baz': '2', 'bar': '1', 'a': '1'}}
                         )
        
    def test_parse_onesection_body_defaults_and_global_defaults(self):
        body = """[DEFAULT]\na=1\n[foo]\nbar=1\nbaz=2"""
        filename = self._makeFile('onesection', body)
        from parser import parse
        sections = parse(filename, {'b':'2'})
        self.assertEqual(sections,
                         {'foo': {'baz': '2', 'bar': '1', 'a': '1', 'b':'2'}}
                         )

    def test_parse_twosections_global_defaults(self):
        body = """[foo]\nbar=1\nbaz=2\n[foo2]\nbar2=3\nbaz2=4"""
        filename = self._makeFile('onesection', body)
        from parser import parse
        sections = parse(filename, defaults={'a':1})
        self.assertEqual(sections,
                         {'foo': {'a':1, 'baz': '2', 'bar': '1'},
                          'foo2': {'a':1, 'baz2': '4', 'bar2': '3'}
                          }
                         )

    def test_bad_xformer(self):
        body = """[foo]\nbar:buz=1\nbaz:biz=2\n"""
        filename = self._makeFile('onesection', body)
        from parser import parse
        import ConfigParser
        self.assertRaises(ConfigParser.ParsingError, parse, filename)

    def test_tokens_xformer(self):
        body = """[foo]\nbar:tokens=1 2 3 4 jammy:jam\n"""
        filename = self._makeFile('onesection', body)
        from parser import parse
        sections = parse(filename)
        self.assertEqual(sections,
                         {'foo': {'bar': ['1', '2', '3', '4', 'jammy:jam']}})

    def test_continuation_lines(self):
        body = """[foo]\nbar=1 2 3 4 jammy:jam\n foo bar\n boo\nbiz=1"""
        filename = self._makeFile('onesection', body)
        from parser import parse
        sections = parse(filename)
        self.assertEqual(
            sections,
            {'foo': {'bar': '1 2 3 4 jammy:jam\nfoo bar\nboo', 'biz': '1'}}
        )
        
    def test_continuation_lines_with_tokens(self):
        body = """[foo]\nbar:tokens=1 2 3 4 jammy:jam\n foo bar\n boo\nbiz=1"""
        filename = self._makeFile('onesection', body)
        from parser import parse
        sections = parse(filename)
        self.assertEqual(
            sections,
            {'foo': {'bar':
                     ['1', '2', '3', '4', 'jammy:jam', 'foo', 'bar', 'boo'],
                     'biz': '1'}})

        

class ContextTests(unittest.TestCase):

    def setUp(self):
        self.tempdir = tempfile.mkdtemp()
        
    def tearDown(self):
        shutil.rmtree(self.tempdir)

    def _makeFile(self, filename, text):
        filename = os.path.join(self.tempdir, filename)
        f = open(filename, 'w')
        f.write(text)
        return filename

    def _getTargetClass(self):
        from context import Context
        return Context

    def _makeOne(self, inifile, buildoutdir=None, logger=None,
                 namespace_overrides=None):
        klass = self._getTargetClass()
        return klass(inifile, buildoutdir, logger, namespace_overrides)

    def test_select_one_ok(self):
        from context import select_one
        f = self._makeFile('file.ini', '[sect]\nz=1\ny=1')
        result = select_one(f, 'sect')
        self.assertEqual(result, {'z':'1', 'y':'1'})

    def test_select_one_fail(self):
        from context import select_one
        f = self._makeFile('file.ini', '[sect]\nz=1\ny=1')
        self.assertRaises(ValueError, select_one, f, 'sect2')

    def test_select_ok(self):
        from context import select
        f1 = self._makeFile('file1.ini', '[sect]\nz=1\ny=1')
        f2 = self._makeFile('file2.ini', '[sect]\nz=2\ny=2')
        result = select((['one', f1, 'sect'], ['two', f2, 'sect']))
        self.assertEqual(result,
                         {'one': {'y': '1', 'z': '1'}, 
                          'two': {'y': '2', 'z': '2'} })

    def test_resolve_file_section(self):
        from context import resolve_file_section
        f = self._makeFile('file.ini', '[sect]\nz=1\ny=1')
        result = resolve_file_section(f, 'sect')
        self.assertEqual(result, {'y': '1', 'z': '1'})

    def test_Context_ctor_noglobals_nonamespaces(self):
        logger = DummyLogger()
        inifile = self._makeFile('foo', '')
        context = self._makeOne(inifile, buildoutdir='/tmp', logger=logger)
        self.assertEqual(context.inifile, inifile)
        self.assertEqual(len(context.globals), 4)
        self.assertEqual(context.globals['buildoutdir'], '/tmp')
        self.assertEqual(context.namespace_overrides, {})
        self.assertEqual(context.namespace_selections, [])
        warnings = logger.msgs
        self.assertEqual(len(warnings), 2)
        self.failUnless(warnings[0].startswith('No [globals] section'))
        self.failUnless(warnings[1].startswith('No [namespaces] section'))

    def test_Context_ctor_nosection(self):
        inifile = self._makeFile('foo',
            """\
[namespaces]
pound = foo.ini
[globals]
a =1""")
        logger = DummyLogger()
        self.assertRaises(ValueError, self._makeOne, inifile, '/tmp', logger)
        self.assertEqual(logger.msgs, [])

    def test_Context_ctor(self):
        inifile = self._makeFile('foo',
            """\
[namespaces]
pound = foo.ini [1.0]
[globals]
a =1""")
        logger = DummyLogger()
        context = self._makeOne(inifile, buildoutdir='/tmp', logger=logger)
        self.assertEqual(context.inifile, inifile)
        self.assertEqual(len(context.globals), 5)
        self.assertEqual(context.globals['buildoutdir'], '/tmp')
        self.assertEqual(context.globals['a'], '1')
        self.assertEqual(context.namespace_overrides, {})
        self.assertEqual(context.namespace_selections,
                         [('pound', 'foo.ini', '1.0')])
        self.assertEqual(logger.msgs, [])

    def test_Context_set_section(self):
        inifile = self._makeFile('foo',
            """\
[namespaces]
pound = foo.ini [1.0]
[globals]
a =1""")
        logger = DummyLogger()
        context = self._makeOne(inifile, buildoutdir='/tmp', logger=logger)
        self.assertRaises(ValueError, context.set_section, 'nonesuch', 'a')
        context.set_section('pound', '2.0')
        self.assertEqual(len(context.namespace_selections), 2)
        self.assertEqual(
            context.namespace_selections,
            [('pound', 'foo.ini', '1.0'), ('pound', 'foo.ini', '2.0')]
            )

    def test_Context_set_file(self):
        inifile = self._makeFile('foo',
            """\
[namespaces]
pound = foo.ini [1.0]
[globals]
a =1""")
        logger = DummyLogger()
        context = self._makeOne(inifile, buildoutdir='/tmp', logger=logger)
        self.assertRaises(ValueError, context.set_section, 'nonesuch', 'a')
        context.set_file('pound', 'bar.ini', '2.0')
        self.assertEqual(len(context.namespace_selections), 2)
        self.assertEqual(
            context.namespace_selections,
            [('pound', 'foo.ini', '1.0'), ('pound', 'bar.ini', '2.0')]
            )

    def test_Context_resolve_nofile(self):
        inifile = self._makeFile('foo',
            """\
[namespaces]
pound = foo.ini [1.0]
[globals]
a =1""")
        logger = DummyLogger()
        context = self._makeOne(inifile, buildoutdir='/tmp', logger=logger,
                                namespace_overrides={'pound':{'a':'1'}})
        self.assertRaises(IOError, context.resolve)

    def test_Context_resolve(self):
        inifile = self._makeFile('root.ini',
            """\
[namespaces]
pound = %s/pound.ini [1.0]
[globals]
a =1""" % self.tempdir)
        self._makeFile('pound.ini',
            """\
[1.0]
a = 2
b = 1""")
        logger = DummyLogger()
        context = self._makeOne(inifile, buildoutdir='/tmp', logger=logger,
                                namespace_overrides={'pound':{'a':'1'}})
        result = context.resolve()
        self.assertEqual(result, {'pound':{'a':'1', 'b':'1'}})

    def test_Context_interpolate(self):
        from resolver import MissingDependencyError
        inifile = self._makeFile('root.ini',
            """\
[namespaces]
pound = %s/pound.ini [1.0]
[globals]
c =5""" % self.tempdir)
        self._makeFile('pound.ini',
            """\
[1.0]
a = 2
b = 1""")
        logger = DummyLogger()
        context = self._makeOne(inifile, buildoutdir='/tmp', logger=logger,
                                namespace_overrides={'pound':{'a':'1'}})
        try:
            context.interpolate('${foo}', 'pound', 'fleeb')
        except MissingDependencyError, why:
            self.assertEqual(
                str(why),
                ("<MissingDependencyError in section named 'pound', option "
                 "named 'fleeb', value '${foo}', offender 'foo'>"))
        result = context.interpolate('${./a}', 'pound')
        self.assertEqual(result, '1')
        result = context.interpolate('${c}', 'pound')
        self.assertEqual(result, '5')

    def test_logging(self):
        inifile = self._makeFile('root.ini',
            """\
[namespaces]
pound = %s/pound.ini [1.0]
[globals]
c =5""" % self.tempdir)
        logger = DummyLogger()
        context = self._makeOne(inifile, buildoutdir='/tmp', logger=logger,
                                namespace_overrides={'pound':{'a':'1'}})
        context.warn('foo')
        self.assertEqual(logger.msgs, ['foo'])
        context.debug('bar')
        self.assertEqual(logger.msgs, ['foo', 'bar'])

    def test_check(self):
        inifile = self._makeFile('root.ini',
            """\
[namespaces]
pound = %s/pound.ini [1.0]
[globals]
c =5""" % self.tempdir)
        logger = DummyLogger()
        context = self._makeOne(inifile, buildoutdir='/tmp', logger=logger,
                                namespace_overrides={'pound':{'a':'1'}})
        task = DummyTask('name', dependencies=(), workdir=self.tempdir,
                         commands=(), targets=(), condition=None)
        result = context.check(task)
        self.assertEqual(result, 1)
        self.assertEqual(task.context, context)
        self.assertEqual(len(logger.msgs), 1)
        self.assertEqual(task.attempted, False)

    def test_run(self):
        inifile = self._makeFile('root.ini',
            """\
[namespaces]
pound = %s/pound.ini [1.0]
[globals]
c =5""" % self.tempdir)
        logger = DummyLogger()
        context = self._makeOne(inifile, buildoutdir='/tmp', logger=logger,
                                namespace_overrides={'pound':{'a':'1'}})
        task = DummyTask('name', dependencies=(), workdir=self.tempdir,
                         commands=(int,), targets=(), condition=None)
        result = context.run(task)
        self.assertEqual(result, None)
        self.assertEqual(task.context, context)
        self.assertEqual(len(logger.msgs), 2)
        self.assertEqual(task.attempted, True)

    def test_install(self):
        logger = DummyLogger()
        # use fixture data in this module as tasks
        inifile = self._makeFile('root.ini',
            """\
[namespaces]
task1 = %s/task1.ini [1.0]
task2 = %s/task2.ini [1.0]
[globals]
c =5
[task1:instance]
buildit_task = tests.task1
buildit_order = 2
[task2:instance]
buildit_task = tests.task2""" % (self.tempdir, self.tempdir))
        self._makeFile('task1.ini',
            """\
[1.0]
tempdir = %s""" % self.tempdir)
        self._makeFile('task2.ini',
            """\
[1.0]
tempdir = %s""" % self.tempdir)
        context = self._makeOne(
            inifile,
            buildoutdir='/tmp', logger=logger,
            )
        context.install()
        self.failUnless(os.path.exists(os.path.join(self.tempdir, 'task1')))
        self.failUnless(os.path.exists(os.path.join(self.tempdir, 'task2')))
        self.assertEqual(logger.msgs[0],
             "buildit: WARNING instance 'task2:instance' has no buildit_order")
        self.assertEqual(logger.msgs[1],
                         "buildit: running instance definition 'task2'")


class TaskTests(unittest.TestCase):
    def setUp(self):
        self.tempdir = tempfile.mkdtemp()
        
    def tearDown(self):
        shutil.rmtree(self.tempdir)

    def _getTargetClass(self):
        from task import Task
        return Task

    def _makeOne(self, *arg, **kw):
        klass = self._getTargetClass()
        return klass(*arg, **kw)

    def _makeFile(self, filename, text):
        filename = os.path.join(self.tempdir, filename)
        f = open(filename, 'w')
        f.write(text)
        return filename

    def test_builtins(self):
        task = self._makeOne('test')
        thisfile = os.path.join(os.path.abspath(__file__))
        thisdir = os.path.split(thisfile)[0]
        try:
            from distutils.util import get_platform
        except ImportError:
            get_platform = lambda x: None

        self.assertEqual(task.builtins['makefile'], thisfile)
        self.assertEqual(task.builtins['makefiledir'], thisdir)

    def test_getMakefile(self):
        task = self._makeOne('test')
        self.assertEqual(task.getMakefile(), os.path.abspath(__file__))

    def test_getWorkDir(self):
        task = self._makeOne('test', workdir=self.tempdir)
        context = DummyContext()
        task.setContext(context)
        self.assertEqual(task.getWorkDir(), self.tempdir)

    def test_getMakefile(self):
        task = self._makeOne('test')
        context = DummyContext()
        task.setContext(context)
        self.assertEqual(task.getMakefile(), os.path.abspath(__file__))

    def test_getTargets_falsecondition(self):
        from task import FALSE_TARGETS
        def foo(task):
            return False
        task = self._makeOne('test', targets='a', condition=foo)
        context = DummyContext()
        task.setContext(context)
        self.assertEqual(task.getTargets(), FALSE_TARGETS)

    def test_getTargets_notargets(self):
        task = self._makeOne('test')
        context = DummyContext()
        task.setContext(context)
        self.assertEqual(task.getTargets(), [])

    def test_getTargets_abs_noworkdir(self):
        task = self._makeOne('test', targets=self.tempdir+'/foo')
        context = DummyContext()
        task.setContext(context)
        self.assertEqual(task.getTargets(), [self.tempdir+'/foo'])

    def test_getTargets_rel_noworkdir(self):
        task = self._makeOne('test', targets='foo')
        context = DummyContext()
        task.setContext(context)
        self.assertEqual(task.getTargets(), ['foo'])
        self.assertEqual(context.msgs[0],
             "Relative target 'foo' specified without a working directory")

    def test_getTargets_abs_workdir(self):
        task = self._makeOne('test', targets=self.tempdir+'/foo',
                             workdir='/etc')
        context = DummyContext()
        task.setContext(context)
        self.assertEqual(task.getTargets(), [self.tempdir+'/foo'])

    def test_getTargets_rel_workdir(self):
        task = self._makeOne('test', targets='foo', workdir='/etc')
        context = DummyContext()
        task.setContext(context)
        self.assertEqual(task.getTargets(), ['/etc/foo'])

    def test_getName(self):
        task = self._makeOne('test')
        context = DummyContext()
        task.setContext(context)
        self.assertEqual(task.getName(), 'test')

    def test_getNamespaces(self):
        task = self._makeOne('test', namespaces='foo')
        context = DummyContext()
        task.setContext(context)
        self.assertEqual(task.getNamespaces(), ['foo'])

        task = self._makeOne('test', namespaces=('foo', 'bar'))
        context = DummyContext()
        task.setContext(context)
        self.assertEqual(task.getNamespaces(), ['foo', 'bar'])

    def test_interpolate_nocontext(self):
        from task import TaskError
        task = self._makeOne('test')
        self.assertRaises(TaskError, task.interpolate, 'a')
        
    def test_interpolate_kw(self):
        task = self._makeOne('test')
        context = DummyContext()
        task.setContext(context)
        result = task.interpolate_kw(foo='1', bar='2')
        self.assertEqual(result, {'foo':'1', 'bar':'2'})

class PostorderTests(unittest.TestCase):
    def test_postorder_all_single_namespaces(self):
        from context import postorder
        startnode = DummyTask('startnode')
        dep1 = DummyTask('dep1')
        dep2 = DummyTask('dep2')
        startnode.dependencies = [dep1, dep2]
        context = DummyContext()
        generator = postorder(startnode, context)
        nodes = list(generator)
        self.assertEqual(len(nodes), 3)
        self.assertEqual(nodes[0], dep1)
        self.assertEqual(nodes[1], dep2)
        self.assertEqual(nodes[2], startnode)

    def test_unknown_child_node(self):
        from context import postorder
        startnode = DummyTask('startnode')
        startnode.dependencies = [object]
        context = DummyContext()
        generator = postorder(startnode, context)
        self.assertRaises(TypeError, list, generator)

    def test_postorder_with_cycles(self):
        from context import postorder
        startnode = DummyTask('startnode')
        startnode.dependencies = [startnode]
        context = DummyContext()
        generator = postorder(startnode, context)
        nodes = list(generator)
        self.assertEqual(len(nodes), 1)
        self.assertEqual(nodes[0], startnode)

    def test_postorder_multiple_namespaces(self):
        from context import postorder
        startnode = DummyTask('startnode')
        dep1 = DummyTask('dep1')
        dep1.namespaces = ['c', 'd']
        dep2 = DummyTask('dep2')
        dep2.namespaces = ['']
        startnode.dependencies = [dep1]
        dep1.dependencies = [dep2]
        context = DummyContext()
        generator = postorder(startnode, context)
        node = generator.next()
        self.assertEqual(node.name, 'dep2')
        self.assertEqual(node.namespace, '')
        self.assertEqual(node.context, context)
        node = generator.next()
        self.assertEqual(node.name, 'dep1')
        self.assertEqual(node.namespace, 'c')
        self.assertEqual(node.context, context)
        node = generator.next()
        self.assertEqual(node.name, 'dep1')
        self.assertEqual(node.namespace, 'd')
        self.assertEqual(node.context, context)
        node = generator.next()
        self.assertEqual(node.name, 'startnode')
        self.assertEqual(node.namespace, '')
        self.assertEqual(node.context, context)
        self.assertRaises(StopIteration, generator.next)

class DummyLogger:
    def __init__(self):
        self.msgs = []

    def warn(self, msg):
        self.msgs.append(msg)

    debug = info = warn

class DummyContext:
    def __init__(self):
        self.msgs = []
        
    def warn(self, msg):
        self.msgs.append(msg)

    def debug(self, msg):
        self.msgs.append(msg)
        
    def interpolate(self, v, default_ns, task=None, overrides=None):
        return v

class DummyTask:
    def __init__(self, name, **kw):
        self.name = name
        self.namespace = None
        self.namespaces = ['']
        self.dependencies = []
        self.__dict__.update(kw)
        self.context = None
        self.attempted = False
        
    def getName(self):
        return self.name

    def getContext(self):
        return self.context

    def setContext(self, context):
        self.context = context

    def getWorkDir(self):
        return self.workdir

    def getCommands(self):
        return self.commands

    def getTargets(self):
        return self.targets

    def attemptCompletion(self):
        self.attempted = True

    def hasCommands(self):
        return not not self.commands

    def needsCompletion(self):
        return True

    def getNamespaces(self):
        return self.namespaces

    def setNamespace(self, namespace):
        self.namespace = namespace

    def __repr__(self):
        return '<DummyTask instance with name %s in namespace %s>' % (
            self.name, self.namespace)

    __str__ = __repr__
        
# fixture data
from task import Task

task1 = Task('task1', namespaces='task1', commands=(
	  'touch ${./tempdir}/task1',), targets=('${./tempdir}/task1',))
task2 = Task('task2', namespaces='task2', commands=(
	  'touch ${./tempdir}/task2',), targets=('${./tempdir}/task2',))

def test_suite():
    return unittest.TestSuite((
        unittest.makeSuite( ResolverTests ),
        unittest.makeSuite( ParserTests ),
        unittest.makeSuite( ContextTests ),
        unittest.makeSuite( TaskTests ),
        unittest.makeSuite( PostorderTests ),
        ))

if __name__ == '__main__':
    unittest.main(defaultTest='test_suite')
