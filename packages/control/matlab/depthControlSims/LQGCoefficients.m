% This file creates K and L for the "don't know what to call it" controller

%physical constants
m=25;
c=0.15;
N = 1; %sensor noise intensity
W = .0001; %random physical disturbance intensity

A = [0 1;0 -c/m];
B = [0; 1/m];
C = [1 0];
G = [0 1/m]';
Q = [.10 0;0 0];
R = 5;
K = lqr(A,B,Q,R)



L = lqe(A,G,C,W,N,0)
