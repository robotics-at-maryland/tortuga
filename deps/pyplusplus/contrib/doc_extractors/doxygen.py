"""
extracting from C++ doxygen documented file
Author G.D.
"""

class doc_extractor:
    """
    extracts doxigen styled documentation from source
    or generates from description
    """
    def __init__(self):
        #for caching source
        self.file_name  = None
        self.source = None
        
    def __call__(self, declaration):
        try:
            if self.file_name != declaration.location.file_name:            
                self.file_name = declaration.location.file_name
                self.source = open(declaration.location.file_name).readlines()
                
            find_block_end = False
            doc_lines = []
            for lcount in xrange(declaration.location.line - 1, -1, -1):
                line = source[lcount]
                if not find_block_end:
                    try:
                        if line.rstrip()[-2:] == "*/":
                            find_block_end = True
                    except:
                        pass
                if  find_block_end:
                    try:
                        if line.lstrip()[:2] == "/*":
                            find_block_end = False
                    except:
                        pass
                final_str = clear_str(line)
                if not find_block_end and code(line):
                    break
                if final_str:
                    doc_lines.insert(0, final_str)
            
            if doc_lines:
                doc_lines.insert(0, self.get_generic_doc())
                return ''.join(doc_lines)
                
        except:
            pass
        
        return self.get_generic_doc(declaration)
    
    def get_generic_doc(self, declaration):
        """
        generate call information about function or method
        """
        try:
            return "Help on %s\n" % str(declaration)
        except:
            pass
        
        return ''
                

def clear_str(str):
    """
    replace */! by Space and \breaf, \fn, \param, ...
    """
    clean = lambda str, sym, change2 = '': str.replace(sym, change2)

    str = reduce(clean, [str, '/', '*', '!', "\brief", "\fn",\
     "@brief", "@fn", "@ref", "\ref"])
     
    str = clean(str, "@param", "Param: ")
    str = clean(str, "\param", "Param: ")
    str = clean(str, "@ingroup", "Group")
    str = clean(str, "\ingroup", "Group")
    str = clean(str, "@return", "It return")
    str = clean(str, "\return", "It return")
    return "  " + str.lstrip()

    
def code(str):
    """
    detect str is code?
    """
    try:
        beg = str.lstrip()[:2]
        return  beg != "//" and beg != "/*"
    except:
        pass
    return False

if __name__ == '__main__':
    class loc:
        def __init__(self, f, l):
            self.file_name = f
            self.line = l
            
    class x_decl:
        def __init__(self, str, file_name, line):
            self.str  = str
            self.location = loc(file_name, line)
            
        def __str__(self):
            return self.str
        
    print doc_extractor()(x_decl("myfunc(int x, int y)","core.h",45))
    print doc_extractor()(x_decl("","core.h",209))
    

            
