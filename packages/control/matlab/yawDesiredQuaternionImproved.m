function newDesiredQuaternion = yawDesiredQuaternionImproved(psi,oldDesiredQuaternion)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% newDesiredQuaternion = yawDesiredQuaternionImproved(psi,oldDesiredQuaternion)
%
%
% assumes:
% q      - a quaternion with the parameterization:
%
%                   [q1] = [e1*sin(et/2)]
%               q = |q2|   |e2*sin(et/2)|
%                   |q3|   |e3*sin(et/2)|
%                   [q4]   [  cos(et/2) ] 
%
%               where euler axis = [e1,e2,e3] and euler angle = et
%
% Written by Joseph Gland on 7-7-2007
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%construct rotation quaternion
qRot=quaternionFromEulerAxis([0 0 1],psi);

%construct 4d cross product matrix
qRotMatrix=[qRot(4) -qRot(3) qRot(2) qRot(1);
            qRot(3) qRot(4) -qRot(1) qRot(2);
            -qRot(2) qRot(1) qRot(4) qRot(3);
            -qRot(1) -qRot(2) -qRot(3) qRot(4)];
        
%compute cross product
newDesiredQuaternion=qRotMatrix*oldDesiredQuaternion;