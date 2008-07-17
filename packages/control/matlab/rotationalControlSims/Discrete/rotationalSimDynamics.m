function dx=rotationalSimDynamics(t,x)

%% grab constants from SimMain

%system inertia
global H;

%controller gain
global Kd;
global lambda;

%drag
global Cd;

%buoyant force
global fb;

%vector from center of gravity (CG) to center of buoyancy (CB)
global rb;

global a_inertial;
global m_inertial;
global u;
global dq_d;
global dw_d;
global dqhat;
%% unpack data
q=x(1:4);
w=x(5:7);
q_d=x(8:11);
w_d=x(12:14);
qhat=x(15:18);
%what=x(19:21);

%% dynamics

%drag
%drag=zeros(3,1);%use this line to turn off drag
drag=Cd*diag([abs(w(1)) abs(w(2)) abs(w(3))])*w;

%buoyant moment
%buoyant=zeros(3,1);%use this line to turn off buoyant moment
Rot=R(q)';
buoyant=fb*[(rb(2)*Rot(3,3)-rb(3)*Rot(2,3));
            (rb(3)*Rot(1,3)-rb(1)*Rot(3,3));
            (rb(1)*Rot(2,3)-rb(2)*Rot(1,3))];

%propagate actual vehicle dynamics
dw=inv(H)*(S(H*w)*w+u-drag-buoyant);
%dw=inv(H)*(S(H*w)*w-drag-buoyant);

%propagate actual vehicle kinematics
dq=(1/2)*Q(q)*w;



%put output states into vector for ode45
dx=[dq; dw; dq_d; dw_d; dqhat];
