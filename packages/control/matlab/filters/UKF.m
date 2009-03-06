% This is an Unscented Kalman Filter for a system with 2 states

% initial x estimate: [depth depth_dot]
x0 = [0 0];

% initial Covariance:
% P0 = [1 0; 0 1];
P0 = [1 0];

% process noise covariance
Rv = [1 0; 0 1];

% measurement noise covariance
Rn = [1 1; 0 1];

% alpha: spread of sigma points
alpha = 10^-2;

% beta: prior knowledge about the distribution of x
beta = 2; %optimal for gaussian

t_end = 100;

x_prev = x0;
P_prev = P0;
L = 2; % state dimension
lambda = 1;%?????????
gamma = sqrt(L+lambda);

for t = 1:t_end
    
    % calculate sigma points
    sigma_prev = [x_prev (x_prev + gamma*sqrt(P_prev)) (x_prev - gamma*sqrt(P_prev))];
    
    % update sigma points
    sigma_curr_prev = F(sigma_prev,u_prev);
    if (t==1)
        sigma_curr_prev_0 = sigma_curr_prev;
    end
    
    % update predicted state estimates
    x_pred = lambda/(L+lambda)*sigma_curr_prev(:,1);
    for i = 2:(2*L+1)
        x_pred = x_pred + 1/(2*(L+lambda))*sigma_curr_prev(:,i);
    end
    
    % update predicted covariance estimate
    P_pred = (lambda/(L+lambda)+1-alpha^2+beta)*(sigma_curr_prev(:,1)-x_pred)*(sigma_curr_prev(:,1)-x_pred)'+Rv;
    for i = 2:(2*L+1)
        P_pred = P_pred + 1/(2*(L+lambda))*(sigma_curr_prev(:,i)-x_pred)*(sigma_curr_prev(:,i)-x_pred)'+Rv;
    end
    
    % augment sigma points
    sigma_curr_prev_aug = [sigma_curr_prev sigma_curr_prev_0+gamma*sqrt(Rv) sigma_curr_prev_0 - gamma*sqrt(Rv)];
    
    % calculating measured sigma points
    sigma_meas = H(sigma_curr_prev_aug);
    
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
    Pxy = (lambda/(L+lambda)+1-alpha^2+beta)*(sigma_curr_prev_aug(:,1) - y_pred)*(sigma_curr_prev_aug(:,1) - y_pred)';
    for i = 2:(2*L+1)
        Pxy = Pxy + 1/(2*(L+lambda))*(sigma_curr_prev_aug(:,i) - y_pred)*(sigma_curr_prev_aug(:,i) - y_pred)';
    end
    
    % update kalman gain
    K = Pxy*inv(Pyy);
    
    % calculate final state estimate: meas & pred
    x = x_pred + K*(y-y_pred);
    
    % update covariance
    P = P_pred - K*Pyy*K';
    
    x_prev = x;
    P_prev = P;
end