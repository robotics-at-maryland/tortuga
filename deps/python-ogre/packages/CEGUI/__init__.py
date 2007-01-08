import sys, os 

if sys.platform == 'win32': 
    os.environ['PATH'] += ';' + __path__[0] 
    print "Path is:", os.environ['PATH']

from _CEGUI_ import *
