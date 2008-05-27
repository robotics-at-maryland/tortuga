%simulate tortuga rotational dynamics

close all;
clear;

%% initialization

%initial position
axis0=[0 0 1]';
angle0=10*pi/180;
q0=[axis0*sin(angle0/2); cos(angle0/2)]';

%initial angular rate
w0=(pi/180)*[0 0 0]';

x0=[q0; w0];


%% constants

%system inertia
global H;
%this inertia matrix is from Tom Capon's CAD model of Tortuga 2 
% as of 2008-4-8
H=[1543 3 -60;
    3 6244 45;
    -60 45 6362];%in lbm*in^2
%convert to kg*m^2
H=(0.45359/39.37^2)*H;

%controller gain
global Kd;
Kd=1*eye(3);



%% timing
t0=0;
te=100;

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

