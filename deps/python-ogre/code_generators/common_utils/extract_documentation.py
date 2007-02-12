"""
extracting from C++ doxygen documented file
Author G.D.

Updates for Ogre handling by AJM - added remarks/note/par
jan 03 06 - fix bug, replace single leading \ with \\ in clear_str
"""

class doc_extractor:
    """
    extracts doxigen styled documentation from source
    or generates from description
    """
    def __init__(self, startswith):
        #for caching source
        self.file_name  = None
        self.source = None
        self.startswith = startswith
        
    def __call__(self, declaration):
        #print "doc_extractor called with:", declaration
        if not declaration.decl_string.startswith(self.startswith):
            return ""
        try:    # some types such as the base namespace don't have 'location'
            if self.file_name != declaration.location.file_name:            
                self.file_name = declaration.location.file_name
                self.source = open(declaration.location.file_name).readlines()
        except:
            print "DOC without location:", declaration
            return ""
            
        find_block_end = False
        failed = 0
        doc_lines = []
        
        ## note the gccxml uses a 1 based line scheme whereas we are using python arrays - 0 based
        ## so we need to subtract 1 from the line number.
        
        ## then we need to subtract another 1 to put us on the line above the function/class etc defination
        
        ## AND to make it worse, Ogre coding style often has the opening brace on the line AFTER the defination name
        ## and gccxml gives us the line number with the brace - so we need to look back even further
        
        ## lets look for a single comment '///'  We'll try upto 2 lines above
        try:
            for lcount in xrange(declaration.location.line-1, declaration.location.line-4 , -1):
                line = self.source[lcount]
                if line.lstrip()[:3] == '///':
                    str = clear_str(line)
                    retvalue = '"' + str[:-2] + '"' # remove the \n from the single line comment
                    #print "Extracted Doc String (short) ", retvalue
                    return retvalue
                    
            ## if that didn't work we'll look for a block of comments
            
            for lcount in xrange(declaration.location.line - 1, -1, -1):
                line = self.source[lcount]
                #print "Checking ", lcount, line
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
                    ## note that ogre code tends to have the opening function/class bracket on the 'next' line 
                    ## from the 'name' -so we may need to backup one more line to fine doc string..
                    failed +=1
                    final_str = ""  ## make sure we don't add the extra code line
                    if failed > 2:
                        break
                if final_str:
                    doc_lines.insert(0, clear_str(final_str))
        except:
            return ""   ## problem finding comments
        if doc_lines:
            #print "Extracted Doc String for:",  declaration, "[", len(doc_lines),"]"
            ## we need to cope with strings longer than 2048 for MSVC
            ret1 = '"' + "\\\n".join(doc_lines) + '"'
            ret =""
            ## Py++ doesn't like non ascii characters in the doc string
            ## class_declaration.py - _generate_constructor - return ( ''.join( result ), used_init )
            for c in ret1:
                if ord(c) >127:
                    c = ' '
                ret = ret + c
            if len ( ret ) < 1700:  ## just to be safe and adjust for line end chars etc..
                return ret  ## OK we don't need to do anything special...
            ret =  '"'
            length = 1
            for line in doc_lines:
                ret = ret + line
                length += len ( line )
                if length < 1500 :  # again lets be conservative
                    ret = ret + "\\\n"
                else :
                    ret = ret + '"' + "\n" + '"'    # we close the original 'quote', move to a new line and open a new quote
                    length = 1
            
            return ret + '"'     
        #print "Doc String not found for", declaration  
        return ""
    
def get_generic_doc(declaration):
    """
    generate call information about function or method
    """
    try:
        return '"' + "Help on %s\n" % str(declaration) + '"'
    except:
        pass
    
    return ''
                

def clear_str(str):
    """
    replace */! by Space and \breaf, \fn, \param, ...
    """
    def clean ( str, sym, change2 = ""): 
        return str.replace(sym, change2)
    str = clean(str, "@param", "Param: ")
    str = clean(str, "\\param", "Param: ")
    str = clean(str, "@ingroup", "Group")
    str = clean(str, "\\ingroup", "Group")
    str = clean(str, "@return", "It return")
    str = clean(str, "\\return", "It return")
    str = clean(str, "@note", "Note: ")
    str = clean(str, "@remarks", "Remarks: ")
    str = clean(str, "@see", "See: ")
    str = clean(str, "\\see", "See: ")
    str = clean(str, "@ref", "Ref: ")
    str = clean(str, "\\ref", "REf: ")
    
    str = clean(str, "\\sa", "See also: ")   # comment string in OgreNewt
    str = clean(str, "\\code", "Code: ")    
    str = clean(str, "\\codeblock", "CodeBlock: ")    
    str = clean(str, "\\endcode", "")    
    str = clean(str, "@code", "")    
    str = clean(str, "@codeblock", "")    
    
    str = clean(str, "@par", "")    ## it will get a single blank line by default
    str = clean(str, "\\par", "")    ## it will get a single blank line by default
    str = clean(str, "\n", "\\n") 
    str = clean(str, "\\p", "")     ## cegui comments
    str = clean(str, "\\exception", "Exception: ") 
    str = clean(str, "::", ".")     ## make it more python accurate 
    str = clean(str, "->", ".") 
    
    ## now clean up the rest
    str = reduce(clean, [str, '/', '*', '!', "\\brief", '\\fn',\
     "@brief", "@fn", '"', "@{", "\\c", "\\a"]) ## somtimes there are '"' in the doc strings and other "\\"...
    return str.lstrip()
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
            self.decl_string = ""
            
        def __str__(self):
            return self.str
        
    print doc_extractor("")(x_decl("myfunc(int x, int y)","c:/development/ocvs/ogrenew/ogremain/include/OgreSceneManager.h",218))
    print doc_extractor("")(x_decl("","c:/development/ocvs/ogrenew/ogremain/include/OgreSceneManager.h",223))
    
    print doc_extractor("")(x_decl("","c:/development/CEGUI-0.5.0/include/CEGUIEvent.h",139))
    

            
