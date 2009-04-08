%
%
% implementation of an EKF for SLAM navigation in a two 
% beacon environment
%
%
% written by Joseph Galante
% Robotics@Maryland

%clean up system
clc
clear
close all

%system properties
c=11.5;%robot drag (vehicle assumed to have symmetric drag)
m=28;%robot mass (in kg)

A=[0 0 1 0 0 0 0 0;
   0 0 0 1 0 0 0 0;
   0 0 -c/m 0 0 0 0 0;
   0 0 0 -c/m 0 0 0 0;
   0 0 0 0 -.1 0 0 0;
   0 0 0 0 0 -.1 0 0;
   0 0 0 0 0 0 -.1 0;
   0 0 0 0 0 0 0 -.1];

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

Ts=1;%seconds

%covariance of process noise (arbitrarily chosen) 
Rv_cont=diag([8 8]);%in N   CONTINUOUS TIME, needs discretization
%covariance of sensor noise
Rn=(180/pi)*[2 2];%in radian

%[n,m]=size(A);
n=length(A);%SHOULD produce the # of columns
%discretize Rv
Gamma=[-A B*Rv_cont*B'; zeros(n) A'];
vanLoan=expm(Gamma*Ts);
F=vanLoan(n+1:end,n+1:end)';
Rv=F*vanLoan(1:n,n+1:end);

%find Bk 
[Ak Bk Ck Dk] = dssdata(c2d(ss(A,B,C,D),Ts));

%Initializes parameters
P0 = diag([2 2 .5 .5 4 4 4 4]); % Initial Covariance Matrix
Ak_prev = Ak; % "A" matrix is LTI so Ak_prev and Ak wont change
x0 = [ 0;  % x       Initial Estimate
       0;  % y            
       0;  % x_dot
       0;  % y_dot
       -50;  % x1  (Pinger 1)
       100;  % y1  (Pinger 1)
       50;   % x2 (Pinger 2)
       100;];% y2 (Pinger 2)

I = eye(size(P0));
x_prev = x0;
P_prev = P0;
t_end = 1000; % Seconds

% for t=1:t_end
%     % Updating the predicted state and covariance forward in time
%     x_pred = Ak*x_prev; %WE WILL NEED TO ADD FORCES
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
