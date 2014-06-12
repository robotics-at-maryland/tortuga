function dz = tortugaDepthDynamics(t,z)
% dz = tortugaDepthDynamics(t,z)
%
% an implementation of the continuous time dynamics
% for the depth axis of tortuga 
%
% input force from thrusters are assumed to be zero order hold
%


%global variables for dyanmics equations
% z(3,i)=-c/m*z(2,i)+(Fthrust(i)/m)+constant/m;
global c;%drag
global m;%mass
global constant;%gravitational force - buoyant force
global Ftemp;%zero order hold value of thrust force
global depthCheckEpsilon;%depth error check margin to see 
%if sub is above surface of water or submerged


dz=zeros(2,1);%position velocity acceleration
%down is positive direction


if(z(1)>=depthCheckEpsilon)
    %water dynamics
    dz(1)=z(2);
    dz(2)=-(c/m)*z(2)+Ftemp/m+constant/m;
    dz(3)=0;
else
    dz(1)=0;
    dz(2)=0;
    dz(3)=0;
end

