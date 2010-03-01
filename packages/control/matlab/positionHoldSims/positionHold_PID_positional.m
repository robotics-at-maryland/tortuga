function [ Fb T ] = positionHold_PID_positional( state_b, k, setpoint_n)
% positionHold_PID_positional
%
% Jonathan Wonders 2010-2-28
%
%   state = [ r_dot_1 ]
%           [ r_dot_2 ]
%           [ t_dot   ]
%
%   k = [ kp_1 ]
%       [ kp_2 ]
%       [ kp_t ]
%       [ kd_1 ]
%       [ kd_2 ]
%       [ kd_t ]
%       [ ki_1 ]
%       [ ki_2 ]
%       [ ki_t ]
%
%   setpoint_n = [ r_1   ]
%                [ r_2   ]
%                [ theta ]
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
    currentPosition_n = currentPosition_n + (state_n(1:2) + input_stoarge(1:2,n)/2)*t_step;
    %currentPosition_n = currentPosition_n + state_n(1:2)*t_step;
    % Numerically integrate rotational speed to get orientation estimate
    currentOrientation = currentOrientation + state_b(3)*t_step;
end

p_error = [currentPosition_n - setpoint_n(1:2); currentOrientation - setpoint_n(3)];

position_storage(1:2,n) = currentPosition_n;
orientation_storage(n) = currentOrientation;

% calculate derivative and integral terms
if(n == 1)
    d_error = [0 0 0]';
    i_error = p_error;
else
   d_error = (p_error - error_storage(1:3,n-1))/t_step;
   i_error = error_storage(7:9,n-1)+(p_error*t_step);
end

% Store errors
error_storage(1:9,n) = [p_error; d_error; i_error]; 

% Calculate Forces and Tourque to apply
Fn_1 = -dot(error_storage(1:3:7,n),k(1:3:7));
Fn_2 = -dot(error_storage(2:3:8,n),k(2:3:8));
T    = -dot(error_storage(3:3:9,n),k(3:3:9));

% convert forces to inertial frame
Fn = [Fn_1, Fn_2]';
Fb = bRn(currentOrientation)*Fn;