%simulate tortuga rotational dynamics
clc
close all;
clear;

%% initialization

%initial position
axis0=[1 1 1]';
angle0=30*pi/180;
q0=[axis0*sin(angle0/2); cos(angle0/2)];
q0=q0/norm(q0);
%q0=[0 0 0 1]';

%initial angular rate
w0=(pi/180)*[0 0 0]';

%initial desired position
qd0=[0 0 0 1]';

%initial desired angular rate
wd0=(pi/180)*[0 0 0]';

%initial estimated position
qhat0=q0;

%initial parameter estimate
%ahat=[h11 h12 h13 h22 h23 h33 rb1*fb rb2*fb rb3*fb cd1 cd2 cd3]
%h - elements of inertial matrix
%rb - vector from center of gravity to center of buoyancy
%fb - buoyant force
%cd - drag parameters (model is c*w*abs(w))
%      h                rb    c
ahat0=[0.5 0 -0.1 1 0 1 0 0 0 1 2 2]';

%initial estimated angular rate
%what0=w0;

x0=[q0; w0; qd0; wd0; qhat0; ahat0];


%% constants

%system inertia
global H;
%this inertia matrix is from Tom Capon's CAD model of Tortuga 2 
% as of 2008-4-8
 H=[1543 3 -60;
     3 6244 45;
     -60 45 6362];%in lbm*in^2

%H=1500*eye(3);
%convert to kg*m^2
H=(0.45359/39.37^2)*H;

%controller gain
global Kd;
Kd=5*eye(3);%typically choose Kd=1*eye(3)
%replacement model pole
global lambda;
lambda=1;%typically choose lambda=1
%adaptation gain matrix
global Gamma;
Gamma=1;%can be matrix (12x12) or scalar

%drag constants
global Cd;
Cd=1*diag([1 5 5]);%i made these numbers up

%buoyant force
global fb;
fb=4;%newtons, i made this number up
%fb=8;%newtons, i made this number up
%vector from center of gravity (CG) to center of buoyancy (CB)
global rb;
rb=[0 0 0.05]';%meters, i made these numbers up
%rb=[0 0 1]';%meters, i made these numbers up

% gravity and magnetic vectors in earth's(inertial) frame
% [1 2 3]
% 1 - North
% 2 - West
% 3 - Up
global a_inertial;
a_inertial = [0 0 -1]';
global m_inertial;
m_inertial = [0.1 0 -0.1732]';

%% timing
t0=0;
te=30;

%% simulation
options=odeset;
options=odeset(options,'AbsTol',1e-3,'MaxStep',0.05);
%[time,x] = ode45(@rotationalSimDynamics,[t0,te],x0);
[time,x] = ode45(@adaptiveRotationalSimDynamics,[t0,te],x0,options);


%% format results
%recall that x = [q; w; qd; wd; qhat; ahat];
%grab results
q=x(:,1:4);%actual angular position
w=x(:,5:7);%actual angular rate
qd=x(:,8:11);%desired angular position
wd=x(:,12:14);%desired angular rate
qhat=x(:,15:18);%estimated angular position
ahat=x(:,19:30);%estimated parameters
%what=x(:,19:21);%estimated angular rate

%% plot errors
%storage arrays for error metrics
attitude_error=zeros(length(time),1);

for i=1:length(time)
    qc_tilde=quaternionProduct(q(i,:)',qd(i,:)');
    attitude_error(i)=norm(qc_tilde(1:3));
end

figure(1)
plot(time,(180/pi)*attitude_error,'linewidth',4)
xlabel('time (sec)')
ylabel('|attitude error| (deg)')

%% plot position response
figure(2)
subplot(4,1,1)
plot(time,qd(:,1),time,q(:,1),'linewidth',4)
ylabel('q_1')
legend('desired','actual')
subplot(4,1,2)
plot(time,qd(:,2),time,q(:,2),'linewidth',4)
ylabel('q_2')
subplot(4,1,3)
plot(time,qd(:,3),time,q(:,3),'linewidth',4)
ylabel('q_3')
subplot(4,1,4)
plot(time,qd(:,4),time,q(:,4),'linewidth',4)
ylabel('q_4')
xlabel('time (s)')

%% plot velocity response
figure(3)
subplot(3,1,1)
plot(time,wd(:,1)*180/pi,time,w(:,1)*180/pi,'linewidth',4)
ylabel('w_1 in deg/s')
legend('desired','actual')
subplot(3,1,2)
plot(time,wd(:,2)*180/pi,time,w(:,2)*180/pi,'linewidth',4)
ylabel('w_2 in deg/s')
subplot(3,1,3)
plot(time,wd(:,3)*180/pi,time,w(:,3)*180/pi,'linewidth',4)
ylabel('w_3 in deg/s')
xlabel('time (s)')

%% plot gyro only quaternion estimation
% figure(4)
% subplot(4,1,1)
% plot(time,q(:,1),time,qhat(:,1))
% ylabel('q_1')
% legend('actual','estimated')
% title('Quaternion Estimation by Gryo Integration')
% subplot(4,1,2)
% plot(time,q(:,2),time,qhat(:,2))
% ylabel('q_2')
% subplot(4,1,3)
% plot(time,q(:,3),time,qhat(:,3))
% ylabel('q_3')
% subplot(4,1,4)
% plot(time,q(:,4),time,qhat(:,4))
% ylabel('q_4')
% xlabel('time (s)')

%% plot learned values - inertia
figure(5)
subplot(3,2,1)
plot(time,H(1,1)*ones(length(time),1),time,ahat(:,1),'linewidth',4)
ylabel('H_1_,_1')
subplot(3,2,2)
plot(time,H(1,2)*ones(length(time),1),time,ahat(:,2),'linewidth',4)
ylabel('H_1_,_2')
legend('estimated','actual')
subplot(3,2,3)
plot(time,H(1,3)*ones(length(time),1),time,ahat(:,3),'linewidth',4)
ylabel('H_1_,_3')
subplot(3,2,4)
plot(time,H(2,2)*ones(length(time),1),time,ahat(:,4),'linewidth',4)
ylabel('H_2_,_2')
subplot(3,2,5)
plot(time,H(2,3)*ones(length(time),1),time,ahat(:,5),'linewidth',4)
ylabel('H_2_,_3')
xlabel('Time')
subplot(3,2,6)
plot(time,H(3,3)*ones(length(time),1),time,ahat(:,6),'linewidth',4)
ylabel('H_3_,_3')
xlabel('time (s)')

%% plot learned values - buoyancy
figure(6)
subplot(3,1,1)
plot(time,rb(1)*fb*ones(length(time),1),time,ahat(:,7),'linewidth',4)
ylabel('r_b_1*f_b')
subplot(3,1,2)
plot(time,rb(2)*fb*ones(length(time),1),time,ahat(:,8),'linewidth',4)
ylabel('r_b_2*f_b')
subplot(3,1,3)
plot(time,rb(3)*fb*ones(length(time),1),time,ahat(:,9),'linewidth',4)
ylabel('r_b_3*f_b')
xlabel('time (s)')

%% plot learned values - drag
figure(7)
subplot(3,1,1)
plot(time,Cd(1)*ones(length(time),1),time,ahat(:,10),'linewidth',4)
ylabel('C_d_1')
subplot(3,1,2)
plot(time,Cd(2)*ones(length(time),1),time,ahat(:,11),'linewidth',4)
ylabel('C_d_2')
subplot(3,1,3)
plot(time,Cd(3)*ones(length(time),1),time,ahat(:,12),'linewidth',4)
ylabel('C_d_3')
xlabel('time (s)')

%final heading of vehicle
%R(q(end,:))'*[1 0 0]'