from pyboost import rational

half = rational.rational( 1, 2 )
one = rational.rational( 1 )
two = rational.rational( 2 )

#Some basic checks
assert half.numerator() == 1
assert half.denominator() == 2
assert float(half) == 0.5
#Arithmetic
assert half + half == one == 1
assert one - half == half
assert two * half == one == 1
assert one / half == two == 2