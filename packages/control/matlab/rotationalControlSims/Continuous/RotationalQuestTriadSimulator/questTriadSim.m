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
acc_meas_storage = 666*ones(3, length(time));
mag_meas_storage = 666*ones(3, length(time));
qtriad_storage = 666*ones(4, length(time));
qQuest_storage = 666*ones(4, length(time));


%Initial Position (Quaternion)  
%describes orientation of N w.r.t. B 
% v_{b frame} = R(q) * v_{n frame}
axis0 = [0 0 1]'; % starting in a level position
axis0=axis0/norm(axis0);
angle0 = (pi/180)*0; 
q0 = [axis0*sin(angle0/2); cos(angle0/2)];
q0 = q0/norm(q0);

%Initial angular rate (of B w.r.t. N written in B frame)
w0=(pi/180)*[0 0 40]';

%initial estimated position
x0=[q0; w0];
        
state_storage(:,1)=x0;
        
%Constants:
%global mag_vec_nf;
mag_vec_nf = [cos(60*pi/180) 0 sin(-60*pi/180)]'; %Note: the declination angle may be incorrect
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
 
for i=2:1:length(time)
    %measurement
%   simulate measurements
%   acc=R(q)*acc_vec_nf + noise
    acc=R(state_storage(1:4,i-1))*acc_vec_nf;
%   mag=R(q)*mag_vec_nf + noise
    mag=R(state_storage(1:4,i-1))*mag_vec_nf;
%   vel=angular_velocity+noise
    vel=state_storage(5:7,i-1);
    
    %save measurements
    acc_meas_storage(:,i-1)=acc;
    mag_meas_storage(:,i-1)=mag;
    vel_meas_storage(:,i-1)=vel; 
    
    %estimation
    %triad(only allowed to access acc, mag, and vel)
    n3 = -acc/norm(acc);
    n2 = cross(n3,mag)/norm(mag);
    n1 = cross(n2,n3);
    %accounting for round-off errors
    n1 = n1/norm(n1);
    bRn = [n1 n2 n3];
    %save triad
    qtriad_storage(1:4, i-1) = quaternionFromnCb(bRn');
           
    %quest(only allowed to access acc, mag, and vel)
    cos_func = dot(mag,acc)*dot(mag_vec_nf,acc_vec_nf) + norm(cross(mag,acc))*norm(cross(mag_vec_nf,acc_vec_nf));
    eig_val_max = sqrt(a1^2 + 2*a1*a2*cos_func + a2^2);
    B = a1*mag*mag_vec_nf' + a2*acc*acc_vec_nf';
    S = B + B';
    Z = a1*cross(mag,mag_vec_nf) + a2*cross(acc,acc_vec_nf);
    sigma2 = 0.5*trace(S);
    delta = det(S);
    kappa = trace(inv(S)*det(S));       %Note: Assumes S is invertible
    alpha = eig_val_max^2 - sigma2^2 + kappa;
    beta = eig_val_max - sigma2;
    gamma = (eig_val_max + sigma2)*alpha - delta;
    X = (alpha*eye(3) + beta*S + S*S)*Z;
    %Ensuring that q_opt is normalized
    q_quest = 1/sqrt(gamma^2 + norm(X)^2)*[X; gamma];
    q_quest = q_quest/norm(q_quest);
    %save quest
    qQuest_storage(1:4, i-1) = q_quest;

    %integrate
    [timeDontCare xi] = ode45(@questTriadDynamics,[time(i-1) time(i)],state_storage(:,i-1));
    state_storage(:,i) = xi(end,:)';
end
%[time,x] = ode45(@questTriadDynamics,[t0 te],x0);

%% Plot Data

figure(1)
subplot(4,1,1)
plot(time, state_storage(1,:), 'b')
plot(time, qtriad_storage(1,:), 'g')
plot(time, qQuest_storage(1,:), 'r')
title('Position')
ylabel('q_1')
legend('Actual','Triad','Quest')
subplot(4,1,2)
plot(time, state_storage(2,:), 'b')
plot(time, qtriad_storage(2,:), 'g')
plot(time, qQuest_storage(2,:), 'r')
ylabel('q_2')
legend('Actual','Triad','Quest')
subplot(4,1,3)
plot(time, state_storage(3,:), 'b')
plot(time, qtriad_storage(3,:), 'g')
plot(time, qQuest_storage(3,:), 'r')
ylabel('q_3')
legend('Actual','Triad','Quest')
subplot(4,1,4)
plot(time, state_storage(4,:), 'b')
plot(time, qtriad_storage(4,:), 'g')
plot(time, qQuest_storage(4,:), 'r')
ylabel('q_4')
legend('Actual','Triad','Quest')
xlabel('time (s)')


%figure(2)
%subplot(3,1,1)
%plot(time,state_storage(5,:))
% title('Velocity')
% ylabel('w_1 (rad/s)')
% subplot(3,1,2)
% plot(time,state_storage(6,:))
% ylabel('w_2 (rad/s)')
% subplot(3,1,3)
% plot(time,state_storage(7,:))
% ylabel('w_3 (rad/s)')
% xlabel('time (s)')

% figure(3)
% subplot(3,1,1)
% plot(time(1:end-1),acc_meas_storage(1,1:end-1))
% title('Accelerometer `g`')
% ylabel('a_1')
% subplot(3,1,2)
% plot(time(1:end-1),acc_meas_storage(2,1:end-1))
% ylabel('a_2')
% subplot(3,1,3)
% plot(time(1:end-1),acc_meas_storage(3,1:end-1))
% ylabel('a_3')

%figure(4)
%subplot(3,1,1)
%plot(time(1:end-1),mag_meas_storage(1,1:end-1))
%title('Magnetometer')
%ylabel('m_1')
%subplot(3,1,2)
%plot(time(1:end-1),mag_meas_storage(2,1:end-1))
%ylabel('m_2')
%subplot(3,1,3)
%plot(time(1:end-1),mag_meas_storage(3,1:end-1))
%ylabel('m_3')


%acc_meas_storage = 666*ones(3, length(time));
%mag_meas_storage = 666*ones(3, length(time));


% figure(2)
% subplot(4,1,1)
% plot(time,actualQ1,time,triadQ1)
% legend('actual','TRIAD')

