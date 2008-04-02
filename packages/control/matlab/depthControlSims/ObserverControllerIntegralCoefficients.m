% This file creates A, B, and C for Observer Controller with Integral
  
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

A_b = 0;
B_b = 1;
C_b = 1;

A_a = [A_b zeros(1,2); B*C_b A];
B_a = [B_b; zeros(2,1)];
C_a = [0 C];

Q_a = [10 0 0;0 10 0;0 0 0];
G_a = [0 1/m 0]';

K_a = place(A_a,B_a,[-4 -4.1 -3.9]);
L_a = (place(A_a',C_a',[-22 -22.1 -21.9]))';




% K_a = place(A_a,B_a,[-.1 -.11 -.09]);
% L_a = (place(A_a',C_a',[-10 -10.1 -9.9]))';




A_c = [A_a - B_a*K_a-L_a*C_a   zeros(3,1);   -B_b*K_a    A_b]
B_c = [L_a; 0]
C_c = [zeros(1,3) C_b]

