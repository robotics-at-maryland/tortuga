#! /usr/bin/python
# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import _rational_

from _rational_ import rational
from _rational_ import lcm
from _rational_ import gcd
from _rational_ import bad_rational

rational.__abs__ = _rational_.abs
rational.__float__ = _rational_.to_double
rational.__long__ = _rational_.to_long
rational.__int__ = _rational_.to_long