% This file creates K and L for the "don't know what to call it" controller
A = [0 1;0 0];
B = [0; 1];
C = [1 0];
K = place(A,B,[-4 -4.1])
L = (lqe(A,C,B,zeros(2),0.1))