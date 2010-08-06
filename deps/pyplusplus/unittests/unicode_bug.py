# -*- coding: UTF-8 -*-

import os
import unittest
import autoconfig
from pygccxml import parser
from pygccxml import declarations
from pyplusplus import code_creators
from pyplusplus import module_creator
from pyplusplus import module_builder
from pyplusplus import utils as pypp_utils
from pyplusplus import function_transformers as ft


mb = module_builder.module_builder_t( 
                [ module_builder.create_text_fc( 'struct x{};' ) ]
                , gccxml_path=autoconfig.gccxml.executable 
                , encoding='UTF-8')

mb.build_code_creator( module_name='unicode_bug' )
mb.code_creator.license = "//абвгдеёжзийклмнопрстуфхцчшщъыьэюя"
mb.write_module( os.path.join( autoconfig.build_dir, 'unicode_bug.cpp' ) )
