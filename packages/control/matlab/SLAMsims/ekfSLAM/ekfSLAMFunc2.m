function [Diverge] = ekfSLAMFunc2(xhat0, x0,plotData,Rv_cont,divergence_criteria)
        %xhat0 = initial state guess
        %x0 = true initial state
        %plot = boolean whether to plot
        %Rv_cont is our process noise    
%
% implementation of an EKF for SLAM navigation in a two 
% beacon environment
%
%
% written by Joseph Galante
% Robotics@Maryland

%clean up system

%system properties
c=4.5;%11.5;%robot drag (vehicle assumed to have symmetric drag)
m=28;%robot mass (in kg)


t_end = 200; % Seconds
Ts=1;%seconds
%k=1:floor(t_end/Ts);
time=Ts:Ts:Ts*floor(t_end/Ts);

%% dynamics for state ESTIMATES
% for xhatdot = A*xhat
% A=[0 0 1 0 0 0 0 0;
%    0 0 0 1 0 0 0 0;
%    0 0 -c/m 0 0 0 0 0;
%    0 0 0 -c/m 0 0 0 0;
%    0 0 0 0 -.000001 0 0 0;
%    0 0 0 0 0 -.000001 0 0;
%    0 0 0 0 0 0 -.000001 0;
%    0 0 0 0 0 0 0 -.000001];

A=[0 0 1 0 0 0 0 0;
   0 0 0 1 0 0 0 0;
   0 0 -c/m 0 0 0 0 0;
   0 0 0 -c/m 0 0 0 0;
   0 0 0 0 0 0 0 0;
   0 0 0 0 0 0 0 0;
   0 0 0 0 0 0 0 0;
   0 0 0 0 0 0 0 0];

% for xhatdot = A*xhat+B*random_forces
B=[0 0;
   0 0;
   1 0;
   0 1;
   0 0;
   0 0;
   0 0;
   0 0];


C = [0 0 0 0 0 0 0 0;0 0 0 0 0 0 0 0];
D = [0 0; 0 0];





%covariance of sensor noise
Rn=(pi/180)*diag([2 2]);%in radian

%[n,m]=size(A);
n=length(A);%SHOULD produce the # of columns
%discretize Rv
Gamma=[-A B*Rv_cont*B'; zeros(n) A'];
vanLoan=expm(Gamma*Ts);
F=vanLoan(n+1:end,n+1:end)';
Rv=F*vanLoan(1:n,n+1:end);

%find Bk 
[Ak Bk Ck Dk] = dssdata(c2d(ss(A,B,C,D),Ts));


%% dynamics for TRUE states
Areal=[0 0 1 0 0 0 0 0;
   0 0 0 1 0 0 0 0;
   0 0 -c/m 0 0 0 0 0;
   0 0 0 -c/m 0 0 0 0;
   0 0 0 0 0 0 0 0;
   0 0 0 0 0 0 0 0;
   0 0 0 0 0 0 0 0;
   0 0 0 0 0 0 0 0];

%use same B, C, and D as estimated dynamics

%find Ak_real and Bk_real 
[Ak_real Bk_real Ck_real Dk_real] = dssdata(c2d(ss(Areal,B,C,D),Ts));



%% Initialize parameters
P0 = diag([2 2 .5 .5 4 4 4 4]); % Initial Covariance Matrix
Ak_prev = Ak; % "A" matrix is LTI so Ak_prev and Ak wont change


  
I = eye(size(P0));
xhat_prev = xhat0;
xhat = xhat0;
x_prev = x0;
x = x0;
P_prev = P0;

%storage variables
P=zeros(n^2,floor(t_end/Ts));

%weighting matrices to generate correct covariances from
%N(0,1) independent gaussian random variables
Cv=[chol(Rv(1:4,1:4))' zeros(4,4); zeros(4,4) zeros(4,4)];
Cn=chol(Rn)';

%% run simulation
for k=2:floor(t_end/Ts)
    %%%%%%%%%%%%% simulate the real world
    %simulate process noise
    noise_process=Cv*randn(n,1);
    x(:,k)=Ak_real*x_prev+noise_process;
    
    
    %%%%%%%%%%%%% simulate measurement
    noise_sensor=Cn*randn(2,1);
    %what the measurements would be without noise
    %angle to pinger 1
    %theta1=atan2(y1-y,x1-x)
    theta1=atan2(x(5,k)-x(1,k),x(6,k)-x(2,k));
    %angle to pinger 2
    %theta2=atan2(y2-y,x2-x)
    theta2=atan2(x(7,k)-x(1,k),x(8,k)-x(2,k));
    y(:,k)=[theta1; theta2]+noise_sensor;
    %y(:,k)=[theta1; theta2];
    
    %%%%%%%%%%%%% run EKF
    %predict step
    %predict state forward
    xhat_pred=Ak*xhat_prev; %need to add control forces here
    %predict decrease in certainty in state estimates
    P_pred=Ak*P_prev*Ak'+Rv;%this is correct, don't need Bk*Rv*Bk'
    %
    %update step
    %compute C matrix (measurement model)
    Citer=iterateMeasModel(x(:,k));
    %compute kalman gain
    K=P_pred*Citer'*inv(Citer*P_pred*Citer'+Rn);%kalman gain
    %K=P_pred*C'*inv(C*P_pred*C')%kalman gain
    %state update
    y1est=atan2(xhat_pred(5)-xhat_pred(1),xhat_pred(6)-xhat_pred(2));
    y2est=atan2(xhat_pred(7)-xhat_pred(1),xhat_pred(8)-xhat_pred(2));
    xhat(:,k)=xhat_pred+K*(y(:,k)-[y1est y2est ]');
    P_t=(I-K*Citer)*P_pred;
    
    %%%%%%%%%%%%% bookkeeping
    P(:,k)=P_t(:);
    x_prev=x(:,k);
    xhat_prev=xhat(:,k);
    P_prev=P_t;
end

% Check if ekf Diverged
if ((abs(xhat(1,end)-x(1,end)) > divergence_criteria)||(abs(xhat(2,end)-x(2,end)) > divergence_criteria))
    Diverge = 1;
else
    Diverge = 0;
end
if (~isfinite(xhat(1,end)) || ~isfinite(xhat(2,end)))
    Diverge = 1;
end

if plotData ==1
    %position plot
    figure('name','Simulation Map')
    %plot(x(1,:),x(2,:),xhat(1,:),xhat(2,:))
    plot(x(1,:),x(2,:),'-',xhat(1,:),xhat(2,:),'-',x0(5),x0(6),'b*',x0(7),x0(8),'r*')
    legend('x','xhat','Pinger 1','Pinger 2')
    xlabel('x')
    ylabel('y')


    %estimates and true states of robot
    figure('name','Robot States')
    subplot(4,1,1)
    plot(time,x(1,:),time,xhat(1,:),time,xhat(1,:)+sqrt(P(1,:)),'r-.',time,xhat(1,:)-sqrt(P(1,:)),'r-.')
    ylabel('x')
    legend('true','estimated','\sigma')
    subplot(4,1,2)
    plot(time,x(2,:),time,xhat(2,:),time,xhat(2,:)+sqrt(P(10,:)),'r-.',time,xhat(2,:)-sqrt(P(10,:)),'r-.')
    ylabel('y')
    subplot(4,1,3)
    plot(time,x(3,:),time,xhat(3,:),time,xhat(3,:)+sqrt(P(19,:)),'r-.',time,xhat(3,:)-sqrt(P(19,:)),'r-.')
    ylabel('xdot')
    subplot(4,1,4)
    plot(time,x(4,:),time,xhat(4,:),time,xhat(4,:)+sqrt(P(28,:)),'r-.',time,xhat(4,:)-sqrt(P(28,:)),'r-.')
    ylabel('ydot')

    % error in xhat
    % figure('name','xHat Error')
    % subplot(4,1,1)
    % plot(time,P(1,:))
    % subplot(4,1,2)
    % plot(time,P(10,:))
    % subplot(4,1,3)
    % plot(time,P(19,:))
    % subplot(4,1,4)
    % plot(time,P(28,:))

    %estimates and true states of pinger locations
    figure('name','Pinger States')
    subplot(4,1,1)
    plot(time,x(5,:),time,xhat(5,:),time,xhat(5,:)+sqrt(P(37,:)),'r-.',time,xhat(5,:)-sqrt(P(37,:)),'r-.')
    ylabel('x1')
    legend('true','estimated')
    subplot(4,1,2)
    plot(time,x(6,:),time,xhat(6,:),time,xhat(6,:)+sqrt(P(46,:)),'r-.',time,xhat(6,:)-sqrt(P(46,:)),'r-.')
    ylabel('y1')
    subplot(4,1,3)
    plot(time,x(7,:),time,xhat(7,:),time,xhat(7,:)+sqrt(P(55,:)),'r-.',time,xhat(7,:)-sqrt(P(55,:)),'r-.')
    ylabel('x2')
    subplot(4,1,4)
    plot(time,x(8,:),time,xhat(8,:),time,xhat(8,:)+sqrt(P(64,:)),'r-.',time,xhat(8,:)-sqrt(P(64,:)),'r-.')
    ylabel('y2')

    %measurement plot
    figure('name','Measurements')
    subplot(2,1,1)
    plot(time,y(1,:))
    ylabel('\theta_1')
    subplot(2,1,2)
    plot(time,y(2,:))
    ylabel('\theta_2')
end

% for t=1:t_end
%     % Updating the predicted state and covariance forward in time
%    x_pred = Ak*x_prev; %WE WILL NEED TO ADD FORCES
%     P_pred = Ak_prev*P_prev*Ak_prev' + ;   
%     % Using the measured value along with the updated state
%     K = P_pred*Ck'*inv(Ck*P_pred*Ck' + Rn);
%     KalmanGains(:,t) = K;
%     x(:,t) = x_pred + K*(y(t) - Ck*x_pred); % y will be from our log files of real dives
%     P_prev = (I - K*Ck)*P_pred;
%     P(:,t) = P_pred(:);%converts matrix into a vector
%     x_prev = x(:,t);
%     u_prev = u(t);
% end
