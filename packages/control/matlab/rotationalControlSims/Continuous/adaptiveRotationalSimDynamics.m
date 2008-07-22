function dx=adaptiveRotationalSimDynamics(t,x)

%% grab constants from SimMain

%system inertia
global H;

%controller gain
global Kd;
global lambda;
global Gamma;

%drag
global Cd;

%buoyant force
global fb;

%vector from center of gravity (CG) to center of buoyancy (CB)
global rb;

global a_inertial;
global m_inertial;

%% unpack ODE data

q=x(1:4);
w=x(5:7);
q_d=x(8:11);
w_d=x(12:14);
qhat=x(15:18);
ahat=x(19:30);
%what=x(19:21);

%fix numerical quaternion drift
q=q/norm(q,2);
q_d=q_d/norm(q_d,2);

%% measurement
Rot = R(q);
%a_meas = Rot * a_inertial + 0.0005*randn; 
%m_meas = Rot * m_inertial + 0.0005*randn;
a_meas = Rot * a_inertial;
m_meas = Rot * m_inertial;
w_meas=w;

%% estimation

q_meas = quaternionFromnCb(nCbFromIMU(m_meas,a_meas));
%q_meas=q;

%quaternion estimation that requires only angular rate gyro
dqhat = (1/2)*Q(qhat)*w_meas;
%dwhat = 

%% controller

%propagate desired states
%desire constant angular rate for now
dw_d=zeros(3,1);
%desired angular position varies
dq_d=(1/2)*Q(q_d)*w_d;

%compute attitude error qc_tilde (controller quaternion)
qc_tilde=quaternionProduct(q_meas,q_d);

%compute composite error metric s
w_r=R(qc_tilde)*w_d-lambda*qc_tilde(1:3);
shat=w_meas-w_r;

%compute angular rate error wc_tilde
wc_tilde=w_meas-R(qc_tilde)*w_d;

%d/dt(wrhat)=alpharhat
dw_r=R(qc_tilde)*dw_d-S(wc_tilde)*R(qc_tilde)*w_d-lambda*Q1(qc_tilde)*wc_tilde;

%old nonlinear PD controller
%u=-Kd*shat+H*dw_r-S(H*w_meas)*w_r;
%u=-Kd*shat+H*dw_r;%-S(H*w_meas)*w_r;

%find rotation matrix
Rot = R(q);

%parameterization matrix divided in four
%Y=[Yinertia1 Yinertia2 Ybuoyancy Ydrag]
Yinertia1=[dw_r(1) dw_r(2)-w_meas(1)*w_r(3) dw_r(3)+w_meas(1)*w_r(2);
           w_meas(1)*w_r(3) dw_r(1)+w_meas(2)*w_r(3) -w_meas(1)*w_r(1)+w_meas(3)*w_r(3);
           -w_meas(1)*w_r(2) w_meas(1)*w_r(1)-w_meas(2)*w_r(2) dw_r(1)-w_meas(3)*w_r(2)];
       
Yinertia2=[-w_meas(2)*w_r(3) w_meas(2)*w_r(2)-w_meas(3)*w_r(3) w_meas(3)*w_r(2);
           dw_r(2) dw_r(3)-w_meas(2)*w_r(1) -w_meas(3)*w_r(1);
           w_meas(2)*w_r(1) dw_r(2)+w_meas(3)*w_r(1) dw_r(3)];
       
Ybuoyancy=[


Y=[Yinertia1 Yinertia2 Ybuoyancy Ydrag];


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
