%-------------------------------------------------------------------------%
%                                                                         
%       Rotational State Dynamics Simulator for Tortuga                         
%       
%       1.Initialization
%       2.Inside For loop or ode45
%           CANNOT be done by robot:
%               a.Measurements
%           CAN be done by robot:
%               b.Estimation (interperetation of measurements)
%               c.Planning (don't have)
%               d.Control (Execution of plan) (not yet)
%               e.Simulation of dynamics
%       3.Plot results/data file
%
%
%
%       Written by Simon Knister and John Martin. Based on the work of M.D.
%       Shuster and S.D. Oh in "Three-Axis Attitude Determination from Vector
%       Observations". Done with the assistance of Joe Gallante.
%       (c) 2009 for Robotics@Maryland
%
%-------------------------------------------------------------------------%

clear all;
close all;
clc;

%% Initialization

    %Initial Position with respect to inertial frame
        %Initial Rotation Matrix:
        bRn0 = [1 0 0 
                0 1 0 
                0 0 1];
        %Initial Quaternion:
        axis0 = [1 1 1]';
        angle0 = 0;
        q0 = [axis0*sin(angle0*(pi/180)/2); cos(angle0*(pi/180)/2)];
        q0 = q0/norm(q0);
        
        %initial angular rate
        w0=(pi/180)*[0 0 0]';

        %initial desired position
        qd0=[0 0 0 1]';

        %initial desired angular rate
        wd0=(pi/180)*[0 0 0]';

        %initial estimated position
        qhat0=q0;

x0=[q0; w0; qd0; wd0; qhat0];
        
    
    %Constants:
    global mag_vec_nf;
    mag_vec_nf = [cos(60*pi/180) 0 sin(60*pi/180)]'; %Note: the declination angle may be incorrect
    global acc_vec_nf;
    acc_vec_nf = [0 0 -1]';
    global a1;
    a1 = 0.5;           %Weight of magnetic sensor
    global a2;
    a2 = 0.5;           %Weight of acceleration sensor
    
        %System Inertia
        global H;
        H= [0 0 0
            0 0 0
            0 0 0];%I don't know what this is!!!

        %Drag Constants
        global Cd;
        Cd=diag([1 1 1]);%I don't know what this is!!!

        %Buoyant Force
        global fb;
        fb=4;%Also don't know this
        
        %vector from center of gravity (CG) to center of buoyancy (CB)
        global rb;
        rb=[0 0 1]';%Or this!!
    
        %Timing
        t0 = 0;
        te = 100;

%% For loop or ODE45
  
[time,x] = ode45(@RotationalStateDynamics,[t0 te],x0);
