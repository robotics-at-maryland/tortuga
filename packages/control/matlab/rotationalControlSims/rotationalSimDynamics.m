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


%% unpack ODE data

q=x(1:4);
w=x(5:7);
qd=x(8:11);
wd=x(12:14);

%fix numerical quaternion drift
q=q/norm(q,2);
qd=qd/norm(qd,2);

%% measurement
%perfect measurement for now
qmeas=q;
wmeas=w;

%% controller

%propagate desired states
%desire constant angular rate for now
dwd=zeros(3,1);
%desired angular position varies
dqd=(1/2)*Q(qd)*wd;

%compute attitude error qc_tilde (controller quaternion)
qc_tilde=quaternionProduct(qmeas,qd);

%compute composite error metric s
wr=R(qc_tilde)*wd-lambda*qc_tilde(1:3);
shat=wmeas-wr;

%compute angular rate error wc_tilde
wc_tilde=wmeas-R(qc_tilde)*wd;

%d/dt(wrhat)=alpharhat
dwr=R(qc_tilde)*dwd-S(wc_tilde)*R(qc_tilde)*wd-lambda*Q1(qc_tilde)*wc_tilde;

u=-Kd*shat+H*dwr-S(H*wmeas)*wr;

%% dynamics

%drag
%drag=zeros(3,1);%use this line to turn off drag
drag=Cd*diag([abs(w(1)) abs(w(2)) abs(w(3))])*w;

%buoyant moment
%buoyant=zeros(3,1);%use this line to turn off buoyant moment
Rot=R(qmeas);
buoyant=fb*[(r(2)*Rot(3,3)-r(3)*Rot(2,3));
            (r(3)*R(1,3)-r(1)*R(3,3));
            (r(1)*R(2,3)-r(2)*R(1,3))];

%propagate actual vehicle dynamics
dw=inv(H)*(S(H*w)*w+u-drag);

%propagate actual vehicle kinematics
dq=(1/2)*Q(q)*w;



%put output states into vector for ode45
dx=[dq; dw; dqd; dwd];