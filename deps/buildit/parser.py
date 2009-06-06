import re
import ConfigParser

class BuilditConfigParser(ConfigParser.RawConfigParser):

    # we override the OPTCRE expression below to only consider '=' a
    # separator (not = and :).

    OPTCRE = re.compile(
        r'(?P<option>[^=\s][^=]*)'
        r'\s*(?P<vi>[=])\s*'                  # any number of space/tab,
                                              # followed by = separator
                                              # followed by any # space/tab
        r'(?P<value>.*)$'                     # everything up to eol
        )

    def optionxform(self, v):
        # do not munge case as RawConfigParser does.
        return v

    def _read(self, fp, fpname):
        """Parse a sectioned setup file.

        The sections in setup file contains a title line at the top,
        indicated by a name in square brackets (`[]'), plus key/value
        options lines, indicated by `name: value' format lines.
        Continuations are represented by an embedded newline then
        leading whitespace.  Blank lines, lines beginning with a '#',
        and just about everything else are ignored.
        """
        cursect = None                            # None, or a dictionary
        curxformer = None
        optname = None
        lineno = 0
        e = None                                  # None, or an exception
        while True:
            line = fp.readline()
            if not line:
                break
            lineno = lineno + 1
            # comment or blank line?
            if line.strip() == '' or line[0] in '#;':
                continue
            # continuation line?
            if line[0].isspace() and cursect is not None and optname:
                value = line.strip()
                if value:
                    old = cursect[optname]
                    if curxformer:
                        value = extenders[curxformer](old, value)
                    else:
                        cursect[optname] = "%s\n%s" % (old, value)
            # a section header or option header?
            else:
                # is it a section header?
                mo = self.SECTCRE.match(line)
                if mo:
                    sectname = mo.group('header')
                    if sectname in self._sections:
                        cursect = self._sections[sectname]
                    elif sectname == ConfigParser.DEFAULTSECT:
                        cursect = self._defaults
                    else:
                        cursect = {'__name__': sectname}
                        self._sections[sectname] = cursect
                    # So sections can't start with a continuation line
                    optname = None
                # no section header in the file?
                elif cursect is None:
                    raise ConfigParser.MissingSectionHeaderError(fpname,
                                                                 lineno, line)
                # an option line?
                else:
                    mo = self.OPTCRE.match(line)
                    if mo:
                        optname, vi, optval = mo.group('option', 'vi', 'value')
                        if vi == '=' and ';' in optval:
                            # ';' is a comment delimiter only if it follows
                            # a spacing character
                            pos = optval.find(';')
                            if pos != -1 and optval[pos-1].isspace():
                                optval = optval[:pos]
                        optval = optval.strip()
                        # allow empty values
                        if optval == '""':
                            optval = ''
                        optname = optname.rstrip()
                        optname, optval, curxformer = typexform(optname, optval,
                                                                fpname, lineno,
                                                                line)
                        cursect[optname] = optval
                    else:
                        # a non-fatal parsing error occurred.  set up the
                        # exception but keep going. the exception will be
                        # raised at the end of the file and will contain a
                        # list of all bogus lines
                        if not e:
                            e = ConfigParser.ParsingError(fpname)
                        e.append(lineno, repr(line))
        # if any parsing errors occurred, raise an exception
        if e:
            raise e

def xformtokens(s):
    return [x.strip() for x in s.split()]

def extendtokens(old, new):
    new = xformtokens(new)
    old.extend(new)
    return old

xforms = {'tokens':xformtokens}
extenders = {'tokens':extendtokens}

def typexform(optname, optval, fpname, lineno, line):
    if not ':' in optname:
        return optname, optval, None
    optname, xformername = optname.split(':', 1)
    xformer = xforms.get(xformername)
    if xformer is None:
        e = ConfigParser.ParsingError(fpname)
        e.append(lineno, repr(line))
        raise e
    return optname, xformer(optval), xformername

def parse(filename, defaults=None):
    sections = {}

    if defaults is None:
        defaults = {}
    if hasattr(filename, 'readline'):
        fp = filename
    else:
        fp = open(filename, 'r')

    parser = BuilditConfigParser(defaults)
    parser.readfp(fp)

    for section in parser.sections():
        sections[section] = dict(parser.items(section))
        
    return sections
