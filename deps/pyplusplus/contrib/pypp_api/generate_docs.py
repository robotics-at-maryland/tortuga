# Generate documentation using epydoc
#
import os, os.path

default_format = "epytext"


cmd_line = "epydoc -o html -v --docformat %s pypp_api"%(default_format)

print "Running: ", cmd_line
os.system(cmd_line)
