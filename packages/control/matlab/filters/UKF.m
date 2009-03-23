% This is an Unscented Kalman Filter for a system with 2 states
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

u = -a(:,23);  %This is flipping our control signal because we use positive signal to 
u_prev = u(1); 


% initial Covariance:
P0 = [1e-4 0; 0 1];
P_prev = P0;

% process noise covariance
Rv = [1 0; 0 1]; %This is the covariance of our process noise and is artbitrarily chosen
% Rn = [1 1; 0 1]; %This is the covariance of our sensor noise and is determined by finding the variance of the depth sensor readings for a constant depth

% Rv = 0.550; 
Rn = 1.1e-4; 

% alpha: spread of sigma points
alpha = 10^-2;
% beta: prior knowledge about the distribution of x
beta = 2; %optimal for gaussian

t_end = length(depth_a);


L = 2; % state dimension only 2 for this example
K = 3-L;
lambda = alpha^2*(L+K) - L; %?????????
gamma = sqrt(L+lambda);

for t = 1:t_end
    % calculate sigma points and then time update them (See page 228 unscented transformation)
    sigma(:,1) = x_prev;
    temp1 = sqrt( (L+lambda)*P_prev);
    sigma_predict(:,1) = Ak*sigma(:,1) + Bk*u_prev + Bk*(-buoy);

    for q=2:2*L+1
        if q <= L+1
            sigma(:,q) = x_prev + temp1(:,q-1);
        else
            sigma(:,q) = x_prev - temp1(:,q-(L+1));
        end

        sigma_predict(:,q) = Ak*sigma(:,q) + Bk*u_prev + Bk*(-buoy); %Check on Bk stuff to make sure we right
    end

sigma;
sigma_predict;


    %%%%%%%%%%%% update predicted state estimates %%%
    x_pred = lambda/(L+lambda)*sigma_predict(:,1);
    for i = 2:(2*L+1)
        x_pred = x_pred + 1/(2*(L+lambda))*sigma_predict(:,i);
    end
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    
    
    %%%%%%%%% update predicted covariance estimate
    P_pred = (lambda/(L+lambda)+1-alpha^2+beta)*(sigma_predict(:,1)-x_pred)*(sigma_predict(:,1)-x_pred)'+Rv;
    for i = 2:(2*L+1)
        P_pred = P_pred + 1/(2*(L+lambda))*(sigma_predict(:,i)-x_pred)*(sigma_predict(:,i)-x_pred)'+Rv;
    end
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    
    %%%%%%%%%%%%% augment sigma points then use for Sigma measured
        % Note we are using the "alternate method on pg 233 bottom
    sigma_pred_aug(:,1) = x_pred;
    temp1 = sqrt( (L+lambda)*P_pred);
    sigma_pred_aug(:,1) = Ak*sigma_pred_aug(:,1) + Bk*u_prev + Bk*(-buoy);

    for q=2:2*L+1
        if q <= L+1
            sigma_pred_aug(:,q) = x_pred + temp1(:,q-1);
        else
            sigma_pred_aug(:,q) = x_pred - temp1(:,q-(L+1));
        end
        sigma_meas(:,q) = Ck*sigma_pred_aug(:,q); %Dk should be here but its 0
    end
    
    %%%%%%%%%%%%%%%%%%%%
    

    % update predicted measured state
    y_pred = lambda/(L+lambda)*sigma_meas(:,1);
    for i = 2:(2*L+1)
        y_pred = y_pred + 1/(2*(L+lambda))*sigma_meas(:,i);
    end

    % calculate Pyy
    Pyy = (lambda/(L+lambda)+1-alpha^2+beta)*(sigma_meas(:,1) - y_pred)*(sigma_meas(:,1) - y_pred)'+Rn;
    for i = 2:(2*L+1)
        Pyy = Pyy + 1/(2*(L+lambda))*(sigma_meas(:,i) - y_pred)*(sigma_meas(:,i) - y_pred)'+Rn;
    end

    % calculate Pxy
    Pxy = (lambda/(L+lambda)+1-alpha^2+beta)*(sigma_pred_aug(:,1) - x_pred)*(sigma_meas(:,1) - y_pred)';
    for i = 2:(2*L+1)
        Pxy = Pxy + 1/(2*(L+lambda))*(sigma_pred_aug(:,i) - x_pred)*(sigma_meas(:,i) - y_pred)';
    end

    % update kalman gain
    K = Pxy*(Pyy^-1);
    
    % calculate final state estimate: meas & pred
    x(:,t) = x_pred + K*(y(t)-y_pred);

    % update covariance
    P = P_pred - K*Pyy*K';

    x_prev = x(:,t);
    P_prev = P;
end


t=1:t_end;
t = t*Ts;
t2= 1:t_end+4;
t2 = t2*Ts;
% subplot(2,1,1)
% hold on
% plot(t,depth_a,'r');
% plot(t,x(1,:),'b');
% ylabel('Depth (ft)'); xlabel('Time (sec)');
% legend('Measured Depth','Estimated Depth')
% hold off
% 
% figure;
% subplot(2,1,1);
% plot(t,KalmanGains(1,:))
% ylabel('Kalman Gain X'); xlabel('Time (sec)');
% subplot(2,1,2);
% plot(t,KalmanGains(2,:))
% ylabel('Kalman Gai X_dot'); xlabel('Time (sec)');