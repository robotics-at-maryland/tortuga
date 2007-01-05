import os
import sys
from pyboost import crc

if __name__ == '__main__':
    if sys.argv:
        files = sys.argv
    else:
        files = [ sys.executable ]
        
    try:
        result = crc.crc_32_type()
        for file_name in files:
            ifs = file( file_name, 'rb' )
            for line in ifs:
                result.process_bytes( line )
        print hex( result.checksum() ).upper()
    except Exception, error:
        sys.stderr.write( "Found an exception with'%s'%s" %( str( error ), os.linesep ) )
        sys.exit( 1 )