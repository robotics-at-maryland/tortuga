
import sys
import StringIO

import ram_version_check

stderr = sys.stderr
sys.stderr = StringIO.StringIO()

try:
    import ext.core

    from ext._network import *
finally:
    sys.stderr = stderr
