#! /usr/bin/python
# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys

sys.path.append( '..' )

def run_it( dir_name ):
    pwd = os.getcwd()
    os.chdir( dir_name )
    command_line = sys.executable + ' ' + 'test_all.py'
    input_, output = os.popen4( command_line )
    input_.close()
    while True:
        data = output.readline()
        print data,
        if not data:
            break
    exit_status = output.close()
    if exit_status:
        print 'Error! %s did not passed the tests.' % dir_name
    os.chdir( pwd )
    
run_it( 'crc' )
run_it( 'rational' )
run_it( 'date_time' )
run_it( 'boost_random' )