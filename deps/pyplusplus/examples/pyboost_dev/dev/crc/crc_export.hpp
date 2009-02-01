// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "boost/crc.hpp"

namespace boost{ namespace details{

inline void export_crc(){
    sizeof( crc_16_type );
    sizeof( crc_ccitt_type );
    sizeof( crc_xmodem_type );
    sizeof( crc_32_type );
    sizeof( crc_basic<1> );
    sizeof( crc_basic<3> );
    sizeof( crc_basic<7> );
    sizeof( crc_basic<16> );
    sizeof( crc_basic<32> );
}

} }

