function q = quaternionFromEulerAxis(e, et)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% q = quaternionFromEulerAxis(e, et)
%
% finds a quaternion from a set of euler axes and angle
%
%
% q      - a quaternion with the parameterization:
%
%                   [q1] = [e1*sin(et/2)]
%               q = |q2|   |e2*sin(et/2)|
%                   |q3|   |e3*sin(et/2)|
%                   [q4]   [  cos(et/2) ] 
%
%               where euler axis = [e1,e2,e3] and euler angle = et
%
% written by Joseph Gland on June 2, 2007
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


q=[e(1)*sin(et/2);
   e(2)*sin(et/2);
   e(3)*sin(et/2);
   cos(et/2)];