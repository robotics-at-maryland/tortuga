% simulate tortuga rotational dynamics
clc
close all;
clear;

%% initialization
%sampling frequency
frequency = 40;
%initial position
axis0=[0 0 0]';
angle0=0*pi/180;%altered from 180
q0=[axis0*sin(angle0/2); cos(angle0/2)];
%q0=[1 0 0 0]';
q_old = q0;
%initial angular rate
w0=(pi/180)*[0 0 0]';
%initial desired position
qd0=[0 0 0.7071 0.7071]';
%initial desired angular rate
wd0=(pi/180)*[0 0 0]';
w_old = 0;
%initial estimated position
qhat0=q0;
%initial estimated angular rate
%what0=w0;

%initial parameter estimate
%ahat=[h11 h12 h13 h22 h23 h33 rb1*fb rb2*fb rb3*fb cd1 cd2 cd3]
%h - elements of inertial matrix
%rb - vector from center of gravity to center of buoyancy
%fb - buoyant force
%cd - drag parameters (model is c*w*abs(w))
%      h                rb    c
ahat0=[0.5 0 -0.1 1 0 1 0 0 0 1 2 2]';

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
Kd=1;%typically choose Kd=1*eye(3)
lambda=1;%typically choose lambda=1
Gamma=1;

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

% u is only constant when ode45 is running
global u;
%% timing (in seconds)
t0=0;
te=50;
step = .3;
time = t0:step:te;


% simulation
%x = [q ;  w;  qd;  wd;  qhat; ahat];
x=zeros(length(time),30);
x(1,:) = x0';

%options=odeset;
%options=odeset(options,'AbsTol',1e-3,'MaxStep',0.05);
%[time,x] = ode45(@rotationalSimDynamics,[t0,te],x0);
%[time,x] = ode45(@rotationalSimDynamics,[t0,te],x0,options);
for i = 2:2%length(time)
    
    % unpack data
    x_curr = x(i-1,:)';
    q=x_curr(1:4);
    w=x_curr(5:7);
    q_d=x_curr(8:11);
    w_d=x_curr(12:14);
    qhat=x_curr(15:18);
    ahat=x_curr(19:30);
    %what=x(19:21);

    %fix numerical quaternion drift
    q=q/norm(q,2);
    q_d=q_d/norm(q_d,2);

    % measurement
    Rot = R(q);
    %a_meas = Rot * a_inertial + 0.0005*randn;
    %m_meas = Rot * m_inertial + 0.0005*randn;
    a_meas = Rot * a_inertial;
    m_meas = Rot * m_inertial;
    w_meas=w;

    % estimation

    q_meas = quaternionFromnCb(nCbFromIMU(m_meas,a_meas));
    %q_meas=q;

    %quaternion estimation that requires only angular rate gyro
    dqhat = (1/2)*Q(qhat)*w_meas;
    qhat = qhat + dqhat*step;
    %dwhat =

    % controller

    % error metrics
    
    %propagate desired states
    %desire constant angular rate for now
    dw_d=zeros(3,1);
    w_d = w_d + dw_d*step;
    %desired angular position varies
    dq_d=(1/2)*Q(q_d)*w_d;
    q_d = q_d + dq_d*step;

    %compute attitude error qc_tilde (controller quaternion)
    qc_tilde=quaternionProduct(q_meas,q_d);

    %compute composite error metric s
    w_r=R(qc_tilde)*w_d-lambda*qc_tilde(1:3)*qc_tilde(4);%altered
    shat=w_meas-w_r;

    %compute angular rate error wc_tilde
    wc_tilde=w_meas-R(qc_tilde)*w_d;

    %d/dt(wrhat)=alpharhat
    dw_r=R(qc_tilde)*dw_d-lambda*Q1(qc_tilde)*wc_tilde;%S(wc_tilde)*R(qc_tilde)*w_d-
    
    
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

    %adaptation law
    
    dahat=-Gamma*Y'*shat;
    
    ahat = ahat + dahat*step;
    
    %hacky parametric dead zone
    
    if(ahat(1)>2)
        ahat(1)=2;
    end
    if(ahat(1)<-2)
        ahat(1)=-2;
    end
    if(ahat(2)>1)
        ahat(2)=1;
    end
    if(ahat(2)<-1)
        ahat(2)=-1;
    end
    if(ahat(3)>0.5)
        ahat(3)=0.5;
    end
    if(ahat(3)<-0.5)
        ahat(3)=-0.5;
    end
    if(ahat(4)>2)
        ahat(4)=2;
    end
    if(ahat(4)<-2)
        ahat(4)=-2;
    end
    if(ahat(5)>1)
        ahat(5)=1;
    end
    if(ahat(5)<-1)
        ahat(5)=-1;
    end
    if(ahat(6)>2)
        ahat(6)=2;
    end
    if(ahat(6)<-2)
        ahat(6)=-2;
    end
    if(ahat(7)>1)
        ahat(7)=1;
    end
    if(ahat(7)<-1)
        ahat(7)=-1;
    end
    if(ahat(8)>1)
        ahat(8)=1;
    end
    if(ahat(8)<-1)
        ahat(8)=-1;
    end
    if(ahat(9)>1)
        ahat(9)=1;
    end
    if(ahat(9)<-1)
        ahat(9)=-1;
    end
    if(ahat(10)>5)%should be 5
        ahat(10)=5;
    end
    if(ahat(10)<4)%should be 0
        ahat(10)=4;
    end
    if(ahat(11)>4)
        ahat(11)=4;
    end
    if(ahat(11)<0)
        ahat(11)=0;
    end
    if(ahat(12)>5)
        ahat(12)=5;
    end
    if(ahat(12)<0)
        ahat(12)=0;
    end
    
    
    %control law
    
    u=-Kd*shat+Y*ahat;
    
    options=odeset;
    options=odeset(options,'AbsTol',1e-3,'MaxStep',0.05);
    [time_ode,x_ode] = ode45(@rotationalSimDynamics,[time(i-1) time(i)],x(i-1,1:7));
    x(i,1:7) = x_ode(end,:);
    x(i,8:11) = q_d;
    x(i,12:14) = w_d;
    x(i,15:18) = qhat;
    x(i,19:30) = ahat;
end
%% format results
 %recall that x = [q; w; qhat; bhat];
%grab results
q=x(:,1:4);%actual angular position
w=x(:,5:7);%actual angular rate
qd=x(:,8:11);%desired angular position
wd=x(:,12:14);%desired angular rate
qhat=x(:,15:18);%estimated angular position
%what=x(:,19:21);%estimated angular rate
ahat=x(:,19:30);

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
% %final heading of vehicle
% R(q(end,:))'*[1 0 0]'

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
plot(time,Cd(1,1)*ones(length(time),1),time,ahat(:,10),'linewidth',4)
ylabel('C_d_1')
subplot(3,1,2)
plot(time,Cd(2,2)*ones(length(time),1),time,ahat(:,11),'linewidth',4)
ylabel('C_d_2')
subplot(3,1,3)
plot(time,Cd(3,3)*ones(length(time),1),time,ahat(:,12),'linewidth',4)
ylabel('C_d_3')
xlabel('time (s)')
