
from __future__ import with_statement

import re
import sys

def analyze():
    try:
        while True:
            with open('/proc/meminfo', 'r') as meminfo:
                total = re.match(r'MemTotal:\s+(\d+.*?)$',
                                 meminfo.readline()).group(1)
                free = re.match(r'MemFree:\s+(\d+.*?)$',
                                meminfo.readline()).group(1)
                # Ignore buffers
                meminfo.readline()
                cached = re.match(r'Cached:\s+(\d+.*?)$',
                                  meminfo.readline()).group(1)
                print '\rTotal: %s   Free: %s   Cached: %s' % (total, free, cached),
                #sys.stdout.flush()
    except KeyboardInterrupt:
        pass
        
