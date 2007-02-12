#process_warnings

def go ( cls ) :
    """ handle warnings from py++
    """
    for f in cls.member_functions(allow_empty=True):
        for w in f.readme():
            if 'W1049' in w: 
                print "processing ", f.name  
                f.mark_as_non_overridable( \
                    "\"" + f.name + "\" cannot be overridden due to the underlying C++ library (W1049)" )
        
# x.readme() returns list of warnings
# x.mark_as_non_overridable( reason ) <- reason is text that will be
# passed as argument
# to an exception, which will be raise when the overriden function will
# be invoked.
