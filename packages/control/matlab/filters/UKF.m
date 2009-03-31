% This is an Extended Kalman Filter Written by Joseph Galante, Alex Janas,
% and Jaymit Patel on 3/24/2009
clc;
clear all;
close all;
print_out = 1; %This is to print our everything

% Loads the logged depth file:
%      depth_m = measured depth
%      depth_a = 5 pt averaged depth = y
%      depth_d = desired depth
a = load('control.log');
depth_m = a(54:end,8);
depth_a = 1/5*(a(1:end-4,8)+a(2:end-3,8)+a(3:end-2,8)+a(4:end-1,8)+a(5:end,8));
depth_d = a(end,16);
y = depth_a;
u = -a(:,23);  %Control Signal values (flipped)





% Constants
m =28;      % Vehicle Mass (kg)
kd = 11.5;  % Drag Coefficient
buoy = .02; % Buoyant Force
Ts = 1/1000 * (a(3,34) - a(2,34)); % This is our sampling time delay
alpha = 10^-2; % Spread of sigma points
beta = 2; %Prior knowledge about distribution of x: Optimal for gaussian
      %Covariance of sensor noise: Determined by finding variance of the depth sensor readings for a constant depth 
      Rv = 0.550; % Arbitrarily Chosen                 
      Rn = 1.1e-4;  % Covariance of process noise: Artbitrarily chosen
      
              

      
      
% A,B,C,D as defined by the system
A = [0 1;0 -kd/m];
B = [0; 1/m];
C = [1, 0];
D = [0];
[Ak Bk Ck Dk] = dssdata(c2d(ss(A,B,C,D),Ts)); % Discretizes system





% Initialize Parameters
P0 = [1e-4 0; 0 1]; % Initial Covariance Matrix
Ak_prev = Ak; % Our A matrix is LTI
x0 = [depth_a(1); 0]; % Initial x estimate: [depth depth_dot]
u_prev = u(1);
x_prev = x0;
P_prev = P0;




% UKF Specific Parameters
L = 2; % State dimension: 2 for this example [depth; DepthDot]
K = 3-L;
lambda = alpha^2*(L+K) - L; % This formula was listed but should we use it?
gamma = sqrt(L+lambda);





t_end = length(depth_a);
% for t = 1:t_end
for t = 1:t_end
    % calculate sigma points and then time update them (See page 228 unscented transformation)
    sigma(:,1) = x_prev;
    temp1 = chol((L+lambda)*P_prev );
    sigma_predict(:,1) = Ak*sigma(:,1) + Bk*u_prev + Bk*(-buoy);

    for q=2:2*L+1
        if q <= L+1
            sigma(:,q) = x_prev + temp1(:,q-1);
        else
            sigma(:,q) = x_prev - temp1(:,q-(L+1));
        end

        sigma_predict(:,q) = Ak*sigma(:,q) + Bk*u_prev + Bk*(-buoy); %Check on Bk stuff to make sure we right
    end



    %%%%%%%%%%%% update predicted state estimates %%%
    x_pred = lambda/(L+lambda)*sigma_predict(:,1);
    for i = 2:(2*L+1)
        x_pred = x_pred + 1/(2*(L+lambda))*sigma_predict(:,i);
    end
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



    %%%%%%%%% update predicted covariance estimate
    P_pred = (lambda/(L+lambda)+1-alpha^2+beta)*(sigma_predict(:,1)-x_pred)*(sigma_predict(:,1)-x_pred)'+ Bk*Rv*Bk';
    for i = 2:(2*L+1)
        P_pred = P_pred + (1/(2*(L+lambda)))*(sigma_predict(:,i)-x_pred)*(sigma_predict(:,i)-x_pred)'+ Bk*Rv*Bk';
    end
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



    %%%%%%%%%%%%% augment sigma points then use for Sigma measured
    % Note we are using the "alternate method on pg 233 bottom
    sigma_pred_aug(:,1) = x_pred;
    temp1 = chol( (L+lambda)*P_pred);
    sigma_meas(:,1) = Ck*sigma_pred_aug(:,1);

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
        Pyy = Pyy + 1/(2*(L+lambda))*(sigma_meas(:,i) - y_pred)*(sigma_meas(:,i) - y_pred)'+ Rn;
    end

    % calculate Pxy
    Pxy = (lambda/(L+lambda)+1-alpha^2+beta)*(sigma_pred_aug(:,1) - x_pred)*(sigma_meas(:,1) - y_pred)';
    for i = 2:(2*L+1)
        Pxy = Pxy + 1/(2*(L+lambda))*(sigma_pred_aug(:,i) - x_pred)*(sigma_meas(:,i) - y_pred)';
    end

    % update kalman gain
    K = Pxy*(Pyy^-1);



    %%%%%% PRINTOUT BECAUSE DEBUG ON MAC IS NOT FUNCTIONAL
    if print_out ==1
        sigma
        sigma_predict
        x_pred
        P_pred
        sigma_pred_aug
        sigma_meas
        y_pred
        Pyy
        Pxy
    end
    %%%%%%%%%%%%%%%




    % calculate final state estimate: meas & pred
    x(:,t) = x_pred + K*(y(t+1)-y_pred);

    % update covariance
    P = P_pred - K*Pyy*K';

    x_prev = x(:,t);
    P_prev = P;

    if print_out ==1
        x(:,t)
        x_prev
        P_prev
    end
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

% figure;
% subplot(2,1,1);
% plot(t,KalmanGains(1,:))
% ylabel('Kalman Gain X'); xlabel('Time (sec)');
% subplot(2,1,2);
% plot(t,KalmanGains(2,:))
% ylabel('Kalman Gai X_dot'); xlabel('Time (sec)');