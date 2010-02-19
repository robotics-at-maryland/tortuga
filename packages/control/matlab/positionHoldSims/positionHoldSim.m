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
x0=[0 0 0 0 0 1]';



%timing
ti=0;
tf=8;
time=ti:0.04:tf;

%storage arrays
state_storage = 666*ones(length(x0),length(time));
dvl_storage = 666*ones(2,length(time));
control_storage = 666*ones(3,length(time));

dvl_variance_1 = 0;
dvl_variance_2 = 0;

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
state_storage(:,1) = x0;
for t = 2:length(time)
    % simulate measurement
    % [dvl_x1_dot dvl_x2_dot]
    dvl_storage(:,t-1) = bRn(state_storage(5,t-1))*[state_storage(3,t-1) + random('norm',0,dvl_variance_1); state_storage(4,t-1) + random('norm',0,dvl_variance_2)];
    
    % do control
    Fb_controller = [10 0]';
    Fn_controller = bRn(state_storage(5,t-1))'*Fb_controller;
    
    % integrate dynamics (simulate the world)
    [tmp, est] = ode45(@positionHoldSimDynamics,[time(t-1),time(t)],state_storage(:,t-1));
    state_storage(:,t) = est(end,:);
end

%[time_ode,x]=ode45(@positionHoldSimDynamics,time,x0);

figure(1)
subplot(2,1,1)
plot(time,state_storage(1,:))
xlabel('time');
ylabel('x_1');
subplot(2,1,2)
plot(time,state_storage(2,:))
xlabel('time');
ylabel('x_2');

figure(2)
subplot(2,1,1)
plot(time,state_storage(3,:))
xlabel('time');
ylabel('xdot_1');
subplot(2,1,2)
plot(time,state_storage(4,:))
xlabel('time');
ylabel('xdot_2');

figure(3)
subplot(2,1,1)
plot(time,state_storage(5,:))
xlabel('time');
ylabel('\theta');
subplot(2,1,2)
plot(time,state_storage(6,:))
xlabel('time');
ylabel('\thetadot');

figure(4)
subplot(2,1,1)
plot(time(1:end-1),dvl_storage(1,1:end-1))
xlabel('time');
ylabel('dvl_1');
subplot(2,1,2)
plot(time(1:end-1),dvl_storage(2,1:end-1))
xlabel('time');
ylabel('dvl_2');
