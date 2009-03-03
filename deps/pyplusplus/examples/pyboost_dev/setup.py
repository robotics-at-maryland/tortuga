#!/usr/bin/env python
# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import sys, os, os.path
from distutils import sysconfig
from distutils.core import setup

__version__ = "0.2"

setup( 
    name = "pyboost"
    , version = __version__
    , description = ""
    , author = "Roman Yakovenko"
    , author_email = "roman.yakovenko@gmail.com"
    , url = 'http://www.language-binding.net/pygccxml/pygccxml.html'
    , packages = [ 'pyboost'
                   , 'pyboost.boost_random'
                   , 'pyboost.crc'
                   , 'pyboost.date_time'
                   , 'pyboost.rational' ]
    , package_data = {
       'pyboost.boost_random' : [ '_random_.dll', 'boost_python.dll' ]
       , 'pyboost.crc' : [ '_crc_.dll', 'boost_python.dll' ]
       , 'pyboost.date_time' : [ '_date_time_.dll'
                                 , 'boost_python.dll'
                                 , 'boost_date_time-vc71-mt-1_34.dll'
                                 , 'date_time_zonespec.csv' ]
       , 'pyboost.rational' : [ '_rational_.dll', 'boost_python.dll' ]
      }
)
