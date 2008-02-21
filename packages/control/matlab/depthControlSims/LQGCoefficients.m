% This file creates K and L for the LQG controller

%physical constants
m=20;
c=11.5;
N = .1; %sensor noise intensity
W = 1; %random physical disturbance intensity

A = [0 1;0 -c/m];
B = [0; 1/m];
C = [1 0];
G = [0 1/m]';
Q = [10 0;0 0];
R = 1;
K = lqr(A,B,Q,R)
L = lqe(A,G,C,W,N,0)
