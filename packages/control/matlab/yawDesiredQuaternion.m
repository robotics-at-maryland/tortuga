function newDesiredQuaternion = yawDesiredQuaternion(psi,oldDesiredQuaternion)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% newDesiredQuaternion = yawDesiredQuaternion(psi,oldDesiredQuaternion)
%
%set rotation "time" equal to 1 second
%rotate about yaw axis for w rad/s
%set w = theta/time 
%find q_dot
%integrate q for "time" (effectively rotating by desired amount theta)
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

rotationTime=1;

%roll = 0,  pitch = 0,   yaw = psi (turn angle in radians)
omega=[0; 0; psi];

%find quaternion "derivative"
omegaMatrix=[0 omega(3) -omega(2) omega(1);
             -omega(3) 0 omega(1) omega(2);
             omega(2) -omega(1) 0 omega(3);
             -omega(1) -omega(2) -omega(3) 0];
         
q_dot=(1/2)*omegaMatrix*oldDesiredQuaternion;


%"integrate"
q_new=oldDesiredQuaternion+q_dot*rotationTime;

%save the normalized "integrated" quaternion
newDesiredQuaternion=q_new/norm(q_new);