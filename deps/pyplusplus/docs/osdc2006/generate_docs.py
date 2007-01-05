# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import shutil
from docutils.core import publish_file

def generate( source_dir, destination_dir ):
    publish_file( source_path=os.path.join( source_dir, 'presentation-talk.rest' )
                  , destination_path=os.path.join( destination_dir, 'presentation-talk.html' )
                  , writer_name='html')

if __name__ == '__main__':
    generate(os.curdir, os.curdir)