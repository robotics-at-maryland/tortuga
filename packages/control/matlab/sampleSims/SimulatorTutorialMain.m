%                       SimulatorTutorialMain
%
%   This file is a tutorial for creating simulators in MATLAB. 
%
%   Created by Elizabeth Ashley and Simon Knister on 12/01/2009.
%
%       1.Initialization
%       2.Simulation
%       3.Plot results/data file

clear;
clc;
close all;


%% Initialization

%Initial conditions
x0(1) = 0;   %Position
x0(2) = 0;   %Velocity

%Global variables
%Note: All variables in this tutorial are arbitrary
%   Using global variables allows all the changes to variable values to be
%   made separate from the equation.  This helps minimize error due to
%   mistake.
%weights for noise
global N1;
N1 = 1;
global N2;
N2 = 1;
    
%System constants (spring constant, etc)
global k;   %k = spring constant
k = 5;
global m;   %m = mass
m = 3;
global c;   %c = damper constant
c = 1;

%Gain values for control
global gain1; 
gain1 = 1;
global gain2;
gain2 = 1;


%Time frame
ti = 0;
tf = 10;
    
%% simulation
%
[time, x] = ode45(@SimulatorTutorialDynamics, [ti tf], x0);
%ode45 is used to integrate continuous highly complex equations.  Similar in concept
%to using a Riemann Sum, but far more accurate.
%% Plots/Analysis
%unpack
x1=x(:,1);
x2=x(:,2);

%plot
subplot(2,1,1)
plot(time, x1) 
xlabel('Time')
ylabel('Position')
subplot(2,1,2)
plot(time, x2)
xlabel('Time')
ylabel('Velocity')

