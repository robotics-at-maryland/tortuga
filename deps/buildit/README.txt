Buildit

  Buildit makes it easier to create a repeatable deployment of
  software in a particular configuration.  With it, you can perform
  conditional complilation of source code, install software, run
  scripts, or perform any repeatable sequence of tasks that ends up
  creating a known set files on your filesystem.  On subsequent runs
  of the same set of tasks, Buildit performs the least amount of work
  possible to create the same set of files, only performing the work
  that it detects has not already been performed by earlier runs.

Change History

  0.1 -- Initial release

  1.0 -- 4/15/2007, second release, see CHANGES.txt for info

Platforms

  Buildit runs under any platform on which Python supports the
  os.system command.  This includes UNIX/Linux and Windows.  It should
  be run via Python 2.4+. 

Installation

  Install Buildit by running the accompanying "setup.py" file, ala
  "python setup.py install".

License

  The license under which Buildit is released is a BSD-like license.
  It can be found accompanying the distribution in the LICENSE.txt
  file.

Rationale

  Buildit was created to allow me to write "buildout" profiles which
  need to perform arbitrary tasks in the service of setting up a
  client operating system environment with software and data specific
  to running applications I helped create.  For instance, in one case,
  it is currently used to build multiple "instances" of Zope, ZEO,
  Apache/PHP, Squid, Python, and MySQL on a single machine, forming a
  software "profile" particular to that machine.  The same buildout
  files are rearranged to create different instances of software on
  different machines at the same site.  The same software is used to
  perform incremental upgrades to existing buildouts.

  Why Not Make?

    We had previously been using GNU Make for the same task, but my
    clients couldn't maintain the makefiles easily after the
    engagement ended because they were not willing to learn GNU Make's
    automatic variables and pattern rules which I used somewhat
    gratuitously to make my life easier.  I also realized that even I
    could barely read the makefiles after I had been away from them
    for some time.

    Although make's fundamental behavior is very simple, it has a few
    problems.  Because its basic feature set is so simple, and because
    it has been proven to need to do some reasonably complex things,
    many make versions have also accreted features over the years to
    allow for this complexity.  Unfortunately, it was never meant to
    be a full-blown programming language, and the additions made to
    make syntax to support complex tasks are fairly mystifying.
    Additionally, if advanced make features are used within a
    Makefile, it is difficult to create, debug and maintain makefiles
    for those with little "make-fu".  Even simple failing makefiles
    can be difficult to debug.

  Why Not Ant?

    I a big fan of neither Java nor hand-writing XML.  Ant requires I
    use one and do the other.

  Why Not SCons?

    SCons is all about solving problems specific to the domain of
    source code recompilation.  Buildit is much smaller, and more
    general.

  Why Not A-A-P?

    A-A-P was designed from the perspective of someone wanting to
    develop and install a software package on many different
    platforms.  It does this very well, but loses some generality in
    the process.  A-A-P also uses a make-like syntax for describing
    tasks, whereas Buildit uses Python.

  Why not zc.buildout?

    zc.buildout was released after Buildit was already mature.
    Additionally, zc.buildout appears to have a focus on Python eggs
    which Buildit does not.

General Comparisons to Other Dependency Systems

  Buildit is, for better or worse, completely general and very simple.
  It performs OS shell tasks and calls in to arbitrary Python as
  necessary only as specified by the recipe-writer, rather than
  relying on any domain-specific implicit rules.

  Buildit includes no built-in provisions for building C/Java/C++/etc
  source to object code via implicit or user-defined pattern rules.
  In fact, it knows nothing whatsoever about creating software from
  source files into binaries.

  Unlike makefiles, Buildit "recipe" files have no intrinsic syntax.
  There are no tabs-vs-spaces issues, default rules, automatic
  variables, or any special kind of syntax at all.  In Buildit, recipe
  files are defined within Python.  If conditionals, looping,
  environment integration, or other advanced features become necessary
  within one of your recipes, rather than needing to spell these
  things within a special syntax, you just use Python instead.

  Unlike make, Buildit does not have the capability to perform
  parallel execution of tasks (although it will not prevent it from
  happening when it calls into make itself).

Tasks

  A Buildit "task" is equivalent to a "rule" in GNUmake.  It is the
  fundamental "unit of work" within buildit.  A task has a name, a set
  of targets, a working directory, a set of commands, and a set of
  dependent tasks.  These are described here.

  name -- A task's name is a freeform bit of text describing the
  purpose of the task.  E.g. "configure python".  Only one name may be
  provided for a task.  A name is required.

  namespaces -- A task's namespaces name is a list or a string
  representing the namespace(s) to which this task belongs.  "Local"
  references interpreted when executing the task will use replacement
  values from each namespace.

  targets -- The files that are created as a result of a successful
  run of this task.  A task's targets are strings specifying the file
  that will be created as a result of this task.  It may include
  buildit interpolation syntax (e.g. '${pkgdir}'), which will be
  resolved against the namespace set just while the task is performed.
  Relative target paths are considered relative to the workdir.

  workdir -- A task's workdir specifies the directory to which the OS
  will chdir before performing the commands implied by the task.  Only
  one workdir may be specified.  A workdir is optional, it needn't be
  specified in the task argument list.

  commands -- A task's command set is a list or tuple specifying the
  commands that do the work implied by the task, which, as a general
  rule, should involve creating the target file.  The command set is
  typically a sequence of strings, although in addition to strings,
  special Python callable objects may be specified as a command.  The
  strings that make up commands are resolved against the replacement
  dictionary for string interpolation.  If only one string command is
  specified, it may be specified without embedding it in a list or a
  tuple (the same does not hold true for a single callable Python
  object used as a command, it must be embedded in a list or tuple).

  dependencies -- A task's dependency set is a sequence of other Task
  instances upon which this task depends.  This is the way a
  dependency graph of tasks is formed.  If only one dependency is
  specified, it may be specified without embedding it in a sequence.

Task Example

  Here is an example task, which implies the work required to run
  'configure' within a Python source tree::

     configure = Task(
         'configure python',
         namespaces = 'python',
         targets = '${sharedir}/build/${pkgdn}/Makefile',
         workdir = '${sharedir}',
         # we build Python using --enable-shared in order to allow plpython
         # to build against us on non-32-bit systems.  It appears that this
         # isn't necessary on 32-bit systems (neither Linux nor Mac), but
         # required at least for x86_64 Linux.
         commands = [

         "mkdir -p build/${pkgdn}",

         "cd build/${pkgdn} && ${sharedir}/src/${pkgdn}/configure \
              --prefix=${sharedir}/opt/${pkgdn} --enable-shared",

         ],
         dependencies = (unpack, gcc4_patch_readline_c, py243_socket_patch)
         )

  The Description

    The description of a task is just a string label.  It is printed
    when Buildit is run to help you track down problems and give users
    a sense of what is happening when your recipes are run.  It is
    required.  In the above example, the description is 'configure
    python'.

  The Namespaces

    The namespaces of a task represent each namespace which it will
    attempt to use to resolve local names (e.g. ${./local} names).  In
    the above example, the namespace is 'python'.

    If a task is provided a namespaces argument which is a single
    string with no spaces it it, it will be considered to have a
    single namespace.

    A task may have multiple namespaces.  If a task has multiple
    namespaces, it will be executed once for each namespace in the
    list provided.  For convenience, if a string with spaces in it is
    provided as the 'namespaces' attribute, it is parsed into a list
    of namespace names (this is mostly to work around the inability to
    define lists easily in ConfigParser format).

  The Targets

    The targets of a task are the files that are meant to be created
    by the commands specified within the task.  Although the commands
    of a task may create many files and perform otherwise arbitrary
    actions, the target files are the files that must be created for
    Buildit itself to consider the task "complete".  It may (and almost
    certainly will) require replacement interpolation.  If only one
    target file is required, it can be specified as a string.  If more
    than one target file is necessary, they must be supplied as
    strings within a Python sequence. We only have one target above.
    The target of our example above is
    '${sharedir}/build/${pkgdn}/Makefile'.

    A target file is not considered to be specified relative to the
    working directory: it must be an absolute path or must be
    specified relative to the current working directory from which the
    Buildit driver is invoked.  However, it can contain interpolation
    syntax that will be resolved against the replacement object.

    A target is optional.  If a task has no targets, it will be run
    unconditionally by Buildit on each invocation of the recipe in
    which it is contained.

    If all of a task's commands are run and the target files are not
    subsequently available on the filesystem, Buildit will throw an
    error.

    Buildit automatically "touches" target files after they've been
    created on the filesystem, so the date of all target files after a
    Buildit run will be close to "now", so there's no need to "touch"
    the target files manually.

  The Working Directory

    In the example task above, we specify a working directory
    ('${sharedir}').  The working directory indicates the directory
    into which we will tell the OS to chdir to before performing the
    commands indicated by the task.  This is useful because it allows
    us to specify relative paths in commands which follow.  When the
    task is finished, the working directory is unconditionally reset
    to the working directory that was effective before the task
    started.  Task working directories take effect for only the
    duration of the task.  Using a workdir is optional.  If a workdir
    is not specified, the commands of the task will execute in the
    context of the working directory of the shell used to invoke the
    recipe file.

  The Commands

    In the example task shown above, we've specified two
    commands.  The first one is::

      "mkdir -p build/${pkgdn}"

    The second
    is::

       "cd build/${pkgdn} && ${sharedir}/src/${pkgdn}/configure \
              --prefix=${sharedir}/opt/${pkgdn} --enable-shared"

    Each command is a shell command.  In this case, the shell commands
    are UNIX shell commands.

    The first command creates a build directory (in this case,
    relative to the workdir directory '${sharedir}').  The second
    changes the working directory to the newly-created build directory
    and runs the 'configure' script in the Python source tree with the
    "prefix" and "enable-shared" options.  Note that each commands is
    interpolated against the namespace provided to the task.  Thus if
    'pkgdn' was 'Python-2.4.3' and 'sharedir' was '/tmp', the command
    would be expanded during execution like so::

      mkdir -p build/Python-2.4.3

      cd build/Python-2.4.3 && /tmp/src/Python-2.4.3/configure \
              --prefix=/tmp/opt/Python-2.4.3 --enable-shared

    Note that the commands are executed serially in the order
    specified within the command set.  Each command specified as a
    string is executed by Python's 'os.system'.  If any command fails,
    (where "failure" is interpreted as a shell command exiting with a
    nonzero exit code), an error will be raised.

    Commands that aren't strings are assumed to be Python callables.
    This is not evident in the above example, but you may provide as a
    command a Python callable with a particular interface (see the
    commandlib module for examples).  These kinds of commands are not
    executed by Python's 'os.system'; instead the callable is expected
    to do the work itself instead of delegating to the OS shell,
    although it is free to do whatever it needs to do (eg. the
    callable may do its own delegation to the OS shell if necessary).

  The Dependencies

    The dependency set of a task (specified by 'dependencies' in a
    Task constructor) identifies other Task instances upon which this
    task is dependent.  "Is dependent" in the previous sentence means
    that the dependent task(s) must be completed before the task which
    declares it as a dependency may be run.  Buildit has a simple
    algorithm for determining task and dependency "completeness"
    specified within "Task Recompletion Algorithm" later in this
    document.

    The dependency set of the example above is '(unpack,
    gcc4_patch_readline_c, py243_socket_patch)', which implies that
    the tasks named 'unpack', 'gcc4_patch_readline_c', and
    'py243_socket_patch' must be completed before we can run the
    'configure' task.  The dependent tasks are not shown in the
    example, but for example, the 'unpack' task is presumably the task
    that places the Python source files into
    '${sharedir}/src/${pkgdn}'.

    A task needn't specify any dependencies.  A task may specify a
    single dependency as a reference to a single Task instance, or it
    may specify a sequence of references to Task instances by
    embedding them in a list.

Task Hints

  Tasks should be written with the expectation that they will be run
  more than once.  For instance, if you create a symlink a directory
  within a command, the command should first check if a symlink
  already exists at that location, or you'll quite possibly end up
  symlinking the directory inside the existing symlinked directory on
  subsequent runs.

Namespaces

  A Buildit namespace is a mapping of names to values.  These mappings
  are used within tasks to perform textual variable replacement (which
  is also known as "interpolation").

  Namespaces are user-defined.  Multiple user-defined namespaces will
  typically exist during a given Buildit execution.  All values in a
  given namespace are typically related to each other.  For example, a
  'squidinstance' namespace might represent all of the names and
  replacement values required to create an instance of the Squid proxy
  server.  A 'pound' namespace might represent all of the names and
  replacement values required to create an installation of the Pound
  load balancer.

  A namespace is typically declared within one "section" of a
  Windows-style ".INI" file.  Names within a namespace must consist
  solely of alphanumeric characters, the underscore, and the minus
  sign.  The value for a name can be any set of characters and may
  also contain zero or more placeholders which mention other names
  that should be interpolated.  These interpolation targets are known
  as "references", and they consist of a set of characters surrounded
  by squiggly brackets prefixed with a dollar sign
  (e.g. '${setofcharacters}').  Names and values are separated by any
  number of whitespace characters on either side of an equal sign.

  Here's an example of contents that might go into a namespace .INI
  file::

   [anamespace]
   name1 = this is value one
   name2 = ${./name1} is relative to this namespace
   name3 = ${globalname}
   name4 = ${external/name}
   name5 = ${./name1} hello ${globalname}

  If you are examining the above example, you might note that there
  are four main types of strings which are allowed to compose a
  value:

   - String literals.  In the above example, the string literal "this
     is value one" is assigned to the 'name1' name.

   - Rererences to "global" names, which are names which must be found
     in the "default" namespace.  In the example above, the value of
     name 'name3' has a reference to the global name 'globalname'.
     Global names never have a slash character in them; they are
     always simple names without any prefixes.

   - References to "external" names, which are names that are found in
     other namespaces.  In the example above, 'name4' refers to one
     external name, "${external/name}", which refers to the name
     'name' in the external namespace named 'external'.  External
     names always contain one slash, which separates the namespace
     name from the name that is to be looked up.  If the 'external'
     namespace contained a name called 'name' with a value of 'foo',
     the external reference in the example would resolve to "foo".

   - References to "local" names, which are pointers to the values of
     names which are found in the same namespace as the name being
     defined.  In the above example, the expansion of the value
     "${./name1} is relative to this namespace" in the local name
     'name2' would become "this is value one is relative to this
     namespace".  A "local" name always starts with the prefix "./".
     Essentially, local names are external names where the namespace
     name is ".".

The Root .INI File

  In order to begin a Buildit project, first create a file named
  "root.ini" with the following content in Windows-style .INI
  format)::

    [defaults]
    tgtdir = ${cwd}/sandbox

    [namespaces]
    foo = ${buildoutdir}/foo.ini [1.0]
    bar = ${buildoutdir}/bar/ini [1.0]

  It really doesn't matter what you name this file but for sake of
  reference let's say it's named "root.ini".

  Note that the file consists of two sections: a 'defaults' section,
  and a 'namespaces' section.

  The 'defaults' section allows you to define names and values which
  end up in the "global" namespace (see the 'Namespaces' section for a
  definition).

  The 'namespaces' section allows you to declare namespaces that will
  be used during the execution of Buildit.  One or more lines may be
  defined within the namespaces section.  Each line defines a
  namespace, and is composed of the following:

   - a name.  In the above example, the namespaces 'foo' and 'bar' are
     declared.

   - a filename and a section name.  In the above example, the section
     named "1.0" in the file named "${buildoutdir}/foo.ini" is used
     for the foo namespace.  A space must separate the filename and
     the section name, and the section name must be surrounded by
     brackets.

  In all values within the root .INI file (the default values or the
  namespace values), you can use the following "built-in" global
  replacement values:

    ${cwd} -- the fullly-qualified path to the initial working
    directory of the process which invoked buildit.

    ${buildoutdir} -- the fullly-qualified path to the directory which
    contains the Python file that first invokes Buildit.

    ${username} -- the user name of the user who invoked the Python
    file that first invokes Buildit.

    ${platform} -- the value returned by distutils util.get_platform()
    function

  These names are also available in the default namespace when
  declaring values for other namespaces and running buildit tasks.

The Namespace .INI Files

  Each name in the 'namespaces' section referred to within the "root'
  .INI file must exist on disk.  Additionally, the section within the
  file that is mentioned on the value line must be contained within
  the named file.

  If your root .INI file declares the following namespace section::

    [namespaces]
    breakfast = ${buildoutdir}/breakfast.ini [1.0]
    dinner    = ${buildoutdir}/dinner.ini [1.0]

  .. then two additional .ini files need to exist on your filesystem,
  'breakfast.ini' and 'dinner.ini'.  In general, these must should
  live relative to the directory in which the python file which will
  initially invoke buildit (the "buildoutdir") lives.  And in this
  case, 'foo.ini' and 'bar.ini' both need to have a section named
  '1.0' which contains one or more key/value pairs that make up the
  namespace content.

  Without explaining much about what it means, here's an example of
  what might go in the "breakfast.ini" we've threatened to define
  above within our root .ini file::

   [1.0]
   orderer = ${username}
   coffeesize = large
   coffeetype = espresso
   coffeeorder = ${./coffeesize} ${./coffeetype}
   bageltype = plain

  Here's an additional example of what might go into the "dinner.ini"
  we've additionally threatened to define::

   [1.0]
   orderer = ${username}
   coffeesize = small
   coffeetype = ${breakfast/coffeetype}
   coffeeorder = ${./coffeesize} ${./coffeetype}
   breadtype  = dinner roll

  What's most interesting about what will "fall out" of these
  definitions is the result of the variable expansion.  Again, without
  explaining why, assuming the current user name of the account
  running the Buildit process is "chrism", here's what the
  replacements would expand to in the 1.0 section of "breakfast.ini"::

   orderer = chrism
   coffeesize = large
   coffeetype = espresso
   coffeeorder = large espresso
   bageltype = plain

  And here's what the replacements would expand to in the 1.0 section
  of "dinner.ini"::

   orderer = chrism
   coffeesize = small
   coffeetype = espresso
   coffeeorder = small espresso
   breadtype = dinner roll

  During namespace processing, note that replacement targets can be
  replaced with global, local, or external values.

  It is an error to create two files which contain namespaces which
  depend on each other's names circularly, and it's an error to refer
  to a local, global, or external name that cannot be resolved because
  it does not exist.  When Buildit is run, these types of errors are
  detected and presented to the person executing the Buildit script
  before any work is actually performed.

Driving Buildit

  An example of kicking off a Buildit process::

   # framework hair
   from buildit.context import Context
   from buildit.context import Software

   # your defined "tasks"
   from mytasks import mkbreakfast
   from mytasks import mkdinner

   # read default root .ini from file named "/etc/root.ini" and contextualize
   context = Context('/etc/root.ini') 

   # use section 1.1 for dinner namespace instead of default named in root.ini
   context.set_section('dinner', '1.1') 

   # use a different file and section for breakfast namespace instead of default
   context.set_file('breakfast', '${buildoutdir}/breakfast2.ini', 
                    'coolbreakfast')

   # create a Software instance for both breakfast and dinner
   breakfast = Software(mkbreakfast, context)
   dinner = Software(mkdinner, context)

   # override the section value used for coffeetype and install
   breakfast.set('coffeetype', 'americano')
   breakfast.install()

   # override the section value used for breadtype and install
   dinner.set('breadtype', 'wheat')
   dinner.install()

Driving Buildit More Declaratively via Config File "Instance" Sections

  Optionally, instead of using Python to drive buildit completely
  procedurally, you may choose to define sections within your root
  initialization file which represent software "instances" in the form
  (e.g.)::

    [breakfast:instance]
    buildit_task = mytasks.mkbreakfast
    buildit_order = 10
    coffeetype = americano

    [dinner:instance]
    buildit_task = mytasks.mydinner
    buildit_order = 20
    breadtype = wheat

  Section headers for instance definitions must end in ':instance'.
  The text that comes before ':instance' is purely informational.
  They must include a "buildit_task" value, which should be a Python
  dotted name identifier which points to a task instance.  It can
  optionally include a "buildit_order" integer value, which represents
  the instance's execution order relative to the other instances
  defined.  Lower numbered instances are run first.  If an instance
  does not have a buildit_order, it is the same as providing it with a
  zero as a value.

  The example above does the same thing the procedural example does
  above, except we cannot substitute a different ini file or a
  different section name for the breakfast task (there is no analogue
  to set_file or set_section).  All key/value pairs within the section
  which do not begin with "buildit_" are used as override variables
  for the namespace of the task named by the buildit_task dotted name.
  You can choose to put a namespace name after the dotted name value
  of buildit_task (e.g. 'buildit_task = mytasks.mydinner [breakfast]')
  to change the namespace in which these overrides will be performed.

  Once you've added instance sections, you can drive your buildout by
  using the boilerplate script (assuming /etc/root.ini is your config
  file)::

    from buildit.context import Context

    def main(defaults_ini):
        context = Context(defaults_ini)
        context.install()

    if __name__ == '__main__':
        import sys
        main('/etc/root.ini')
  
Task Recompletion Algorithm

  A task is considered to be complete if all of the following
  statements can be made about it:

   - it specifies one or more target files in the task definition

   - all of its target files exist

  If a task does not meet these completion requirements at any given
  time, on a subsequent run of the recipe file in which it defined (or
  from a recipe file in which it is imported and used), its commands,
  *and all the commands of the tasks which are dependent upon it* will
  be rerun in dependency order.

  Buildit (unlike make) does not take into account the timestamp of a
  task's dependent targets when assessing whether a task needs to be
  recompleted.

Command Library

  The buildit command library includes a number of standard command
  types that can be used in place of shell commands in the 'commands='
  argument to a task.  Each argument to the command can be a string
  literal or it can be a string which include expansion markers.
  These commands are available via 'from buildit.commandlib import X"
  where X includes:

  Download -- Download(filename, url, remove_on_error=True).

  CVSCheckout -- Checkout(repo, dir, module, tag='').  repo is the
  :ext: or :pserver: name of the repository including its path on the
  remote server's disk, dir is the directory into which we wish to
  check the module out, module is the CVS module name of the module,
  tag is the tag name, including the '-r '.

  Fetch -- Fetch(url, namem, using, path, version='').  Omnibus
  fetcher including features of "Download" and "CVSCheckout".  If
  'using' is 'svn', the command generated is 'svn co %(url)s
  %(name)s%(version)s'.  If 'using' is 'cvs', the command generated is
  'cvs -d %(url)s co -d %(name)s%(version)s %(path)s'.  If 'using' is
  'download', the command generated is something along the lines of
  "wget -q -O - %(url)s ' '| tar xz && mv %(name)s %(name)s%(version)s'"

  Symlink -- Symlink(frm, to)

  Patch -- Patch(file, patch, level).  'patch' is the patchfle to
  apply to 'file', 'level' is the argument to apply to 

  InFileWriter -- InFileWriter(infile, outfile, mode=0755).  Replaces
  all <<HUGGED>> text in infile with an expansion based on the current
  namespace and the global namespace.  Changes mode of outfile to
  mode.

  Substitute -- Substitute(filename, search_re, replacement_string,
  backupext='.~subst').  Replaces all text in 'filename' matching the
  regular expression string 'search_re' with the replacement_string.
  Make a backup of the original with the original filename plus the
  backup extension.

  SkelCopier -- SkelCopier(skeldir, tgtdir). Makes an exact copy of
  one directory ('skeldir') to another ('tgtdir').  If a file in the
  source directory has a .in extension, replace its <<HUGGED>> values
  with task interpolated values, and write it to the target directory
  without the .in extension.

Reporting Bugs

  Please use "the buildit issue
  tracker":http://agendaless.com/Members/chrism/software/buildit_issues
  to report issues.

Have fun!

Chris McDonough (chrism@agendaless.com)

