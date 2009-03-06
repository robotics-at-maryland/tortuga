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


m = 20;
kd = 11.5;

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

uorig = a(:,23);  %unused
for i = 1:length(uorig)
    u(i) = limitU(a(i,23));

end

u_prev = u(1);
%this is just an example and we are constantly diving

Rv = .10; %This is the covariance of our noise matrices and is artbitrarily chosen
Rn = .10; %This is the covariance of our noise matrices and is artbitrarily chosen

% initial Covariance:
%P0 = [1.7 .5; 1.7 .5];
P0 = [1 0; 0 1];
nbar = 0; %assumed
vbar = 0; %assumed
I = eye(2,2); %always identity but only the size will change

P_prev = P0;


t_end = 2500;
buoy = .02; % Buoyant Force

for t=1:t_end
    % Updating the predicted state and covariance forward in time
    x_pred = Ak*x_prev + Bk*u_prev + [0;buoy*Ts];
    P_pred = Ak_prev*P_prev*Ak_prev' + Bk*Rv*Bk'; 
    % Using the measured value along with the updated state
    K = P_pred*Ck'*inv(Ck*P_pred*Ck' + Rn');
    Blah(:,t) = K;
    x(:,t) = x_pred + K*(y(t) - Ck*x_pred); % y will be from our log files of real dives There may be a noise term added on here nex to Ck*xpred
    P_prev = (I - K*Ck)*P_pred;
    x_prev = x(:,t);
    u_prev = u(t);
end

t=1:t_end;
hold on
plot(depth_a,'r');
plot(x(1,t)),'y';
%axis([0 2500 -10 15]);
x;
hold off;

figure;
subplot(2,1,1);
plot(t,Blah(1,:))
ylabel('blah1')
subplot(2,1,2);
plot(t,Blah(2,:))
ylabel('blah2')