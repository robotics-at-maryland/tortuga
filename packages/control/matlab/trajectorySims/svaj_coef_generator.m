function [Cp, Cq, Cr, Cs] = svaj_coef_generator(p,q,r,s)

Cp = q*r*s/((q-p)*(r-p)*(s-p))
Cq = p*r*s/((p-q)*(r-q)*(s-q))
Cr = p*q*s/((p-r)*(q-r)*(s-r))
Cs = p*q*r/((p-s)*(q-s)*(r-s))