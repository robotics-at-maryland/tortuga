import os
import shared_ptr
from pygccxml import declarations
from pyplusplus.decl_wrappers import property_t

def configure_shared_ptr( mb ):
    exposer = shared_ptr.exposer_t( mb )
    exposer.expose()


class decl_starts_with (object):
    def __init__ (self, prefix):
        self.prefix = prefix
    def __call__ (self, decl):
        return self.prefix in decl.name


## Lets go looking for 'private' OGRE classes as defined by comments etc in the include files
class private_decls_t:
    ## First build a list of include file name/ line number for all instances of 'private' classes
    ## note the index + 'x' to get things to the same line as gcc..  However potential bugs in code
    ## depending upon the include file 'formating'.  We are capturing the line the comment is on, gcc
    ## records the line the opening brace '{' is on...
    def __init__( self, include_dirs ):
        self.__private = {} #fname : [line ]
        for include_dir in include_dirs:    
            for fname in os.listdir( include_dir ):
                full_name = os.path.join( include_dir, fname )
                if not os.path.isfile( full_name ):
                    continue
                fobj = file( full_name, 'r' )
                for index, line in enumerate( fobj ):
                    if '_OgrePrivate' in line:
                        if not self.__private.has_key( fname ):
                            self.__private[ fname ] = []
                        if '{' in line:     ## AJM Ugly hack - assumes there won't be blank lines between class name and opening brace
                            index = index + 1   #enumerate calcs from 0, while gccxml from 1
                        else:
                            index = index + 2   #one line down to the opening brace
                        self.__private[ fname ].append( index ) #enumerate calcs from 0, while gccxml from 1
                    line = line.strip()
    
                    ## Again this next bit assumes an opening brace on the same line as the method or class :(
                    if line.startswith( '/// Internal method ' ) or line.startswith( '/** Internal class' ):
                        if not self.__private.has_key( fname ):
                            self.__private[ fname ] = []
                        self.__private[ fname ].append( index + 2 ) #enumerate calcs from 0, while gccxml from 1
    
                fobj.close()

    def is_private( self, decl ):
        if None is decl.location:
            return False
        file_name = os.path.split( decl.location.file_name )[1]
        return self.__private.has_key( file_name ) and decl.location.line in self.__private[ file_name ]

def fix_unnamed_classes( classes, namespace ):
    for unnamed_cls in classes:
        named_parent = unnamed_cls.parent
        if not named_parent.name:
            named_parent = named_parent.parent

        for mvar in unnamed_cls.public_members:
            if not mvar.name:
                continue

            if declarations.is_array (mvar.type):
                template = '''def_readonly("%(mvar)s", &%(ns)s::%(parent)s::%(mvar)s)'''
            else:
                template = '''def_readwrite("%(mvar)s", &%(ns)s::%(parent)s::%(mvar)s)'''
            named_parent.add_code( template % dict( ns=namespace, mvar=mvar.name, parent=named_parent.name ) )

def set_declaration_aliases(global_ns, aliases):
    for name, alias in aliases.items():
        try:
            cls = global_ns.class_( name )
            cls.alias = alias
            cls.wrapper_alias = alias + '_wrapper'
        except declarations.matcher.declaration_not_found_t:
            global_ns.decl( name, decl_type=declarations.class_declaration_t ).rename( alias )


def add_constants( mb, constants ):
    tmpl = 'boost::python::scope().attr("%(name)s") = %(value)s;'
    for name, value in constants.items():
        mb.add_registration_code( tmpl % dict( name=name, value=str(value) ) )
    
def add_properties( classes ):
    for cls in classes:
        cls.add_properties()

## note that we assume all the properties have been added by ourselves and we checked case insentistively for conflicts
##                
def add_LeadingLowerProperties ( cls ):
    new_props = []
    existing_names = []
    for prop in cls.properties:
        existing_names.append( prop.name)
    for prop in cls.properties:
        name = prop.name[0].lower() + prop.name[1:]
        if name not in existing_names:  # lets make sure it's different
            existing_names.append ( name )
            new_props.append( property_t( name, prop.fget, prop.fset, prop.doc, prop.is_static ) )
    cls.properties.extend( new_props )
    
def extract_func_name ( name ):
    """ extract the function name from a fully qualified description
    """
    end = name.index('(')  # position of open brace, function call is before that
    start = -1
    for x in range ( end, 0, -1 ):   # step back until we find the ':'
        if name[x] == ':' :
            start = x + 1
            break
    ## so now return the function call
    return name[start:end]
        
        
       