%position hold sim v1
%Joseph Galante  2010-2-2

clear all
close all
clc

%% initialization

%state vector
%position (m) and velocity (m/s) in inertial frame
%orientation (rad) is angle from inertial frame to body frame
%state = [r1 r2 r1Dot r2Dot theta thetaDot]
global x0;
x0=[0 0 0 0 0 0]';

% List of Controllers
% 1. PID_velocity - velocity based pid controller "drives velocity error to zero"
% 2. PID_positional - position based pid controller "drives position error
% to zero and keeps track of position estimate"

CONTROLLER = 'PID_positional';

%   k = [ kp  kd  ki ] <-- x_1
%       [ kp  kd  ki ] <-- x_2
%       [ kp  kd  ki ] <-- theta
%
%
%   state_desired = [ x_1       ]
%                   [ x_1_dot   ]
%                   [ x_2       ]
%                   [ x_2_dot   ]
%                   [ theta     ]
%                   [ theta_dot ]

% Controller gains
k_PID_velocity = [0 0 0; 0 0 0; 0 0 0];
k_PID_positional = [100 40 5; 100 40 5; 100 40 5];


%timing
global t_step;
global current_time;
ti=0;
tf=8;
t_step = .04;
time=ti:t_step:tf;

%storage arrays
state_storage = 666*ones(length(x0),length(time));
dvl_storage = 666*ones(2,length(time));
control_storage = 666*ones(3,length(time));
disturbances_storage = 666*ones(3,length(time));

% DVL Noise ~22 mm/s = .0022 m/s
dvl_variance_1 = 0.0022;
dvl_variance_2 = 0.0022;

% Random Currents
dist_Fn_variance = 1;
dist_Tn_variance = 1;

%system constants
global m;%mass (kg)
m=10;
global h;%inertial about yaw axis (N*m*s^2)
h=5;
global drag_1;%translational drag along 1 axis
drag_1=5;
global drag_2;%translational drag along 2 axis
drag_2=15;
global drag_rot;%rotational drag about yaw axis
drag_rot=10;

%force output (to be changed by controller)
global Fn_controller;
Fn_controller=[0 0]';
global Fn_disturbances;
Fn_disturbances = [0 0]';
global Tn_controller;
Tn_controller=0;
global Tn_disturbances;
Tn_disturbances = 0;


%% simulation
state_storage(:,1) = x0;
control_storage(:,1) = [0 0 0];
for t = 2:length(time)
    current_time = (t-1)*t_step;
    % simulate measurement
    % [dvl_x1_dot dvl_x2_dot]
    dvl_storage(:,t-1) = bRn(state_storage(5,t-1))*[state_storage(3,t-1) + random('norm',0,dvl_variance_1); state_storage(4,t-1) + random('norm',0,dvl_variance_2)];
    
    % store state for passing to controllers [r_dot1 r_dot2 theta_dot]
    state_estimate = [dvl_storage(:,t-1); state_storage(6,t-1)];
    % do control
    switch(CONTROLLER)
        case 'PID_velocity'
            k = k_PID_velocity;
            state_desired = state_desired_velocity;
            [Fb_controller , Tb_controller] = positionHold_PID_velocity(state_estimate, k, state_desired);
        case 'PID_positional'
            k = k_PID_positional;
            state_desired = [log(current_time), 0, 0, 1/current_time, 0, 0]';
            [Fb_controller, Tb_controller] = positionHold_PID_positional(state_estimate, k, state_desired);
    end
    
    % transform vehicle forces to inertial frame
    Fn_controller = bRn(state_storage(5,t-1))'*Fb_controller;
    Tn_controller = Tb_controller;
   
    % generate new random velocity disturbance every .5 seconds
    if(mod(uint32(current_time*10),5) == 0)
        Fn_disturbances = [random('norm',0,dist_Fn_variance) random('norm',0,dist_Fn_variance)]';
        Tn_disturbances = random('norm',0,dist_Tn_variance);
    end
    
    % store disturbance forces history
    disturbances_storage(:,t-1) = [Fn_disturbances;Tn_disturbances];
    
    % store controller forceing history
    control_storage(:,t) = [Fn_controller;Tn_controller];
    
    % integrate dynamics (simulate the world)
    [tmp, est] = ode45(@positionHoldSimDynamics,[time(t-1),time(t)],state_storage(:,t-1));
    state_storage(:,t) = est(end,:);
end

% clear persistent variables in the controllers
clear positionHold_PID_velocity
clear positionHold_PID_positional

figure(1)
title('true position in inertial frame');
subplot(2,1,1)
plot(time,state_storage(1,:))
ylabel('x_1');
subplot(2,1,2)
plot(time,state_storage(2,:))
xlabel('time');
ylabel('x_2');

figure(2)
title('true velocity');
subplot(2,1,1)
plot(time,state_storage(3,:))
ylabel('x_1dot');
subplot(2,1,2)
plot(time,state_storage(4,:))
xlabel('time');
ylabel('x_2dot');

figure(3)
title('true orientation and angular velocity');
subplot(2,1,1)
plot(time,state_storage(5,:))
ylabel('\theta');
subplot(2,1,2)
plot(time,state_storage(6,:))
xlabel('time');
ylabel('\theta dot');

figure(4)
title('dvl velocity data');
subplot(2,1,1)
plot(time(1:end-1),dvl_storage(1,1:end-1))
ylabel('dvl_1');
subplot(2,1,2)
plot(time(1:end-1),dvl_storage(2,1:end-1))
xlabel('time');
ylabel('dvl_2');

figure(5)
title('controller forcings');
subplot(3,1,1)
plot(time,control_storage(1,:))
ylabel('^bF_1');
subplot(3,1,2)
plot(time,control_storage(2,:))
ylabel('^bF_2');
subplot(3,1,3)
plot(time,control_storage(3,:))
xlabel('time');
ylabel('\theta');

% Controller specific plots
switch(CONTROLLER)
    case 'PID_positional'
        global position_storage;
        global orientation_storage;
        
        l = length(position_storage(1,:));
        
        % Plot what the controller thinks is happening
        figure(6)
        title('controller estimates');
        subplot(3,1,1)
        plot(time(1:l),position_storage(1,:))
        ylabel('x_1');
        subplot(3,1,2)
        plot(time(1:l),position_storage(2,:))
        ylabel('x_2');
        subplot(3,1,3)
        plot(time(1:l),orientation_storage(:))
        xlabel('time');
        ylabel('\theta');
        
        % Plot the difference between what the controller thinks is
        % happening and what is really happening
        figure(7)
        title('absolute error of controller estimates');
        subplot(3,1,1)
        plot(time(1:l),abs(state_storage(1,1:l)-position_storage(1,:)));
        ylabel('x_1');
        subplot(3,1,2)
        plot(time(1:l),abs(state_storage(2,1:l)-position_storage(2,:)));
        ylabel('x_2');
        subplot(3,1,3)
        plot(time(1:l),abs(state_storage(5,1:l)-orientation_storage(1:l)));
        xlabel('time');
        ylabel('\theta');
end

