function dx=rotationalSimDynamics(t,x)

%% grab constants from SimMain

%system inertia
global H;

%controller gain
global Kd;
global lambda;

%drag
global Cd;

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
drag=Cd*diag([abs(w(1)) abs(w(2)) abs(w(3))])*w;

%propagate actual vehicle dynamics
dw=inv(H)*(S(H*w)*w+u-drag);

%propagate actual vehicle kinematics
dq=(1/2)*Q(q)*w;



%put output states into vector for ode45
dx=[dq; dw; dqd; dwd];