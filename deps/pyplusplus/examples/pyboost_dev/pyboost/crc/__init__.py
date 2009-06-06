#! /usr/bin/python
# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import _crc_

from _crc_ import crc_16_type
from _crc_ import crc_32_type
from _crc_ import crc_ccitt_type
from _crc_ import crc_xmodem_type


__optimal__ = [ crc_16_type, crc_32_type, crc_ccitt_type, crc_xmodem_type ]

from _crc_ import crc_basic_1
from _crc_ import crc_basic_16
from _crc_ import crc_basic_3
from _crc_ import crc_basic_32
from _crc_ import crc_basic_7

basic = { 
      1 : crc_basic_1
    , 3 : crc_basic_3
    , 7 : crc_basic_7
    , 16 : crc_basic_16
    , 32 : crc_basic_32
}

__basic__ = basic.values()

__all__ = __optimal__ + __basic__

for cls in __all__:
    cls.__call__ = cls.process_byte

def process_bytes( self, data ):
    if isinstance( data, str ):
        for byte in data:
            self.process_byte( ord( byte ) )
    else:
        for byte in data:
            self.process_byte( byte )

for cls in __all__:
    cls.process_bytes = process_bytes
    cls.process_block = process_bytes