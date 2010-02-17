%position hold sim v1
%
%Joseph Galante  2010-2-2

clear all
close all
clc

%% initialization

%state vector
%position (m) and velocity (m/s) in inertial frame
%orientation (rad) is angle from inertial frame to body frame
%state = [r1 r2 r1Dot r2Dot theta thetaDot]
x0=[1 2 2 2 0 0]';



%timing
ti=0;
tf=8;
time=ti:0.04:tf;

%storage arrays
state_storage = 666*ones(length(x0),length(time));
dvl_storage = 666*ones(2,length(time));
control_storage = 666*ones(3,length(time));

%system constants
global m;%mass (kg)
m=20;
global h;%inertial about yaw axis (N*m*s^2)
h=10;
global drag_1;%translational drag along 1 axis
drag_1=5;
global drag_2;%translational drag along 2 axis
drag_2=15;
global drag_rot;%rotational drag about yaw axis
drag_rot=7;

%force output (to be changed by controller)
global Fn_controller;
Fn_controller=[0 0]';
global Tn_controller;
Tn_controller=0;


%% simulation

%for 2:length(time)

%simulate DVL here

%[time,x]=ode45(@positionHoldSimDynamics,[t(i-1) t(i)],x(i-1))

%end

[time_ode,x]=ode45(@positionHoldSimDynamics,time,x0);

figure(1)
subplot(2,1,1)
plot(time,x(:,1))
subplot(2,1,2)
plot(time,x(:,2))

