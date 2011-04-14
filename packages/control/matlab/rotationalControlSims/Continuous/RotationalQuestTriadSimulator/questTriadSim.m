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
tf=10;
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
angle0 = (pi/180)*45; 
q0 = [axis0*sin(angle0/2); cos(angle0/2)];
q0 = q0/norm(q0);

%Initial angular rate (of B w.r.t. N written in B frame)
w0=(pi/180)*[-20 30 70]';

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

% Error magnitudes
acc_mean = [0 0 0]';       acc_var = [3.7278e-04 2.2597e-04 3.4584e-04]';
mag_mean = [0 0 0]';        mag_var = [1.1013e-04 1.1122e-04 1.1210e-04]';
vel_mean = [0 0 0]';   vel_var = [0.0033 0.0029 0.0036]';

%% For loop or ODE45
 
for i=2:1:length(time)
    %measurement
%   simulate measurements
%   acc=R(q)*acc_vec_nf + noise
    acc=R(state_storage(1:4,i-1))*acc_vec_nf + random('norm', acc_mean, acc_var);
%   mag=R(q)*mag_vec_nf + noise
    mag=R(state_storage(1:4,i-1))*mag_vec_nf + random('norm', mag_mean, mag_var);
%   vel=angular_velocity+noise
    vel=state_storage(5:7,i-1) + random('norm',vel_mean, vel_var);
       
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
    qtriad_storage(1:4, i) = quaternionFromnCb(bRn');
    
    
    
    %quest(only allowed to access acc, mag, and vel)
    cos_func = dot(mag,acc)*dot(mag_vec_nf,acc_vec_nf) + norm(cross(mag,acc))*norm(cross(mag_vec_nf,acc_vec_nf));
    eig_val_max = sqrt(a1^2 + 2*a1*a2*cos_func + a2^2);
    B = a1*mag*mag_vec_nf' + a2*acc*acc_vec_nf';
    S = B + B';
    Z = a1*cross(mag,mag_vec_nf) + a2*cross(acc,acc_vec_nf);
    sigma2 = 0.5*trace(S);
    delta = det(S);
    kappa = trace(inv(S)*det(S));       %Note: Assumes S is invertible%
    alpha = eig_val_max^2 - sigma2^2 + kappa;
    beta = eig_val_max - sigma2;
    gamma = (eig_val_max + sigma2)*alpha - delta;
    X = (alpha*eye(3) + beta*S + S*S)*Z;
    %Ensuring that q_opt is normalized
    q_quest = 1/sqrt(gamma^2 + norm(X)^2)*[X; gamma];
    q_quest = q_quest/norm(q_quest);
    %save quest
    %someone had the first term as q(4) here, probably was the problem
    qQuest_storage(1:4, i) = [q_quest(1) q_quest(2) q_quest(3) q_quest(4)];

    %integrate
    [timeDontCare xi] = ode45(@questTriadDynamics,[time(i-1) time(i)],state_storage(:,i-1));
    state_storage(:,i) = xi(end,:)';
    
end
%[time,x] = ode45(@questTriadDynamics,[t0 te],x0);

%% Plot Data (indices 2:end because index 1 has no estimation )

figure(1)

subplot(4,1,1)
hold on
plot(time(2:end), state_storage(1,2:end), 'b')
plot(time(2:end), qtriad_storage(1,2:end), 'g')
plot(time(2:end), qQuest_storage(1,2:end), 'r')
title('Rotation Quaternion')
ylabel('q_1')
legend('Actual','Triad','Quest')
hold off

subplot(4,1,2)
hold on
plot(time(2:end), state_storage(2,2:end), 'b')
plot(time(2:end), qtriad_storage(2,2:end), 'g')
plot(time(2:end), qQuest_storage(2,2:end), 'r')
ylabel('q_2')
legend('Actual','Triad','Quest')
hold off

subplot(4,1,3)
hold on
plot(time(2:end), state_storage(3,2:end), 'b')
plot(time(2:end), qtriad_storage(3,2:end), 'g')
plot(time(2:end), qQuest_storage(3,2:end), 'r')
ylabel('q_3')
legend('Actual','Triad','Quest')
hold off

subplot(4,1,4)
hold on
plot(time(2:end), state_storage(4,2:end), 'b')
plot(time(2:end), qtriad_storage(4,2:end), 'g')
plot(time(2:end), qQuest_storage(4,2:end), 'r')
ylabel('q_4')
legend('Actual','Triad','Quest')
hold off

xlabel('time (s)')

%% Plotting error in angle theta
%q_err = qXq^-1
%q = [eps n]
%2arcsin(norm(eps))=mag(theta)


for j = 1:length(state_storage)
    a=state_storage(4,j);
    state_storage(2:4,j)=state_storage(1:3,j);
    state_storage(1,j)=a;
    a=qtriad_storage(4,j);
    qtriad_storage(2:4,j)=qtriad_storage(1:3,j);
    qtriad_storage(1,j)=a;
    a=qQuest_storage(4,j);
    qQuest_storage(2:4,j)=qQuest_storage(1:3,j);
    qQuest_storage(1,j)=a;
    triad_err(:,j) = quatmultiply(quatinv(state_storage(1:4,j)'),qtriad_storage(:,j)');
    triad_err_theta(j) = 2*asind(norm(triad_err(2:4,j)));
    quest_err(:,j) = quatmultiply(quatinv(state_storage(1:4,j)'),qQuest_storage(:,j)');
    quest_err_theta(j) = 2*asind(norm(quest_err(2:4,j)));
end

figure(2)
title('Error angle between real and estimated quaternions')

hold on
plot(time(2:end), triad_err_theta(2:end), 'b');
plot(time(2:end), quest_err_theta(2:end), 'r');
legend('Triad error', 'Quest error')
ylabel('Error Angle (degrees)')
xlabel('time (s)')

figure(3)
title('Error quaternion between real and estimated quaternions')
xlabel('time (s)')

subplot(4,1,1)
hold on
plot(time(2:end), triad_err(2,2:end), 'b');
plot(time(2:end), quest_err(1,2:end), 'r');
hold off
legend('Triad error', 'Quest error')
ylabel('q_1')

subplot(4,1,2)
hold on
plot(time(2:end), triad_err(3,2:end), 'b');
plot(time(2:end), quest_err(2,2:end), 'r');
hold off
legend('Triad error', 'Quest error')
ylabel('q_2')

subplot(4,1,3)
hold on
plot(time(2:end), triad_err(4,2:end), 'b');
plot(time(2:end), quest_err(3,2:end), 'r');
hold off
legend('Triad error', 'Quest error')
ylabel('q_3')

subplot(4,1,4)
hold on
plot(time(2:end), triad_err(1,2:end), 'b');
plot(time(2:end), quest_err(4,2:end), 'r');
hold off
legend('Triad error', 'Quest error')
ylabel('q_4')



%I screwed something up here(maybe), it doesn't work
%this should display the final estimate orientation vectors
%now actually displays, but does not appear correct, is defiitely calcs
%also the actual error in angle is pretty big
theta_quest=2*acosd(qQuest_storage(1,1:end));
theta_triad=2*acosd(qtriad_storage(1,1:end));
theta_actual=2*acosd(state_storage(1,1:end));
theta_triad_error=theta_triad-theta_actual
theta_quest_error=theta_quest-theta_actual
%noticed that quest's error angle seemed to be exceeding 360
for k=1:length(theta_quest)
    if theta_quest_error(k)>360
        theta_quest_error(k)=theta_quest_error(k)-floor(theta_quest_error(k)/360)*360;
    end
end

euler(1,:,:)=(qtriad_storage(1:3,1:end)/norm(qtriad_storage(1:4,1:end)));%./sin(theta_triad/2))
euler(2,:,:)=(qQuest_storage(1:3,1:end)/norm(qQuest_storage(1:4,1:end)));%./sin(theta_quest/2))
euler(3,:,:)=(state_storage(1:3,1:end)/norm(state_storage(1:4,1:end)))
%attempting to plot the error in the euler coordinates

triad_err2(1:3,:)=euler(1,1:3,:)-euler(3,1:3,:);
quest_err2(1:3,:)=euler(2,1:3,:)-euler(3,1:3,:);
triad_err2(1,2:end)
figure(5)
title('Error euler angles between real and estimated euler angles')
xlabel('time (s)')

subplot(4,1,1)
hold on
plot(time(2:end), triad_err2(1,2:end), 'b')
plot(time(2:end), quest_err2(1,2:end), 'r')
hold off
legend('Triad error', 'Quest error')
ylabel('e_1')

subplot(4,1,2)
hold on
plot(time(2:end), triad_err2(2,2:end), 'b');
plot(time(2:end), quest_err2(2,2:end), 'r');
hold off
legend('Triad error', 'Quest error')
ylabel('e_2')

subplot(4,1,3)
hold on
plot(time(2:end), triad_err2(3,2:end), 'b');
plot(time(2:end), quest_err2(3,2:end), 'r');
hold off
legend('Triad error', 'Quest error')
ylabel('e_3')

subplot(4,1,4)
hold on
plot(time(2:end), theta_triad(2:end)-theta_actual(2:end), 'b');
plot(time(2:end), theta_quest(2:end)-theta_actual(2:end), 'r');
hold off
legend('Triad error', 'Quest error')
ylabel('theta')

figure(4)

hold on
Quiver3(0,0,0,euler(1,1,end).*theta_triad(end),euler(1,2,end).*theta_triad(end), euler(1,3,end).*theta_triad(end),'-b');
Quiver3(0,0,0,euler(2,1,end).*theta_quest(end),euler(2,2,end).*theta_quest(end), euler(2,3,end).*theta_quest(end),'-r');
Quiver3(0,0,0,euler(3,1,end).*theta_actual(end),euler(3,2,end).*theta_actual(end), euler(3,3,end).*theta_actual(end),'-k');
axis([-1,1,-1,1,-1,1])



