% This is an Extended Kalman Filter Written by Joseph Galante, Alex Janas,
% and Jaymit Patel on 3/24/2009
clc;
clear all;
close all;



% Loads the logged depth file:
%      depth_m = measured depth
%      depth_a = 5 pt averaged depth = y
%      depth_d = desired depth
a = load('control.log');
depth_m = a(5:end,8);
depth_a = 1/5*(a(1:end-4,8)+a(2:end-3,8)+a(3:end-2,8)+a(4:end-1,8)+a(5:end,8));
depth_d = a(end,16);
y = depth_a;
u = -a(:,23);  %Control Signal values (flipped)




%Constants
m =28;      % vehicle mass (kg)
kd = 11.5;  % drag coefficient
buoy = .02; % Buoyant Force
Ts = -1/1000 * (a(2,34) - a(3,34)); % Sampling time delay
Rv = 0.550;  % Covariance of process noise: artbitrarily chosen
Rn = 1.1e-4; % Covariance of sensor noise: determined by finding the variance of the depth sensor readings for a constant depth
nbar = 0; %assumed
vbar = 0; %assumed


% A,B,C,D as defined by the system equations
A = [0 1;0 -kd/m];
B = [0; 1/m];
C = [1, 0];
D = [0];
[Ak Bk Ck Dk] = dssdata(c2d(ss(A,B,C,D),Ts)); % Discretizes system


%discretize Rn
Gamma=[-A B*Rv*B'; zeros(2) A'];
vanLoan=expm(Gamma*Ts);
F=vanLoan(3:4,3:4)';
Q=F*vanLoan(1:2,3:4);



%Initializes parameters
P0 = [1e-4 0; 0 1]; % Initial Covariance Matrix
Ak_prev = Ak; % "A" matrix is LTI so Ak_prev and Ak wont change
x0 = [depth_a(1); 0]; % initial x estimate: [depth depth_dot]






% Thruster Limiting Code (Wont be needed once our log files provide limited values)
 uorig = a(:,23);  %unused
 for i = 1:length(uorig)
     u(i) = limitU(a(i,23));
 
 end




I = eye(size(P0));
x_prev = x0;
P_prev = P0;
u_prev = u(1);
t_end = length(depth_a);



for t=1:t_end
    % Updating the predicted state and covariance forward in time
    x_pred = Ak*x_prev + Bk*u_prev + Bk*(-buoy);
    P_pred = Ak_prev*P_prev*Ak_prev' + Q;   
    % Using the measured value along with the updated state
    K = P_pred*Ck'*inv(Ck*P_pred*Ck' + Rn);
    KalmanGains(:,t) = K;
    x(:,t) = x_pred + K*(y(t) - Ck*x_pred); % y will be from our log files of real dives
    P_prev = (I - K*Ck)*P_pred;
    P(:,t) = P_pred(:);%converts matrix into a vector
    x_prev = x(:,t);
    u_prev = u(t);
end


% Plotting:  Figure 1 Measured and Estimated Depth
%            Figure 2 Kalman Gain

t=1:t_end;
t = t*Ts;
t2= 1:t_end+4;
t2 = t2*Ts;

hold on
plot(t,depth_a,'r');
plot(t,x(1,:),'b');
ylabel('Depth (ft)'); xlabel('Time (sec)');
legend('Measured Depth','Estimated Depth')
hold off

figure;
subplot(2,1,1);
plot(t,KalmanGains(1,:))
ylabel('Kalman Gain X'); xlabel('Time (sec)');
subplot(2,1,2);
plot(t,KalmanGains(2,:))
ylabel('Kalman Gai X_dot'); xlabel('Time (sec)');


% OPTIONAL CONTROL SIGNAL PLOTTING
% hold on
% plot(t2,a(:,23),'r');
% plot(t2,u,'b');
% ylabel(''); xlabel('Time (sec)');
% legend('Original Control Signal','Limited Control Signal')
% hold off