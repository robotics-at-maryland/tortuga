import time
from pyboost import boost_random

rng = boost_random.mt19937( int( time.time() ) ) #setting initial seed
six = boost_random.uniform_int(1,6)
die = boost_random.variate_generator( rng, six )

print die()