Content:
    This directory contains functionality that extracts documentation string
from C++ source files. 

How to integrate the functionality with pyplusplus?
 
  mb = module_builder_t( ... )
  mb.build_code_creator( ..., doc_extractor=my_doc_extractor )

What is "my_doc_extractor"? Well, "my_doc_extractor" is callable object,
that takes only one argument - declaration reference and returns documentation
string. Something like this:

def my_doc_extractor( decl ):
    return decl.location.file_name + str( decl.location.line ) 
    
Yes, every declaration contains next information:
  1. Full path to file it was defined in.
  2. Line number.
