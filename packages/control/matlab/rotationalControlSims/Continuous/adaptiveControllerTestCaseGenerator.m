%% controller test case 1

clear

%% test case input

%desired
q_d=[0 0 0 1]';
w_d=[0 0 0]';

%measured
q_meas=[0 0 0 1]';
w_meas=[0 0 0]';

%gains
lambda=1;
Gamma=1;
Kd=1;

%estimated parameters
ahat=[0.5 0 -0.1 1 0 1 0 0 0 1 2 2]';

%timing
dt=0.2;


%% actual controller

%propagate desired states
%desire constant angular rate for now
dw_d=zeros(3,1);
%desired angular position varies
dq_d=(1/2)*Q(q_d)*w_d;

%integrate the desired states
w_d=w_d+dw_d*dt
q_d=q_d+dq_d*dt

%compute attitude error qc_tilde (controller quaternion)
qc_tilde=quaternionProduct(q_meas,q_d);

%compute composite error metric s
w_r=R(qc_tilde)*w_d-lambda*qc_tilde(1:3);
shat=w_meas-w_r;

%compute angular rate error wc_tilde
wc_tilde=w_meas-R(qc_tilde)*w_d;

%d/dt(wrhat)=alpharhat
dw_r=R(qc_tilde)*dw_d-S(wc_tilde)*R(qc_tilde)*w_d-lambda*Q1(qc_tilde)*wc_tilde

%find rotation matrix
Rot = R(q_meas);

%parameterization matrix divided in four
%Y=[Yinertia1 Yinertia2 Ybuoyancy Ydrag]
Yinertia1=[dw_r(1) dw_r(2)-w_meas(1)*w_r(3) dw_r(3)+w_meas(1)*w_r(2);
           w_meas(1)*w_r(3) dw_r(1)+w_meas(2)*w_r(3) -w_meas(1)*w_r(1)+w_meas(3)*w_r(3);
           -w_meas(1)*w_r(2) w_meas(1)*w_r(1)-w_meas(2)*w_r(2) dw_r(1)-w_meas(3)*w_r(2)];
       
Yinertia2=[-w_meas(2)*w_r(3) w_meas(2)*w_r(2)-w_meas(3)*w_r(3) w_meas(3)*w_r(2);
           dw_r(2) dw_r(3)-w_meas(2)*w_r(1) -w_meas(3)*w_r(1);
           w_meas(2)*w_r(1) dw_r(2)+w_meas(3)*w_r(1) dw_r(3)];
       
Ybuoyancy=[0 -Rot(3,3) Rot(2,3);
           Rot(3,3) 0 -Rot(1,3);
           -Rot(2,3) Rot(1,3) 0];
       
Ydrag=[-w_meas(1)*abs(w_meas(1)) 0 0;
       0 -w_meas(2)*abs(w_meas(2)) 0;
       0 0 -w_meas(3)*abs(w_meas(3))];


Y=[Yinertia1 Yinertia2 Ybuoyancy Ydrag];

u=-Kd*shat+Y*ahat

dahat=-Gamma*Y'*shat

%% controller test case 2

clear

%% test case input

%desired
q_d=[0 0 sin(pi/4) cos(pi/4)]';
q_d=q_d/norm(q_d,2)
w_d=[0 0 0]';

%measured
q_meas=[0 0 0 1]';
w_meas=[0 0 0]';

%gains
lambda=1;
Gamma=1;
Kd=1;

%timing
dt=0.1;

%estimated parameters
ahat=[0.5 0 -0.1 1 0 1 0 0 0 1 2 2]';


%% actual controller

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
dw_r=R(qc_tilde)*dw_d-S(wc_tilde)*R(qc_tilde)*w_d-lambda*Q1(qc_tilde)*wc_tilde

%find rotation matrix
Rot = R(q_meas);

%parameterization matrix divided in four
%Y=[Yinertia1 Yinertia2 Ybuoyancy Ydrag]
Yinertia1=[dw_r(1) dw_r(2)-w_meas(1)*w_r(3) dw_r(3)+w_meas(1)*w_r(2);
           w_meas(1)*w_r(3) dw_r(1)+w_meas(2)*w_r(3) -w_meas(1)*w_r(1)+w_meas(3)*w_r(3);
           -w_meas(1)*w_r(2) w_meas(1)*w_r(1)-w_meas(2)*w_r(2) dw_r(1)-w_meas(3)*w_r(2)];
       
Yinertia2=[-w_meas(2)*w_r(3) w_meas(2)*w_r(2)-w_meas(3)*w_r(3) w_meas(3)*w_r(2);
           dw_r(2) dw_r(3)-w_meas(2)*w_r(1) -w_meas(3)*w_r(1);
           w_meas(2)*w_r(1) dw_r(2)+w_meas(3)*w_r(1) dw_r(3)];
       
Ybuoyancy=[0 -Rot(3,3) Rot(2,3);
           Rot(3,3) 0 -Rot(1,3);
           -Rot(2,3) Rot(1,3) 0];
       
Ydrag=[-w_meas(1)*abs(w_meas(1)) 0 0;
       0 -w_meas(2)*abs(w_meas(2)) 0;
       0 0 -w_meas(3)*abs(w_meas(3))];


Y=[Yinertia1 Yinertia2 Ybuoyancy Ydrag];

u=-Kd*shat+Y*ahat

dahat=-Gamma*Y'*shat