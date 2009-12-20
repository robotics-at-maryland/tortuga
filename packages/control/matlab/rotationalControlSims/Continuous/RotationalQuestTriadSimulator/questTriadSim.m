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
ti=0;
tf=8;
time = ti:0.04:tf;
state_storage = 666*ones(7, length(time));
accel_meas_storage = 666*ones(3, length(time));
mag_meas_storage = 666*ones(3, length(time));
qtriad_storage = 666*ones(4, length(time));
qQuest_storage = 666*ones(4, length(time));


%Initial Position with respect to inertial frame
%Initial Quaternion:
axis0 = [0 0 1]';
axis0=axis0/norm(axis0);
angle0 = (pi/180)*10;  % starting in a level position
q0 = [axis0*sin(angle0/2); cos(angle0/2)];
q0 = q0/norm(q0);

%Initial angular rate
w0=(pi/180)*20*[1 0 0]';

%initial estimated position
x0=[q0; w0];
        
state_storage(:,1)=x0;
        
%Constants:
%global mag_vec_nf;
mag_vec_nf = [cos(60*pi/180) 0 sin(60*pi/180)]'; %Note: the declination angle may be incorrect
%global acc_vec_nf;
acc_vec_nf = [0 0 -1]';
%global a1;
a1 = 0.5;           %Weight of magnetic sensor
%global a2;
a2 = 0.5;           %Weight of acceleration sensor
    
%System Inertia (Inertia Matrix)
global H;
H= [1 0 0;
    0 2 0;
    0 0 2];

% Drag Constants
global Cd;
Cd=4*diag([1 1 1]);

% Buoyant Force
global fb;
fb=4;

% Vector from center of gravity (CG) to center of buoyancy (CB)
global rb;
rb=[0 0 1]';

%% For loop or ODE45
 
for i=2:1:length(time)%:i<length(time)+1...I don't understand this part too well
    %measurement
%     accel=R(q) *accel_vec_nf+noise  % These should stem from the  rotation matrix, or unit quaternion in some respect
%     mag=f(q)
%     
    %estimation
    %triad goes here
    %quest goes here
    
    %integrate
    [timeDontCare xi] = ode45(@questTriadDynamics,[time(i-1) time(i)],state_storage(:,i-1));
    state_storage(:,i) = xi(end,:)';
end
%[time,x] = ode45(@questTriadDynamics,[t0 te],x0);

%% Plot Data

figure(1)
subplot(4,1,1)
plot(time, state_storage(1,:))
subplot(4,1,2)
plot(time, state_storage(2,:))
subplot(4,1,3)
plot(time, state_storage(3,:))
subplot(4,1,4)
plot(time,state_storage(4,:))

% figure(2)
% subplot(4,1,1)
% plot(time,actualQ1,time,triadQ1)
% legend('actual','TRIAD')
