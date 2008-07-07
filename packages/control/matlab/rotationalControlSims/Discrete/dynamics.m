
%% dynamics

%drag
%drag=zeros(3,1);%use this line to turn off drag
drag=Cd*diag([abs(w(1)) abs(w(2)) abs(w(3))])*w;

%buoyant moment
%buoyant=zeros(3,1);%use this line to turn off buoyant moment
Rot=R(q)';
buoyant=fb*[(rb(2)*Rot(3,3)-rb(3)*Rot(2,3));
            (rb(3)*Rot(1,3)-rb(1)*Rot(3,3));
            (rb(1)*Rot(2,3)-rb(2)*Rot(1,3))];

%propagate actual vehicle dynamics
dw=inv(H)*(S(H*w)*w+u-drag-buoyant);
%dw=inv(H)*(S(H*w)*w-drag-buoyant);

%propagate actual vehicle kinematics
dq=(1/2)*Q(q)*w;

