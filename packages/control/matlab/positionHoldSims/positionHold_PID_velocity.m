function [ Fb Tb ] = positionHold_PID_velocity( x, k, setpoint )
% positionHold_PID_velocity
% 
% Jonathan Wonders 2010-2-16
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


persistent INPUT_STORAGE;
global t_step;
global current_time;

n = uint32(current_time/t_step); % index of current state in INPUT_STORAGE

p_error = x - setpoint;

if(n <= 2)
    i_error = [0 0 0]';
    d_error = [0 0 0]';
else
    d_error = (p_error - INPUT_STORAGE(1:3,n-1))/t_step; % derivative estimate
    i_error = INPUT_STORAGE(7:9,n-1) + (p_error*t_step); % integral estimate
end

INPUT_STORAGE(1:9,n) = [p_error; d_error; i_error];

% Calculate Forces and Tourque to apply
Fb_1 = -dot(INPUT_STORAGE(1:3:7,n),k(1:3:7));
Fb_2 = -dot(INPUT_STORAGE(2:3:8,n),k(2:3:8));
Tb   = -dot(INPUT_STORAGE(3:3:9,n),k(3:3:9));
Fb = [Fb_1, Fb_2]';

end