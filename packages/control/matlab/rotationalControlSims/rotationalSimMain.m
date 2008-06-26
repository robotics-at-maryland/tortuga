%simulate tortuga rotational dynamics
clc
close all;
clear;

%% initialization

%initial position
axis0=[1 0 0]';
angle0=20*pi/180;
%q0=[axis0*sin(angle0/2); cos(angle0/2)];
q0=[0 0 0 1]';

%initial angular rate
w0=(pi/180)*[0 0 0]';

%initial desired position
qd0=[0 0 0 1]';

%initial desired angular rate
wd0=(pi/180)*[0 0 0]';

x0=[q0; w0; qd0; wd0];


%% constants

%system inertia
global H;
%this inertia matrix is from Tom Capon's CAD model of Tortuga 2 
% as of 2008-4-8
% H=[1543 3 -60;
%     3 6244 45;
%     -60 45 6362];%in lbm*in^2

H=1500*eye(3);
%convert to kg*m^2
H=(0.45359/39.37^2)*H;

%controller gain
global Kd;
Kd=1*eye(3);%typically choose Kd=1*eye(3)
global lambda;
lambda=1;%typically choose lambda=1

%drag constants
global Cd;
Cd=0.01*diag([1 5 5]);%i made these numbers up

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
a_inertial = [0 0 -1];
global m_inertial;
m_inertial = [0 0.1 -0.1732];

%% timing
t0=0;
te=10;

%% simulation
options=odeset;
options=odeset(options,'AbsTol',1e-3,'MaxStep',0.05);
%[time,x] = ode45(@rotationalSimDynamics,[t0,te],x0);
[time,x] = ode45(@rotationalSimDynamics,[t0,te],x0,options);


%% format results
%recall that x = [q; w; qhat; bhat];
%grab results
q=x(:,1:4);%actual angular position
w=x(:,5:7);%actual angular rate
qd=x(:,8:11);%desired angular position
wd=x(:,12:14);%desired angular rate

%% plot errors
%storage arrays for error metrics
attitude_error=zeros(length(time),1);

for i=1:length(time)
    qc_tilde=quaternionProduct(q(i,:)',qd(i,:)');
    attitude_error(i)=norm(qc_tilde(1:3));
end

figure(1)
plot(time,(180/pi)*attitude_error)
xlabel('time (sec)')
ylabel('|attitude error| (deg)')

%% plot position response
figure(2)
subplot(4,1,1)
plot(time,q(:,1),time,qd(:,1))
ylabel('q_1')
legend('actual','desired')
subplot(4,1,2)
plot(time,q(:,2),time,qd(:,2))
ylabel('q_2')
subplot(4,1,3)
plot(time,q(:,3),time,qd(:,3))
ylabel('q_3')
subplot(4,1,4)
plot(time,q(:,4),time,qd(:,4))
ylabel('q_4')
xlabel('time (s)')

%% plot velocity response
figure(3)
subplot(3,1,1)
plot(time,w(:,1)*180/pi,time,wd(:,1)*180/pi)
ylabel('w_1 in deg/s')
legend('actual','desired')
subplot(3,1,2)
plot(time,w(:,2)*180/pi,time,wd(:,2)*180/pi)
ylabel('w_2 in deg/s')
subplot(3,1,3)
plot(time,w(:,3)*180/pi,time,wd(:,3)*180/pi)
ylabel('w_3 in deg/s')
xlabel('time (s)')

R(q(end,:))'*[1 0 0]'