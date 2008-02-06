%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% generates the mass moments of inertia for a box
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%parameters of the box (in meters)
a=0.787;%length along box x axis
b=0.229;%length along box y axis
c=0.229;%length along box z axis

%mass of box (in kg)
m=23;

%inertia model for a box (kg*m^2)
Ix=(1/12)*m*(b^2+c^2)
Iy=(1/12)*m*(a^2+c^2)
Iz=(1/12)*m*(a^2+b^2)