function nCb = nCbFromQuaternion(q)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% nCb = nCbFromQuaternion(q)
%
% finds a rotation matrix from a quaternion
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
% written by Joseph Gland on Sept 11, 2007
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


a=q(1);
b=q(2);
c=q(3);
d=q(4);

nCb = [a*a-b*b-c*c+d*d 2*(a*b-c*d) 2*(a*c+b*d);
       2*(a*b+c*d) -a*a+b*b-c*c+d*d 2*(b*c-a*d);
       2*(a*c-b*d) 2*(b*c+a*d) -a*a-b*b+c*c+d*d];
   
       