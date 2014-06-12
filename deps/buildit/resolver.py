import re

# "bracketed" names are meant to be any of:
# ${foo} (name foo available in defaults)
# ${./foo} (foo defined within the section in which the value lives)
# ${foo/bar} (bar available within the foo namespace)
# ${foo/bar/baz} (bar/baz available within the foo namespace)

# the TOKEN_RE below will also find, for example
# ${/foo/bar}
# ${foo/}
# even though the meaning of these names is undefined

TOKEN_RE = re.compile(r'\$\{([\.\w/-]+)\}')
DEFAULT = ()
LOCAL = '.'

class DependencyError(ValueError):
    pass
    
class CyclicDependencyError(DependencyError):
    def __str__(self):
        L = []
        cycles = self.args[0]
        for cycle in cycles:
            section, dependent = cycle
            dependees = cycles[cycle]
            L.append('In section %r, option %r depends on options %r' % (
                        section, dependent, dependees)
                     )
        msg = '; '.join(L)
        return '<CyclicDependencyError: %s>' % msg

class MissingDependencyError(DependencyError):
    def __init__(self, section_name, option_name, value, offender):
        self.args = [(section_name, option_name, value, offender)]
        self.section_name = section_name
        self.option_name = option_name
        self.value = value
        self.offender = offender

    def __str__(self):
        return (
            '<MissingDependencyError in section named %r, option named '
            '%r, value %r, offender %r>' % (
            self.section_name, self.option_name, self.value, self.offender)
            )

def resolve(sections, defaults=None, overrides=None):
    """
    Resolve the replacement values in the dict of unresolved values
    named 'sections'.
    """
    if defaults is None:
        defaults = {}
    if overrides is None:
        overrides = {}

    missing = []
    sections_copy = sections.copy()

    # overrides is a dict of section names to override dictionaries
    # XXX this really should feel better
    for secname, odict in overrides.items():
        if sections_copy.has_key(secname):
            for k, v in odict.items():
                sections_copy[secname][k] = v

    # we need to process the sections in a resolveable order based on the
    # replacement values they contain, relative to one another.
    ordered_sections = section_resolution_order(sections_copy)

    for section_name in ordered_sections:
        # mutate the copy as we resolve values; each successive call to
        # resolve_options will be using the most "fully resolved" version of the
        # copy of the sections dictionary possible.  Because the sections
        # are processed in order, transitive replacements should work
        # properly.
        sections_copy[section_name] = resolve_options(
            section_name, sections_copy, defaults)

    return sections_copy
        
def section_resolution_order(sections):
    order = []
    items = []

    for section_name in sections.keys():
        items.append(section_name)
        options = sections[section_name]
        for k, v in options.items():
            try:
                tokens = TOKEN_RE.findall(v)
            except TypeError:
                raise TypeError('value %r must be a string' % v)
            for token in tokens:
                namespace, name = interpret_substitution_token(token)
                if namespace not in (DEFAULT, LOCAL):
                    order.append((namespace, section_name))

    return topological_sort(items, order)

def resolve_options(section_name, sections, defaults=None, overrides=None):
    """
    Given a section_name and a sections dict for which
    sections[section_name] contains one or more 'options' dictionaries
    with string keys and values referencing those keys,
    e.g. ${./localname}' and ${globalname} and ${external/name},
    resolve the values and return a 'resolved' copy of the options
    dictionary referred to by 'section_name'.  Use the 'defaults'
    dictionary passed in to resolve 'default' names, use the local
    section to resolve 'local' names, and use the 'sections'
    dictionary passed in to resolve 'external' names.  This function
    will only expand things properly for external names if the
    dependents of 'section_name' have already been resolved within
    'sections'.
    """
    if defaults is None:
        defaults = {}
    if overrides is None:
        overrides = {}

    options = sections[section_name]
    options_copy = options.copy()
    sections_copy = sections.copy()
    options_copy.update(overrides)
    sections_copy[section_name] = options_copy

    missing = []

    # we need to process the option values in a resolveable order based on the
    # replacement values they contain, relative to one another.
    options = sections_copy[section_name]
    index, ordered_keys = option_resolution_order(options_copy, section_name)

    for option_name in ordered_keys:
        # mutate the copy as we resolve values; each successive call to
        # resolve_option will be using the most "fully resolved" version of the
        # copy of the options dictionary possible.  This is how transitive
        # replacements work.
        v = options_copy.get(option_name)
        if v is None:
            # the user may have supplied a missing option name
            offenders = index[option_name]
            value, offender = offenders[0]
            raise MissingDependencyError(section_name, option_name, offender,
                                         value)
        else:
            options_copy[option_name] = resolve_value(
                    v, section_name, sections_copy, defaults, option_name
                    )

        sections_copy[section_name] = options_copy

    return options_copy
    
def option_resolution_order(options, section_name=None):
    order = []
    items = []
    index = {}

    for k, v in options.items():
        items.append(k)
        try:
            tokens = TOKEN_RE.findall(v)
        except TypeError:
            raise TypeError('value %r must be a string' % v)
        for token in tokens:
            namespace, name = interpret_substitution_token(token)
            # we don't resolve external references here, and global references
            # aren't required to be resolved in any particular order because
            # they're by nature fully qualified, so we just need to deal with
            # local values when determining a resolution order
            if namespace == LOCAL:
                order.append((name, k))
                L = index.setdefault(name, [])
                L.append((k, v))

    return index, topological_sort(items, order, section_name=section_name)

def resolve_value(value, section_name, sections, defaults=None,
                  option_name=None):
    if defaults is None:
        defaults = {}
    options = sections.get(section_name)

    try:
        tokens = TOKEN_RE.findall(value)
    except TypeError:
        raise TypeError('value %r must be a string' % value)

    missing = []
    orig_value = value

    for token in tokens:
        namespace, name = interpret_substitution_token(token)
        if namespace == DEFAULT:
            lookup = defaults
        elif namespace == LOCAL:
            if options is None:
                raise MissingDependencyError(section_name, option_name,
                                             orig_value, name)
            lookup = options
        else:
            lookup = sections.get(namespace, {})

        substring = '${%s}' % token
        option = lookup.get(name)
        if option is None:
            raise MissingDependencyError(section_name, option_name, orig_value,
                                         name)
        value = value.replace(substring, lookup[name])

    return value

def interpret_substitution_token(token):
    tup = token.split('/', 1)
    if len(tup) == 1:
        return DEFAULT, token
    else:
        return tup # namespace, name

def topological_sort(items, partial_order, section_name=None,
                     ignore_missing_partials=True):

    """ Stolen from http://www.bitinformation.com/art/python_topsort.html

    Given the example list of items ['item2', 'item3', 'item1',
    'item4'] and a 'partial order' list in the form [(item1, item2),
    (item2, item3)], where the example tuples indicate that 'item1'
    should precede 'item2' and 'item2' should precede 'item3', return
    the sorted list of items ['item1', 'item2', 'item3', 'item4'].
    Note that since 'item4' is not mentioned in the partial ordering
    list, it will be at an arbitrary position in the returned list.

    """

    def add_node(graph, node):
        if not graph.has_key(node):
            graph[node] = [0] # 0 = number of arcs coming into this node

    def add_arc(graph, fromnode, tonode):
        graph[fromnode].append(tonode)
        graph[tonode][0] = graph[tonode][0] + 1

    graph = {}

    for v in items:
        add_node(graph, v)

    for a, b in partial_order:
        if ignore_missing_partials:
            # don't fail if a value is present in the partial_order
            # list but missing in items.  In this mode, we fake up a
            # value instead of raising a KeyError when trying to use
            # add_arc in order to be able to produce errror reports
            # that aggregate both local and default error conditions
            # in callers.
            if not graph.has_key(a):
                graph[a] = [0]
            elif not graph.has_key(b):
                graph[b] = [0]
            else:
                add_arc(graph, a, b)
        else:
            add_arc(graph, a, b)

    roots = [ node for (node, nodeinfo) in graph.items() if nodeinfo[0] == 0 ]

    sorted = []

    while len(roots) != 0:
        root = roots.pop()
        sorted.append(root)
        for child in graph[root][1:]:
            graph[child][0] = graph[child][0] - 1
            if graph[child][0] == 0:
                roots.append(child)
        del graph[root]

    if len(graph) != 0:
        # loop in input
        cycledeps = {}
        for k, v in graph.items():
            cycledeps[(section_name, k)] = v[1:]
        raise CyclicDependencyError(cycledeps)

    return sorted
