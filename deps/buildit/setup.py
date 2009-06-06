#!/usr/bin/env python

from distutils.core import setup

setup (name = 'buildit',
       url = 'http://agendaless.com/Members/chrism/software/buildit',
       version = '1.0',
       description = 'buildit parsing and conditional execution system',
       author = "Chris McDonough",
       author_email="chrism@agendaless.com",
       packages = ['buildit', ],
       package_dir = {'buildit':'.'},
       )
