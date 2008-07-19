function DCM = DCMFromQuaternion(q)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% DCM = DCMFromQuaternion(q)
%
% as the function name cleverly indicates, this function generates a
% direction cosine matrix from a quaternion
%
% output: DCM - direction cosine matrix
%
% input: q - a quaternion with the parameterization:
%
%                   [q1] = [e1*sin(et/2)]
%               q = |q2|   |e2*sin(et/2)|
%                   |q3|   |e3*sin(et/2)|
%                   [q4]   [  cos(et/2) ] 
%
%               where euler axis = [e1,e2,e3] and euler angle = et
%
% Written by Joseph Gland on June 29, 2007
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

a=q(1);
b=q(2);
c=q(3);
d=q(4);

DCM = [a^2-b^2-c^2+d^2 2*(a*b-c*d) 2*(a*c+b*d);
        2*(a*b+c*d) -a^2+b^2-c^2+d^2 2*(b*c-a*d);
        2*(a*c-b*d) 2*(b*c+a*d) -a^2-b^2+c^2+d^2];
