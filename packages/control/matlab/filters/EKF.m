% This is an Extended Kalman Filter
clc;
clear all;
close all;
offset = 1;

a = load('control.log');
depth_m = a(offset+4:end,8);
depth_a = 1/5*(a(offset:end-4,8)+a(offset+1:end-3,8)+a(offset+2:end-2,8)+a(offset+3:end-1,8)+a(offset+4:end,8));
depth_d = a(end,16);

y = depth_a;


m =28;
kd = 11.5;
buoy = .02; % Buoyant Force

% A,B,C,D as defined by the system
A = [0 1;0 -kd/m]; % k is our drag coeff.   m is mass
B = [0; 1/m];
C = [1, 0];
D = [0];

Ts = -1/1000 * (a(2,34) - a(3,34)); % This is our sampling time delay


[Ak Bk Ck Dk] = dssdata(c2d(ss(A,B,C,D),Ts)); % This discretizes our system
Ak_prev = Ak; % Our A matrix is LTI



% initial x estimate: [depth depth_dot]
x0 = [depth_a(1); 0];
x_prev = x0; 

% uorig = a(:,23);  %unused
% for i = 1:length(uorig)
%     u(i) = limitU(a(i,23));
% 
% end

u = -a(:,23);  %This is flipping our control signal because we use positive signal to dive


u_prev = u(1);
%this is just an example and we are constantly diving

Rv = 0.550; %This is the covariance of our process noise and is artbitrarily chosen
Rn = 1.1e-4; %This is the covariance of our sensor noise and is determined by finding the variance of the depth sensor readings for a constant depth

% initial Covariance:
P0 = [1e-4 0; 0 1];
nbar = 0; %assumed
vbar = 0; %assumed
I = eye(2,2); %always identity but only the size will change

P_prev = P0;


t_end = length(depth_a);

for t=1:t_end
    % Updating the predicted state and covariance forward in time
    x_pred = Ak*x_prev + Bk*u_prev + Bk*(-buoy);
    P_pred = Ak_prev*P_prev*Ak_prev' + Bk*Rv*Bk';   
    % Using the measured value along with the updated state
    K = P_pred*Ck'*inv(Ck*P_pred*Ck' + Rn);
    KalmanGains(:,t) = K;
    x(:,t) = x_pred + K*(y(t) - Ck*x_pred); % y will be from our log files of real dives
    P_prev = (I - K*Ck)*P_pred;
    P(:,t) = P_pred(:);%converts matrix into a vector
    x_prev = x(:,t);
    u_prev = u(t);
end

t=1:t_end;
t = t*Ts;
t2= 1:t_end+4;
t2 = t2*Ts;
% subplot(2,1,1)
hold on
plot(t,depth_a,'r');
plot(t,x(1,:),'b');
ylabel('Depth (ft)'); xlabel('Time (sec)');
legend('Measured Depth','Estimated Depth')
hold off
% subplot(2,1,2)
% hold on
% plot(t2,a(:,23),'r');
% plot(t2,u,'b');
% ylabel(''); xlabel('Time (sec)');
% legend('Original Control Signal','Limited Control Signal')
% hold off

figure;
subplot(2,1,1);
plot(t,KalmanGains(1,:))
ylabel('Kalman Gain X'); xlabel('Time (sec)');
subplot(2,1,2);
plot(t,KalmanGains(2,:))
ylabel('Kalman Gai X_dot'); xlabel('Time (sec)');