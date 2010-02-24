function [ Fb Tb ] = positionHold_PID_velocity( x, k, t )
% positionHold_PID_velocity
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
%   t = [ time      ]
%       [ time_step ]

persistent INPUT_STORAGE;

n = uint32(t(1)/t(2)); % index of current state in INPUT_STORAGE

p_state = x;

if(n <= 2)
    i_state = [0 0 0]';
    d_state = [0 0 0]';
end

if(n > 2)
    d_state = INPUT_STORAGE(1:3,n-1) - x./t(2); % derivative estimate
    i_state = INPUT_STORAGE(7:9,n-1) + x.*t(2); % integral estimate
end

INPUT_STORAGE(1:9,n) = [p_state; d_state; i_state];

% Calculate Forces and Tourque to apply
Fb_1 = -dot(INPUT_STORAGE(1:3:7,n),k(1:3:7));
Fb_2 = -dot(INPUT_STORAGE(2:3:8,n),k(2:3:8));
Tb   = -dot(INPUT_STORAGE(3:3:9,n),k(3:3:9));
Fb = [Fb_1, Fb_2]';

end