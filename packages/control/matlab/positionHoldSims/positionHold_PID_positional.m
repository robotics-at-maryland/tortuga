function [ Fb T ] = positionHold_PID_positional( state_b, k, state_desired)
% positionHold_PID_positional
%
% Jonathan Wonders 2010-2-28
%
%   state_b = [ x_1_dot   ]
%             [ x_2_dot   ]
%             [ theta_dot ]
%
%   k = [ kp  kd  ki ] <-- x_1
%       [ kp  kd  ki ] <-- x_2
%       [ kp  kd  ki ] <-- theta
%
%
%   state_desired = [ x_1       ]
%                   [ x_1_dot   ]
%                   [ x_2       ]
%                   [ x_2_dot   ]
%                   [ theta     ]
%                   [ theta_dot ]
%

% Store controllers position and orientation estimates for plotting
global position_storage;
global orientation_storage;

% Pull in global time, time step, and initial conditions
global t_step;
global current_time;
global x0;

% Store history of inputs to allow for different types of numerical
% integration
persistent input_storage;
persistent state_storage;

% Store proportional, derivitive, and integral error terms
persistent error_storage;

persistent currentPosition_n;
persistent currentOrientation;

% index of current state in INPUT_STORAGE
n = uint32(current_time/t_step); 

% Initialize currentPosition for time = 0
if n == 1
    currentPosition_n = [x0(1) x0(2)]';
    currentOrientation = x0(5);
end

% Convert input to inertial frame
state_n = bRn(currentOrientation)'*state_b(1:2);
input_storage(1:2,n) = state_n;

if n > 1
    % Numerically integrate velocities in inertial frame to get position
    % estimate in inertial frame
    currentPosition_n = currentPosition_n + t_step*(state_n(1:2) + input_storage(1:2,n-1))/2;
    %currentPosition_n = currentPosition_n + state_n(1:2)*t_step;
    % Numerically integrate rotational speed to get orientation estimate
    currentOrientation = currentOrientation + state_b(3)*t_step;
end

position_storage(1:2,n) = currentPosition_n;
orientation_storage(n) = currentOrientation;

current_state = [currentPosition_n;currentOrientation;state_n;state_b(3)];
state_storage(:,n) = current_state;

% proportional error term
p_error = state_desired - current_state;

% derivative and integral error terms
if(n == 1)
    d_error = [0 0 0 0 0 0]';
    i_error = p_error;
else
   d_error = (p_error - error_storage(1:6,n-1))/t_step;
   i_error = error_storage(13:18,n-1)+(p_error*t_step);
end

% Store errors
error_storage(1:18,n) = [p_error; d_error; i_error]; 

% Calculate Forces and Tourque to apply
Fn_1 = k(1,1)*sum(p_error(1:2)) + k(1,2)*sum(d_error(1:2)) + k(1,3)*sum(i_error(1:2));
Fn_2 = k(2,1)*sum(p_error(3:4)) + k(2,2)*sum(d_error(3:4)) + k(2,3)*sum(i_error(3:4));
T    = k(3,1)*sum(p_error(5:6)) + k(3,2)*sum(d_error(5:6)) + k(3,3)*sum(i_error(5:6));

% convert forces to body frame
Fn = [Fn_1, Fn_2]';
Fb = bRn(currentOrientation)*Fn;