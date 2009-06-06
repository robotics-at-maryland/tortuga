#! /usr/bin/python
# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import sys
import _random_

#TODO: 
#  normal_distribution, cauchy_distribution
# are missed default argument

#generators:
from _random_ import ecuyer1988 
from _random_ import hellekalek1995
from _random_ import kreutzer1986
from _random_ import lagged_fibonacci1279
from _random_ import lagged_fibonacci19937
from _random_ import lagged_fibonacci2281
from _random_ import lagged_fibonacci23209
from _random_ import lagged_fibonacci3217
from _random_ import lagged_fibonacci4423
if sys.platform != 'win32':
    from _random_ import lagged_fibonacci44497
from _random_ import lagged_fibonacci607
from _random_ import lagged_fibonacci9689
from _random_ import minstd_rand
from _random_ import minstd_rand0
from _random_ import mt11213b
from _random_ import mt19937
from _random_ import ranlux3
from _random_ import ranlux3_01
from _random_ import ranlux4
from _random_ import ranlux4_01
from _random_ import ranlux64_3_01
from _random_ import ranlux64_4_01
from _random_ import taus88

generators = [ 
      ecuyer1988 
    , hellekalek1995
    , kreutzer1986
    , lagged_fibonacci1279
    , lagged_fibonacci19937
    , lagged_fibonacci2281
    , lagged_fibonacci23209
    , lagged_fibonacci3217
    , lagged_fibonacci4423    
    , lagged_fibonacci607
    , lagged_fibonacci9689
    , minstd_rand
    , minstd_rand0
    , mt11213b
    , mt19937
    , ranlux3
    , ranlux3_01
    , ranlux4
    , ranlux4_01
    , ranlux64_3_01
    , ranlux64_4_01
    , taus88
]

if sys.platform != 'win32':
    generators.append( lagged_fibonacci44497 )
#distribution
from _random_ import bernoulli_distribution
bernoulli_distribution.__call__ = _random_.py_bernoulli_distribution.call

from _random_ import binomial_distribution
binomial_distribution.__call__ = _random_.py_binomial_distribution.call

from _random_ import cauchy_distribution
cauchy_distribution.__call__ = _random_.py_cauchy_distribution.call

from _random_ import exponential_distribution
exponential_distribution.__call__ = _random_.py_exponential_distribution.call

from _random_ import gamma_distribution
gamma_distribution.__call__ = _random_.py_gamma_distribution.call

from _random_ import geometric_distribution
geometric_distribution.__call__ = _random_.py_geometric_distribution.call

from _random_ import lognormal_distribution
lognormal_distribution.__call__ = _random_.py_lognormal_distribution.call

from _random_ import normal_distribution
normal_distribution.__call__ = _random_.py_normal_distribution.call

from _random_ import poisson_distribution
poisson_distribution.__call__ = _random_.py_poisson_distribution.call

from _random_ import triangle_distribution
triangle_distribution.__call__ = _random_.py_triangle_distribution.call

from _random_ import uniform_int
uniform_int.__call__ = _random_.py_uniform_int.call

from _random_ import uniform_on_sphere
uniform_on_sphere.__call__ = _random_.py_uniform_on_sphere.call

from _random_ import uniform_real
uniform_real.__call__ = _random_.py_uniform_real.call

from _random_ import uniform_smallint
uniform_smallint.__call__ = _random_.py_uniform_smallint.call

distributions =  [ 
      bernoulli_distribution  
    , binomial_distribution    
    , cauchy_distribution      
    , exponential_distribution 
    , gamma_distribution       
    , geometric_distribution   
    , lognormal_distribution   
    , normal_distribution      
    , poisson_distribution     
    , triangle_distribution    
    , uniform_int              
    , uniform_on_sphere        
    , uniform_real             
    , uniform_smallint         
] 



__vg = {}
for g in generators:
    __vg[g] = {}

__vg[ ecuyer1988 ][ bernoulli_distribution   ] = _random_.variate_generator_ecuyer1988__bernoulli_distribution
__vg[ ecuyer1988 ][ binomial_distribution    ] = _random_.variate_generator_ecuyer1988__binomial_distribution
__vg[ ecuyer1988 ][ cauchy_distribution      ] = _random_.variate_generator_ecuyer1988__cauchy_distribution
__vg[ ecuyer1988 ][ exponential_distribution ] = _random_.variate_generator_ecuyer1988__exponential_distribution
__vg[ ecuyer1988 ][ gamma_distribution       ] = _random_.variate_generator_ecuyer1988__gamma_distribution
__vg[ ecuyer1988 ][ geometric_distribution   ] = _random_.variate_generator_ecuyer1988__geometric_distribution
__vg[ ecuyer1988 ][ lognormal_distribution   ] = _random_.variate_generator_ecuyer1988__lognormal_distribution
__vg[ ecuyer1988 ][ normal_distribution      ] = _random_.variate_generator_ecuyer1988__normal_distribution
__vg[ ecuyer1988 ][ poisson_distribution     ] = _random_.variate_generator_ecuyer1988__poisson_distribution
__vg[ ecuyer1988 ][ triangle_distribution    ] = _random_.variate_generator_ecuyer1988__triangle_distribution
__vg[ ecuyer1988 ][ uniform_int              ] = _random_.variate_generator_ecuyer1988__uniform_int
__vg[ ecuyer1988 ][ uniform_on_sphere        ] = _random_.variate_generator_ecuyer1988__uniform_on_sphere
__vg[ ecuyer1988 ][ uniform_real             ] = _random_.variate_generator_ecuyer1988__uniform_real
__vg[ ecuyer1988 ][ uniform_smallint         ] = _random_.variate_generator_ecuyer1988__uniform_smallint

__vg[ hellekalek1995 ][ bernoulli_distribution   ] = _random_.variate_generator_hellekalek1995__bernoulli_distribution  
__vg[ hellekalek1995 ][ binomial_distribution    ] = _random_.variate_generator_hellekalek1995__binomial_distribution   
__vg[ hellekalek1995 ][ cauchy_distribution      ] = _random_.variate_generator_hellekalek1995__cauchy_distribution     
__vg[ hellekalek1995 ][ exponential_distribution ] = _random_.variate_generator_hellekalek1995__exponential_distribution
__vg[ hellekalek1995 ][ gamma_distribution       ] = _random_.variate_generator_hellekalek1995__gamma_distribution      
__vg[ hellekalek1995 ][ geometric_distribution   ] = _random_.variate_generator_hellekalek1995__geometric_distribution  
__vg[ hellekalek1995 ][ lognormal_distribution   ] = _random_.variate_generator_hellekalek1995__lognormal_distribution  
__vg[ hellekalek1995 ][ normal_distribution      ] = _random_.variate_generator_hellekalek1995__normal_distribution     
__vg[ hellekalek1995 ][ poisson_distribution     ] = _random_.variate_generator_hellekalek1995__poisson_distribution    
__vg[ hellekalek1995 ][ triangle_distribution    ] = _random_.variate_generator_hellekalek1995__triangle_distribution   
__vg[ hellekalek1995 ][ uniform_int              ] = _random_.variate_generator_hellekalek1995__uniform_int             
__vg[ hellekalek1995 ][ uniform_on_sphere        ] = _random_.variate_generator_hellekalek1995__uniform_on_sphere       
__vg[ hellekalek1995 ][ uniform_real             ] = _random_.variate_generator_hellekalek1995__uniform_real            
__vg[ hellekalek1995 ][ uniform_smallint         ] = _random_.variate_generator_hellekalek1995__uniform_smallint        

__vg[ kreutzer1986 ][ bernoulli_distribution   ] = _random_.variate_generator_kreutzer1986__bernoulli_distribution  
__vg[ kreutzer1986 ][ binomial_distribution    ] = _random_.variate_generator_kreutzer1986__binomial_distribution   
__vg[ kreutzer1986 ][ cauchy_distribution      ] = _random_.variate_generator_kreutzer1986__cauchy_distribution     
__vg[ kreutzer1986 ][ exponential_distribution ] = _random_.variate_generator_kreutzer1986__exponential_distribution
__vg[ kreutzer1986 ][ gamma_distribution       ] = _random_.variate_generator_kreutzer1986__gamma_distribution      
__vg[ kreutzer1986 ][ geometric_distribution   ] = _random_.variate_generator_kreutzer1986__geometric_distribution  
__vg[ kreutzer1986 ][ lognormal_distribution   ] = _random_.variate_generator_kreutzer1986__lognormal_distribution  
__vg[ kreutzer1986 ][ normal_distribution      ] = _random_.variate_generator_kreutzer1986__normal_distribution     
__vg[ kreutzer1986 ][ poisson_distribution     ] = _random_.variate_generator_kreutzer1986__poisson_distribution    
__vg[ kreutzer1986 ][ triangle_distribution    ] = _random_.variate_generator_kreutzer1986__triangle_distribution   
__vg[ kreutzer1986 ][ uniform_int              ] = _random_.variate_generator_kreutzer1986__uniform_int             
__vg[ kreutzer1986 ][ uniform_on_sphere        ] = _random_.variate_generator_kreutzer1986__uniform_on_sphere       
__vg[ kreutzer1986 ][ uniform_real             ] = _random_.variate_generator_kreutzer1986__uniform_real            
__vg[ kreutzer1986 ][ uniform_smallint         ] = _random_.variate_generator_kreutzer1986__uniform_smallint        

__vg[ lagged_fibonacci1279 ][ bernoulli_distribution   ] = _random_.variate_generator_lagged_fibonacci1279__bernoulli_distribution  
__vg[ lagged_fibonacci1279 ][ binomial_distribution    ] = _random_.variate_generator_lagged_fibonacci1279__binomial_distribution   
__vg[ lagged_fibonacci1279 ][ cauchy_distribution      ] = _random_.variate_generator_lagged_fibonacci1279__cauchy_distribution     
__vg[ lagged_fibonacci1279 ][ exponential_distribution ] = _random_.variate_generator_lagged_fibonacci1279__exponential_distribution
__vg[ lagged_fibonacci1279 ][ gamma_distribution       ] = _random_.variate_generator_lagged_fibonacci1279__gamma_distribution      
__vg[ lagged_fibonacci1279 ][ geometric_distribution   ] = _random_.variate_generator_lagged_fibonacci1279__geometric_distribution  
__vg[ lagged_fibonacci1279 ][ lognormal_distribution   ] = _random_.variate_generator_lagged_fibonacci1279__lognormal_distribution  
__vg[ lagged_fibonacci1279 ][ normal_distribution      ] = _random_.variate_generator_lagged_fibonacci1279__normal_distribution     
__vg[ lagged_fibonacci1279 ][ poisson_distribution     ] = _random_.variate_generator_lagged_fibonacci1279__poisson_distribution    
__vg[ lagged_fibonacci1279 ][ triangle_distribution    ] = _random_.variate_generator_lagged_fibonacci1279__triangle_distribution   
__vg[ lagged_fibonacci1279 ][ uniform_on_sphere        ] = _random_.variate_generator_lagged_fibonacci1279__uniform_on_sphere       
__vg[ lagged_fibonacci1279 ][ uniform_real             ] = _random_.variate_generator_lagged_fibonacci1279__uniform_real            
#__vg[  ][ uniform_int             ] = _random_.does not exist
#__vg[  ][ uniform_smallint         ] = _random_.does not exist
                            
__vg[ lagged_fibonacci19937 ][ bernoulli_distribution   ] = _random_.variate_generator_lagged_fibonacci19937__bernoulli_distribution  
__vg[ lagged_fibonacci19937 ][ binomial_distribution    ] = _random_.variate_generator_lagged_fibonacci19937__binomial_distribution   
__vg[ lagged_fibonacci19937 ][ cauchy_distribution      ] = _random_.variate_generator_lagged_fibonacci19937__cauchy_distribution     
__vg[ lagged_fibonacci19937 ][ exponential_distribution ] = _random_.variate_generator_lagged_fibonacci19937__exponential_distribution
__vg[ lagged_fibonacci19937 ][ gamma_distribution       ] = _random_.variate_generator_lagged_fibonacci19937__gamma_distribution      
__vg[ lagged_fibonacci19937 ][ geometric_distribution   ] = _random_.variate_generator_lagged_fibonacci19937__geometric_distribution  
__vg[ lagged_fibonacci19937 ][ lognormal_distribution   ] = _random_.variate_generator_lagged_fibonacci19937__lognormal_distribution  
__vg[ lagged_fibonacci19937 ][ normal_distribution      ] = _random_.variate_generator_lagged_fibonacci19937__normal_distribution     
__vg[ lagged_fibonacci19937 ][ poisson_distribution     ] = _random_.variate_generator_lagged_fibonacci19937__poisson_distribution    
__vg[ lagged_fibonacci19937 ][ triangle_distribution    ] = _random_.variate_generator_lagged_fibonacci19937__triangle_distribution   
__vg[ lagged_fibonacci19937 ][ uniform_on_sphere        ] = _random_.variate_generator_lagged_fibonacci19937__uniform_on_sphere       
__vg[ lagged_fibonacci19937 ][ uniform_real             ] = _random_.variate_generator_lagged_fibonacci19937__uniform_real            
#__vg[  ][ uniform_int             ] = _random_.does not exist
#__vg[  ][ uniform_smallint         ] = _random_.does not exist

__vg[ lagged_fibonacci2281 ][ bernoulli_distribution   ] = _random_.variate_generator_lagged_fibonacci2281__bernoulli_distribution  
__vg[ lagged_fibonacci2281 ][ binomial_distribution    ] = _random_.variate_generator_lagged_fibonacci2281__binomial_distribution   
__vg[ lagged_fibonacci2281 ][ cauchy_distribution      ] = _random_.variate_generator_lagged_fibonacci2281__cauchy_distribution     
__vg[ lagged_fibonacci2281 ][ exponential_distribution ] = _random_.variate_generator_lagged_fibonacci2281__exponential_distribution
__vg[ lagged_fibonacci2281 ][ gamma_distribution       ] = _random_.variate_generator_lagged_fibonacci2281__gamma_distribution      
__vg[ lagged_fibonacci2281 ][ geometric_distribution   ] = _random_.variate_generator_lagged_fibonacci2281__geometric_distribution  
__vg[ lagged_fibonacci2281 ][ lognormal_distribution   ] = _random_.variate_generator_lagged_fibonacci2281__lognormal_distribution  
__vg[ lagged_fibonacci2281 ][ normal_distribution      ] = _random_.variate_generator_lagged_fibonacci2281__normal_distribution     
__vg[ lagged_fibonacci2281 ][ poisson_distribution     ] = _random_.variate_generator_lagged_fibonacci2281__poisson_distribution    
__vg[ lagged_fibonacci2281 ][ triangle_distribution    ] = _random_.variate_generator_lagged_fibonacci2281__triangle_distribution   
__vg[ lagged_fibonacci2281 ][ uniform_on_sphere        ] = _random_.variate_generator_lagged_fibonacci2281__uniform_on_sphere       
__vg[ lagged_fibonacci2281 ][ uniform_real             ] = _random_.variate_generator_lagged_fibonacci2281__uniform_real            
#__vg[  ][ uniform_int             ] = _random_.does not exist
#__vg[  ][ uniform_smallint         ] = _random_.does not exist

__vg[ lagged_fibonacci23209 ][ bernoulli_distribution   ] = _random_.variate_generator_lagged_fibonacci23209__bernoulli_distribution  
__vg[ lagged_fibonacci23209 ][ binomial_distribution    ] = _random_.variate_generator_lagged_fibonacci23209__binomial_distribution   
__vg[ lagged_fibonacci23209 ][ cauchy_distribution      ] = _random_.variate_generator_lagged_fibonacci23209__cauchy_distribution     
__vg[ lagged_fibonacci23209 ][ exponential_distribution ] = _random_.variate_generator_lagged_fibonacci23209__exponential_distribution
__vg[ lagged_fibonacci23209 ][ gamma_distribution       ] = _random_.variate_generator_lagged_fibonacci23209__gamma_distribution      
__vg[ lagged_fibonacci23209 ][ geometric_distribution   ] = _random_.variate_generator_lagged_fibonacci23209__geometric_distribution  
__vg[ lagged_fibonacci23209 ][ lognormal_distribution   ] = _random_.variate_generator_lagged_fibonacci23209__lognormal_distribution  
__vg[ lagged_fibonacci23209 ][ normal_distribution      ] = _random_.variate_generator_lagged_fibonacci23209__normal_distribution     
__vg[ lagged_fibonacci23209 ][ poisson_distribution     ] = _random_.variate_generator_lagged_fibonacci23209__poisson_distribution    
__vg[ lagged_fibonacci23209 ][ triangle_distribution    ] = _random_.variate_generator_lagged_fibonacci23209__triangle_distribution   
__vg[ lagged_fibonacci23209 ][ uniform_on_sphere        ] = _random_.variate_generator_lagged_fibonacci23209__uniform_on_sphere       
__vg[ lagged_fibonacci23209 ][ uniform_real             ] = _random_.variate_generator_lagged_fibonacci23209__uniform_real            
#__vg[  ][ uniform_int             ] = _random_.does not exist
#__vg[  ][ uniform_smallint         ] = _random_.does not exist


__vg[ lagged_fibonacci3217 ][ bernoulli_distribution   ] = _random_.variate_generator_lagged_fibonacci3217__bernoulli_distribution  
__vg[ lagged_fibonacci3217 ][ binomial_distribution    ] = _random_.variate_generator_lagged_fibonacci3217__binomial_distribution   
__vg[ lagged_fibonacci3217 ][ cauchy_distribution      ] = _random_.variate_generator_lagged_fibonacci3217__cauchy_distribution     
__vg[ lagged_fibonacci3217 ][ exponential_distribution ] = _random_.variate_generator_lagged_fibonacci3217__exponential_distribution
__vg[ lagged_fibonacci3217 ][ gamma_distribution       ] = _random_.variate_generator_lagged_fibonacci3217__gamma_distribution      
__vg[ lagged_fibonacci3217 ][ geometric_distribution   ] = _random_.variate_generator_lagged_fibonacci3217__geometric_distribution  
__vg[ lagged_fibonacci3217 ][ lognormal_distribution   ] = _random_.variate_generator_lagged_fibonacci3217__lognormal_distribution  
__vg[ lagged_fibonacci3217 ][ normal_distribution      ] = _random_.variate_generator_lagged_fibonacci3217__normal_distribution     
__vg[ lagged_fibonacci3217 ][ poisson_distribution     ] = _random_.variate_generator_lagged_fibonacci3217__poisson_distribution    
__vg[ lagged_fibonacci3217 ][ triangle_distribution    ] = _random_.variate_generator_lagged_fibonacci3217__triangle_distribution   
__vg[ lagged_fibonacci3217 ][ uniform_int              ] = _random_.variate_generator_lagged_fibonacci3217__uniform_on_sphere       
__vg[ lagged_fibonacci3217 ][ uniform_on_sphere        ] = _random_.variate_generator_lagged_fibonacci3217__uniform_real            
#__vg[  ][ uniform_real             ] = _random_.does not exist
#__vg[  ][ uniform_smallint         ] = _random_.does not exist


__vg[ lagged_fibonacci4423 ][ bernoulli_distribution   ] = _random_.variate_generator_lagged_fibonacci4423__bernoulli_distribution  
__vg[ lagged_fibonacci4423 ][ binomial_distribution    ] = _random_.variate_generator_lagged_fibonacci4423__binomial_distribution   
__vg[ lagged_fibonacci4423 ][ cauchy_distribution      ] = _random_.variate_generator_lagged_fibonacci4423__cauchy_distribution     
__vg[ lagged_fibonacci4423 ][ exponential_distribution ] = _random_.variate_generator_lagged_fibonacci4423__exponential_distribution
__vg[ lagged_fibonacci4423 ][ gamma_distribution       ] = _random_.variate_generator_lagged_fibonacci4423__gamma_distribution      
__vg[ lagged_fibonacci4423 ][ geometric_distribution   ] = _random_.variate_generator_lagged_fibonacci4423__geometric_distribution  
__vg[ lagged_fibonacci4423 ][ lognormal_distribution   ] = _random_.variate_generator_lagged_fibonacci4423__lognormal_distribution  
__vg[ lagged_fibonacci4423 ][ normal_distribution      ] = _random_.variate_generator_lagged_fibonacci4423__normal_distribution     
__vg[ lagged_fibonacci4423 ][ poisson_distribution     ] = _random_.variate_generator_lagged_fibonacci4423__poisson_distribution    
__vg[ lagged_fibonacci4423 ][ triangle_distribution    ] = _random_.variate_generator_lagged_fibonacci4423__triangle_distribution   
__vg[ lagged_fibonacci4423 ][ uniform_on_sphere        ] = _random_.variate_generator_lagged_fibonacci4423__uniform_on_sphere       
__vg[ lagged_fibonacci4423 ][ uniform_real             ] = _random_.variate_generator_lagged_fibonacci4423__uniform_real            
#__vg[  ][ uniform_int             ] = _random_.does not exist
#__vg[  ][ uniform_smallint         ] = _random_.does not exist

if sys.platform != 'win32':
    __vg[ lagged_fibonacci44497 ][ bernoulli_distribution   ] = _random_.variate_generator_lagged_fibonacci44497__bernoulli_distribution  
    __vg[ lagged_fibonacci44497 ][ binomial_distribution    ] = _random_.variate_generator_lagged_fibonacci44497__binomial_distribution   
    __vg[ lagged_fibonacci44497 ][ cauchy_distribution      ] = _random_.variate_generator_lagged_fibonacci44497__cauchy_distribution     
    __vg[ lagged_fibonacci44497 ][ exponential_distribution ] = _random_.variate_generator_lagged_fibonacci44497__exponential_distribution
    __vg[ lagged_fibonacci44497 ][ gamma_distribution       ] = _random_.variate_generator_lagged_fibonacci44497__gamma_distribution      
    __vg[ lagged_fibonacci44497 ][ geometric_distribution   ] = _random_.variate_generator_lagged_fibonacci44497__geometric_distribution  
    __vg[ lagged_fibonacci44497 ][ lognormal_distribution   ] = _random_.variate_generator_lagged_fibonacci44497__lognormal_distribution  
    __vg[ lagged_fibonacci44497 ][ normal_distribution      ] = _random_.variate_generator_lagged_fibonacci44497__normal_distribution     
    __vg[ lagged_fibonacci44497 ][ poisson_distribution     ] = _random_.variate_generator_lagged_fibonacci44497__poisson_distribution    
    __vg[ lagged_fibonacci44497 ][ triangle_distribution    ] = _random_.variate_generator_lagged_fibonacci44497__triangle_distribution   
    __vg[ lagged_fibonacci44497 ][ uniform_on_sphere        ] = _random_.variate_generator_lagged_fibonacci44497__uniform_on_sphere       
    __vg[ lagged_fibonacci44497 ][ uniform_real             ] = _random_.variate_generator_lagged_fibonacci44497__uniform_real            
    #__vg[  ][ uniform_int             ] = _random_.does not exist
    #__vg[  ][ uniform_smallint         ] = _random_.does not exist

__vg[ lagged_fibonacci607 ][ bernoulli_distribution   ] = _random_.variate_generator_lagged_fibonacci607__bernoulli_distribution  
__vg[ lagged_fibonacci607 ][ binomial_distribution    ] = _random_.variate_generator_lagged_fibonacci607__binomial_distribution   
__vg[ lagged_fibonacci607 ][ cauchy_distribution      ] = _random_.variate_generator_lagged_fibonacci607__cauchy_distribution     
__vg[ lagged_fibonacci607 ][ exponential_distribution ] = _random_.variate_generator_lagged_fibonacci607__exponential_distribution
__vg[ lagged_fibonacci607 ][ gamma_distribution       ] = _random_.variate_generator_lagged_fibonacci607__gamma_distribution      
__vg[ lagged_fibonacci607 ][ geometric_distribution   ] = _random_.variate_generator_lagged_fibonacci607__geometric_distribution  
__vg[ lagged_fibonacci607 ][ lognormal_distribution   ] = _random_.variate_generator_lagged_fibonacci607__lognormal_distribution  
__vg[ lagged_fibonacci607 ][ normal_distribution      ] = _random_.variate_generator_lagged_fibonacci607__normal_distribution     
__vg[ lagged_fibonacci607 ][ poisson_distribution     ] = _random_.variate_generator_lagged_fibonacci607__poisson_distribution    
__vg[ lagged_fibonacci607 ][ triangle_distribution    ] = _random_.variate_generator_lagged_fibonacci607__triangle_distribution   
__vg[ lagged_fibonacci607 ][ uniform_on_sphere        ] = _random_.variate_generator_lagged_fibonacci607__uniform_on_sphere       
__vg[ lagged_fibonacci607 ][ uniform_real             ] = _random_.variate_generator_lagged_fibonacci607__uniform_real            
#__vg[  ][ uniform_int             ] = _random_.does not exist
#__vg[  ][ uniform_smallint         ] = _random_.does not exist


__vg[ lagged_fibonacci9689 ][ bernoulli_distribution   ] = _random_.variate_generator_lagged_fibonacci9689__bernoulli_distribution  
__vg[ lagged_fibonacci9689 ][ binomial_distribution    ] = _random_.variate_generator_lagged_fibonacci9689__binomial_distribution   
__vg[ lagged_fibonacci9689 ][ cauchy_distribution      ] = _random_.variate_generator_lagged_fibonacci9689__cauchy_distribution     
__vg[ lagged_fibonacci9689 ][ exponential_distribution ] = _random_.variate_generator_lagged_fibonacci9689__exponential_distribution
__vg[ lagged_fibonacci9689 ][ gamma_distribution       ] = _random_.variate_generator_lagged_fibonacci9689__gamma_distribution      
__vg[ lagged_fibonacci9689 ][ geometric_distribution   ] = _random_.variate_generator_lagged_fibonacci9689__geometric_distribution  
__vg[ lagged_fibonacci9689 ][ lognormal_distribution   ] = _random_.variate_generator_lagged_fibonacci9689__lognormal_distribution  
__vg[ lagged_fibonacci9689 ][ normal_distribution      ] = _random_.variate_generator_lagged_fibonacci9689__normal_distribution     
__vg[ lagged_fibonacci9689 ][ poisson_distribution     ] = _random_.variate_generator_lagged_fibonacci9689__poisson_distribution    
__vg[ lagged_fibonacci9689 ][ triangle_distribution    ] = _random_.variate_generator_lagged_fibonacci9689__triangle_distribution   
__vg[ lagged_fibonacci9689 ][ uniform_on_sphere        ] = _random_.variate_generator_lagged_fibonacci9689__uniform_on_sphere       
__vg[ lagged_fibonacci9689 ][ uniform_real             ] = _random_.variate_generator_lagged_fibonacci9689__uniform_real            
#__vg[  ][ uniform_int             ] = _random_.does not exist
#__vg[  ][ uniform_smallint         ] = _random_.does not exist

__vg[ minstd_rand0 ][ bernoulli_distribution   ] = _random_.variate_generator_minstd_rand0__bernoulli_distribution  
__vg[ minstd_rand0 ][ binomial_distribution    ] = _random_.variate_generator_minstd_rand0__binomial_distribution   
__vg[ minstd_rand0 ][ cauchy_distribution      ] = _random_.variate_generator_minstd_rand0__cauchy_distribution     
__vg[ minstd_rand0 ][ exponential_distribution ] = _random_.variate_generator_minstd_rand0__exponential_distribution
__vg[ minstd_rand0 ][ gamma_distribution       ] = _random_.variate_generator_minstd_rand0__gamma_distribution      
__vg[ minstd_rand0 ][ geometric_distribution   ] = _random_.variate_generator_minstd_rand0__geometric_distribution  
__vg[ minstd_rand0 ][ lognormal_distribution   ] = _random_.variate_generator_minstd_rand0__lognormal_distribution  
__vg[ minstd_rand0 ][ normal_distribution      ] = _random_.variate_generator_minstd_rand0__normal_distribution     
__vg[ minstd_rand0 ][ poisson_distribution     ] = _random_.variate_generator_minstd_rand0__poisson_distribution    
__vg[ minstd_rand0 ][ triangle_distribution    ] = _random_.variate_generator_minstd_rand0__triangle_distribution   
__vg[ minstd_rand0 ][ uniform_int              ] = _random_.variate_generator_minstd_rand0__uniform_int             
__vg[ minstd_rand0 ][ uniform_on_sphere        ] = _random_.variate_generator_minstd_rand0__uniform_on_sphere       
__vg[ minstd_rand0 ][ uniform_real             ] = _random_.variate_generator_minstd_rand0__uniform_real            
__vg[ minstd_rand0 ][ uniform_smallint         ] = _random_.variate_generator_minstd_rand0__uniform_smallint        

__vg[ minstd_rand ][ bernoulli_distribution   ] = _random_.variate_generator_minstd_rand__bernoulli_distribution  
__vg[ minstd_rand ][ binomial_distribution    ] = _random_.variate_generator_minstd_rand__binomial_distribution   
__vg[ minstd_rand ][ cauchy_distribution      ] = _random_.variate_generator_minstd_rand__cauchy_distribution     
__vg[ minstd_rand ][ exponential_distribution ] = _random_.variate_generator_minstd_rand__exponential_distribution
__vg[ minstd_rand ][ gamma_distribution       ] = _random_.variate_generator_minstd_rand__gamma_distribution      
__vg[ minstd_rand ][ geometric_distribution   ] = _random_.variate_generator_minstd_rand__geometric_distribution  
__vg[ minstd_rand ][ lognormal_distribution   ] = _random_.variate_generator_minstd_rand__lognormal_distribution  
__vg[ minstd_rand ][ normal_distribution      ] = _random_.variate_generator_minstd_rand__normal_distribution     
__vg[ minstd_rand ][ poisson_distribution     ] = _random_.variate_generator_minstd_rand__poisson_distribution    
__vg[ minstd_rand ][ triangle_distribution    ] = _random_.variate_generator_minstd_rand__triangle_distribution   
__vg[ minstd_rand ][ uniform_int              ] = _random_.variate_generator_minstd_rand__uniform_int             
__vg[ minstd_rand ][ uniform_on_sphere        ] = _random_.variate_generator_minstd_rand__uniform_on_sphere       
__vg[ minstd_rand ][ uniform_real             ] = _random_.variate_generator_minstd_rand__uniform_real            
__vg[ minstd_rand ][ uniform_smallint         ] = _random_.variate_generator_minstd_rand__uniform_smallint        

__vg[ mt11213b ][ bernoulli_distribution   ] = _random_.variate_generator_mt11213b__bernoulli_distribution  
__vg[ mt11213b ][ binomial_distribution    ] = _random_.variate_generator_mt11213b__binomial_distribution   
__vg[ mt11213b ][ cauchy_distribution      ] = _random_.variate_generator_mt11213b__cauchy_distribution     
__vg[ mt11213b ][ exponential_distribution ] = _random_.variate_generator_mt11213b__exponential_distribution
__vg[ mt11213b ][ gamma_distribution       ] = _random_.variate_generator_mt11213b__gamma_distribution      
__vg[ mt11213b ][ geometric_distribution   ] = _random_.variate_generator_mt11213b__geometric_distribution  
__vg[ mt11213b ][ lognormal_distribution   ] = _random_.variate_generator_mt11213b__lognormal_distribution  
__vg[ mt11213b ][ normal_distribution      ] = _random_.variate_generator_mt11213b__normal_distribution     
__vg[ mt11213b ][ poisson_distribution     ] = _random_.variate_generator_mt11213b__poisson_distribution    
__vg[ mt11213b ][ triangle_distribution    ] = _random_.variate_generator_mt11213b__triangle_distribution   
__vg[ mt11213b ][ uniform_int              ] = _random_.variate_generator_mt11213b__uniform_int             
__vg[ mt11213b ][ uniform_on_sphere        ] = _random_.variate_generator_mt11213b__uniform_on_sphere       
__vg[ mt11213b ][ uniform_real             ] = _random_.variate_generator_mt11213b__uniform_real            
__vg[ mt11213b ][ uniform_smallint         ] = _random_.variate_generator_mt11213b__uniform_smallint        

__vg[ mt19937 ][ bernoulli_distribution   ] = _random_.variate_generator_mt19937__bernoulli_distribution  
__vg[ mt19937 ][ binomial_distribution    ] = _random_.variate_generator_mt19937__binomial_distribution   
__vg[ mt19937 ][ cauchy_distribution      ] = _random_.variate_generator_mt19937__cauchy_distribution     
__vg[ mt19937 ][ exponential_distribution ] = _random_.variate_generator_mt19937__exponential_distribution
__vg[ mt19937 ][ gamma_distribution       ] = _random_.variate_generator_mt19937__gamma_distribution      
__vg[ mt19937 ][ geometric_distribution   ] = _random_.variate_generator_mt19937__geometric_distribution  
__vg[ mt19937 ][ lognormal_distribution   ] = _random_.variate_generator_mt19937__lognormal_distribution  
__vg[ mt19937 ][ normal_distribution      ] = _random_.variate_generator_mt19937__normal_distribution     
__vg[ mt19937 ][ poisson_distribution     ] = _random_.variate_generator_mt19937__poisson_distribution    
__vg[ mt19937 ][ triangle_distribution    ] = _random_.variate_generator_mt19937__triangle_distribution   
__vg[ mt19937 ][ uniform_int              ] = _random_.variate_generator_mt19937__uniform_int             
__vg[ mt19937 ][ uniform_on_sphere        ] = _random_.variate_generator_mt19937__uniform_on_sphere       
__vg[ mt19937 ][ uniform_real             ] = _random_.variate_generator_mt19937__uniform_real            
__vg[ mt19937 ][ uniform_smallint         ] = _random_.variate_generator_mt19937__uniform_smallint        

__vg[ ranlux3_01 ][ bernoulli_distribution   ] = _random_.variate_generator_ranlux3_01__bernoulli_distribution  
__vg[ ranlux3_01 ][ binomial_distribution    ] = _random_.variate_generator_ranlux3_01__binomial_distribution   
__vg[ ranlux3_01 ][ cauchy_distribution      ] = _random_.variate_generator_ranlux3_01__cauchy_distribution     
__vg[ ranlux3_01 ][ exponential_distribution ] = _random_.variate_generator_ranlux3_01__exponential_distribution
__vg[ ranlux3_01 ][ gamma_distribution       ] = _random_.variate_generator_ranlux3_01__gamma_distribution      
__vg[ ranlux3_01 ][ geometric_distribution   ] = _random_.variate_generator_ranlux3_01__geometric_distribution  
__vg[ ranlux3_01 ][ lognormal_distribution   ] = _random_.variate_generator_ranlux3_01__lognormal_distribution  
__vg[ ranlux3_01 ][ normal_distribution      ] = _random_.variate_generator_ranlux3_01__normal_distribution     
__vg[ ranlux3_01 ][ poisson_distribution     ] = _random_.variate_generator_ranlux3_01__poisson_distribution    
__vg[ ranlux3_01 ][ triangle_distribution    ] = _random_.variate_generator_ranlux3_01__triangle_distribution   
__vg[ ranlux3_01 ][ uniform_on_sphere        ] = _random_.variate_generator_ranlux3_01__uniform_on_sphere       
__vg[ ranlux3_01 ][ uniform_real             ] = _random_.variate_generator_ranlux3_01__uniform_real            
#__vg[  ][ uniform_int             ] = _random_.does not exist
#__vg[  ][ uniform_smallint         ] = _random_.does not exist

__vg[ ranlux3 ][ bernoulli_distribution   ] = _random_.variate_generator_ranlux3__bernoulli_distribution  
__vg[ ranlux3 ][ binomial_distribution    ] = _random_.variate_generator_ranlux3__binomial_distribution   
__vg[ ranlux3 ][ cauchy_distribution      ] = _random_.variate_generator_ranlux3__cauchy_distribution     
__vg[ ranlux3 ][ exponential_distribution ] = _random_.variate_generator_ranlux3__exponential_distribution
__vg[ ranlux3 ][ gamma_distribution       ] = _random_.variate_generator_ranlux3__gamma_distribution      
__vg[ ranlux3 ][ geometric_distribution   ] = _random_.variate_generator_ranlux3__geometric_distribution  
__vg[ ranlux3 ][ lognormal_distribution   ] = _random_.variate_generator_ranlux3__lognormal_distribution  
__vg[ ranlux3 ][ normal_distribution      ] = _random_.variate_generator_ranlux3__normal_distribution     
__vg[ ranlux3 ][ poisson_distribution     ] = _random_.variate_generator_ranlux3__poisson_distribution    
__vg[ ranlux3 ][ triangle_distribution    ] = _random_.variate_generator_ranlux3__triangle_distribution   
__vg[ ranlux3 ][ uniform_int              ] = _random_.variate_generator_ranlux3__uniform_int             
__vg[ ranlux3 ][ uniform_on_sphere        ] = _random_.variate_generator_ranlux3__uniform_on_sphere       
__vg[ ranlux3 ][ uniform_real             ] = _random_.variate_generator_ranlux3__uniform_real            
__vg[ ranlux3 ][ uniform_smallint         ] = _random_.variate_generator_ranlux3__uniform_smallint        

__vg[ ranlux4_01 ][ bernoulli_distribution   ] = _random_.variate_generator_ranlux4_01__bernoulli_distribution  
__vg[ ranlux4_01 ][ binomial_distribution    ] = _random_.variate_generator_ranlux4_01__binomial_distribution   
__vg[ ranlux4_01 ][ cauchy_distribution      ] = _random_.variate_generator_ranlux4_01__cauchy_distribution     
__vg[ ranlux4_01 ][ exponential_distribution ] = _random_.variate_generator_ranlux4_01__exponential_distribution
__vg[ ranlux4_01 ][ gamma_distribution       ] = _random_.variate_generator_ranlux4_01__gamma_distribution      
__vg[ ranlux4_01 ][ geometric_distribution   ] = _random_.variate_generator_ranlux4_01__geometric_distribution  
__vg[ ranlux4_01 ][ lognormal_distribution   ] = _random_.variate_generator_ranlux4_01__lognormal_distribution  
__vg[ ranlux4_01 ][ normal_distribution      ] = _random_.variate_generator_ranlux4_01__normal_distribution     
__vg[ ranlux4_01 ][ poisson_distribution     ] = _random_.variate_generator_ranlux4_01__poisson_distribution    
__vg[ ranlux4_01 ][ triangle_distribution    ] = _random_.variate_generator_ranlux4_01__triangle_distribution   
__vg[ ranlux4_01 ][ uniform_on_sphere        ] = _random_.variate_generator_ranlux4_01__uniform_on_sphere       
__vg[ ranlux4_01 ][ uniform_real             ] = _random_.variate_generator_ranlux4_01__uniform_real            
#__vg[  ][ uniform_int             ] = _random_.does not exist
#__vg[  ][ uniform_smallint         ] = _random_.does not exist

__vg[ ranlux4 ][ bernoulli_distribution   ] = _random_.variate_generator_ranlux4__bernoulli_distribution  
__vg[ ranlux4 ][ binomial_distribution    ] = _random_.variate_generator_ranlux4__binomial_distribution   
__vg[ ranlux4 ][ cauchy_distribution      ] = _random_.variate_generator_ranlux4__cauchy_distribution     
__vg[ ranlux4 ][ exponential_distribution ] = _random_.variate_generator_ranlux4__exponential_distribution
__vg[ ranlux4 ][ gamma_distribution       ] = _random_.variate_generator_ranlux4__gamma_distribution      
__vg[ ranlux4 ][ geometric_distribution   ] = _random_.variate_generator_ranlux4__geometric_distribution  
__vg[ ranlux4 ][ lognormal_distribution   ] = _random_.variate_generator_ranlux4__lognormal_distribution  
__vg[ ranlux4 ][ normal_distribution      ] = _random_.variate_generator_ranlux4__normal_distribution     
__vg[ ranlux4 ][ poisson_distribution     ] = _random_.variate_generator_ranlux4__poisson_distribution    
__vg[ ranlux4 ][ triangle_distribution    ] = _random_.variate_generator_ranlux4__triangle_distribution   
__vg[ ranlux4 ][ uniform_int              ] = _random_.variate_generator_ranlux4__uniform_int             
__vg[ ranlux4 ][ uniform_on_sphere        ] = _random_.variate_generator_ranlux4__uniform_on_sphere       
__vg[ ranlux4 ][ uniform_real             ] = _random_.variate_generator_ranlux4__uniform_real            
__vg[ ranlux4 ][ uniform_smallint         ] = _random_.variate_generator_ranlux4__uniform_smallint        

__vg[ ranlux64_3_01 ][ bernoulli_distribution   ] = _random_.variate_generator_ranlux64_3_01__bernoulli_distribution  
__vg[ ranlux64_3_01 ][ binomial_distribution    ] = _random_.variate_generator_ranlux64_3_01__binomial_distribution   
__vg[ ranlux64_3_01 ][ cauchy_distribution      ] = _random_.variate_generator_ranlux64_3_01__cauchy_distribution     
__vg[ ranlux64_3_01 ][ exponential_distribution ] = _random_.variate_generator_ranlux64_3_01__exponential_distribution
__vg[ ranlux64_3_01 ][ gamma_distribution       ] = _random_.variate_generator_ranlux64_3_01__gamma_distribution      
__vg[ ranlux64_3_01 ][ geometric_distribution   ] = _random_.variate_generator_ranlux64_3_01__geometric_distribution  
__vg[ ranlux64_3_01 ][ lognormal_distribution   ] = _random_.variate_generator_ranlux64_3_01__lognormal_distribution  
__vg[ ranlux64_3_01 ][ normal_distribution      ] = _random_.variate_generator_ranlux64_3_01__normal_distribution     
__vg[ ranlux64_3_01 ][ poisson_distribution     ] = _random_.variate_generator_ranlux64_3_01__poisson_distribution    
__vg[ ranlux64_3_01 ][ triangle_distribution    ] = _random_.variate_generator_ranlux64_3_01__triangle_distribution   
__vg[ ranlux64_3_01 ][ uniform_on_sphere        ] = _random_.variate_generator_ranlux64_3_01__uniform_on_sphere       
__vg[ ranlux64_3_01 ][ uniform_real             ] = _random_.variate_generator_ranlux64_3_01__uniform_real            
#__vg[  ][ uniform_int             ] = _random_.does not exist
#__vg[  ][ uniform_smallint         ] = _random_.does not exist

__vg[ ranlux64_4_01 ][ bernoulli_distribution   ] = _random_.variate_generator_ranlux64_4_01__bernoulli_distribution  
__vg[ ranlux64_4_01 ][ binomial_distribution    ] = _random_.variate_generator_ranlux64_4_01__binomial_distribution   
__vg[ ranlux64_4_01 ][ cauchy_distribution      ] = _random_.variate_generator_ranlux64_4_01__cauchy_distribution     
__vg[ ranlux64_4_01 ][ exponential_distribution ] = _random_.variate_generator_ranlux64_4_01__exponential_distribution
__vg[ ranlux64_4_01 ][ gamma_distribution       ] = _random_.variate_generator_ranlux64_4_01__gamma_distribution      
__vg[ ranlux64_4_01 ][ geometric_distribution   ] = _random_.variate_generator_ranlux64_4_01__geometric_distribution  
__vg[ ranlux64_4_01 ][ lognormal_distribution   ] = _random_.variate_generator_ranlux64_4_01__lognormal_distribution  
__vg[ ranlux64_4_01 ][ normal_distribution      ] = _random_.variate_generator_ranlux64_4_01__normal_distribution     
__vg[ ranlux64_4_01 ][ poisson_distribution     ] = _random_.variate_generator_ranlux64_4_01__poisson_distribution    
__vg[ ranlux64_4_01 ][ triangle_distribution    ] = _random_.variate_generator_ranlux64_4_01__triangle_distribution   
__vg[ ranlux64_4_01 ][ uniform_on_sphere        ] = _random_.variate_generator_ranlux64_4_01__uniform_on_sphere       
__vg[ ranlux64_4_01 ][ uniform_real             ] = _random_.variate_generator_ranlux64_4_01__uniform_real            
#__vg[  ][ uniform_int             ] = _random_.does not exist
#__vg[  ][ uniform_smallint         ] = _random_.does not exist


__vg[ taus88 ][ bernoulli_distribution   ] = _random_.variate_generator_taus88__bernoulli_distribution  
__vg[ taus88 ][ binomial_distribution    ] = _random_.variate_generator_taus88__binomial_distribution   
__vg[ taus88 ][ cauchy_distribution      ] = _random_.variate_generator_taus88__cauchy_distribution     
__vg[ taus88 ][ exponential_distribution ] = _random_.variate_generator_taus88__exponential_distribution
__vg[ taus88 ][ gamma_distribution       ] = _random_.variate_generator_taus88__gamma_distribution      
__vg[ taus88 ][ geometric_distribution   ] = _random_.variate_generator_taus88__geometric_distribution  
__vg[ taus88 ][ lognormal_distribution   ] = _random_.variate_generator_taus88__lognormal_distribution  
__vg[ taus88 ][ normal_distribution      ] = _random_.variate_generator_taus88__normal_distribution     
__vg[ taus88 ][ poisson_distribution     ] = _random_.variate_generator_taus88__poisson_distribution    
__vg[ taus88 ][ triangle_distribution    ] = _random_.variate_generator_taus88__triangle_distribution   
__vg[ taus88 ][ uniform_int              ] = _random_.variate_generator_taus88__uniform_int             
__vg[ taus88 ][ uniform_on_sphere        ] = _random_.variate_generator_taus88__uniform_on_sphere       
__vg[ taus88 ][ uniform_real             ] = _random_.variate_generator_taus88__uniform_real            
__vg[ taus88 ][ uniform_smallint         ] = _random_.variate_generator_taus88__uniform_smallint        


def variate_generator( generator, distribution ):
    global __vg
    global distributions
    gcls = generator.__class__
    if not __vg.has_key( gcls ):
        raise RuntimeError( 'Unable to create variate_generator - unknown generator type!' )
    dcls = distribution.__class__
    if dcls not in distributions:
        raise RuntimeError( 'Unable to create variate_generator - unknown distribution type!' )
    
    valid_distrs = __vg[ gcls ]
    if not valid_distrs.has_key( dcls ):
        raise RuntimeError( 'Unable to create variate_generator(%s, %s) - takoe sochetanie does not exist!'
                            % ( gcls.__name__, dcls.__name__ ) )
    return valid_distrs[dcls]( generator, distribution )

def variate_generator_exists( generator, distribution ):
    global __vg
    global distributions
    gcls = generator.__class__
    if not __vg.has_key( gcls ):
        return False
    dcls = distribution.__class__
    if dcls not in distributions:
        return False
    
    valid_distrs = __vg[ gcls ]
    return valid_distrs.has_key( dcls )