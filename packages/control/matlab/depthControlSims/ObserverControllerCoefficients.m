% This file creates K and L for the observer controller
A = [0 1;0 0];
B = [0; 1];
C = [1 0];
K = place(A,B,[-4 -4.1])
L = (place(A',C',[-22 -22.1]))'