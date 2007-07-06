function storedValues = SimulateController(TESTCPP)
% choose to simulate w/ or w/o cpp implementation
% 1 = with,   0 = without

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%initial parameters%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%define structs 
DesiredState = struct('speed',1,'depth',1,'quaternion',[1 2 3 4]','angularRate',[1 2 3]');
MeasuredState = struct('depth',1,'linearAcceleration',[1 2 3]','quaternion',[1 2 3 4]','angularRate',[1 2 3]');
ControllerState = struct('inertiaEstimate',[1 2 3 4 5 6]','adaptationGain',[1 0 0; 0 1 0; 0 0 1],'angularGain',[1 2 3; 4 5 6; 7 8 9],'depthPGain',1,'speedPGain',1);

%generate a measured quaternion for STARTING POSITION
%euler parameters
e=[1; 0; 0];
e=e/norm(e);
phi = pi/8;
%measured quaternion
qMeasured=[e(1)*sin(phi/2);
e(2)*sin(phi/2);
e(3)*sin(phi/2);
cos(phi/2)];

%set measured state
%
% assume starting at rest conditions
%
MeasuredState.depth = 12;
MeasuredState.linearAcceleration = [-1 -1 -1]';
MeasuredState.quaternion = qMeasured;
MeasuredState.angularRate = [1 0 0]';

%generate a desired quaternion  or FINAL POSITION
%euler parameters
e=[0; 0; 1];%0 0 1
e=e/norm(e);
phi = pi/4;%pi/2
%original quaternion
qDesired=[e(1)*sin(phi/2);
e(2)*sin(phi/2);
e(3)*sin(phi/2);
cos(phi/2)];

%set desired state
DesiredState.speed = 3;
DesiredState.depth = 8;
DesiredState.quaternion = qDesired;
DesiredState.angularRate = [0 0 0]';

%set controller state
ControllerState.inertiaEstimate = [0.2 0 0 1.3 0 1.3]';
ControllerState.adaptationGain = eye(6);
ControllerState.angularGain = [1 0 0; 0 1 0; 0 0 1];
ControllerState.depthPGain = 4;
ControllerState.speedPGain = 1;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%exclusively simulation code%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%set up simulation%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%simulation parameters
%
% NOTE: change these parameters if better values become available
%
realInertia=[0.201 0 0; 0 1.288 0; 0 0 1.288];%kg*m^2
realMass=23;%kg
%thruster positions from CG in meters (also hard coded in
%computeThrusterForces.m)  TODO: implement better way of storing this data?
% should it exist in ControllerState? (i don't think so....)
rPort = 0.1905;%m
rStarboard = 0.1905;%m
rFore = 0.3366;%m
rAft = 0.3366;%m

%set up timing array
startTime=0;
stopTime=70;
numSimPoints=2000;
time=linspace(startTime,stopTime,numSimPoints);
dt=time(2)-time(1);

%set up storage arrays for later plotting
storedPosition=zeros(3,numSimPoints);
storedVelocity=zeros(3,numSimPoints);
storedAcceleration=zeros(3,numSimPoints);
storedQuaternion=[zeros(3,numSimPoints); ones(1,numSimPoints)];
storedAngularRate=zeros(3,numSimPoints);
storedEstimatedParameters=zeros(6,numSimPoints);

%initialize storage arrays for plotting
storedPosition(3,1)=MeasuredState.depth;
storedQuaternion(:,1)=MeasuredState.quaternion;
storedAngularRate(:,1)=MeasuredState.angularRate;
storedEstimatedParameters(:,1)=ControllerState.inertiaEstimate;

%%%%%%%
%%%%%Setup data storage


%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%simulation loop%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%
for index=2:1:numSimPoints
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%
    %%%% sense %%%%%%%%%%%%%%%%%
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    %take "measurements" from previously saved values in stored* arrays
    MeasuredState.depth=storedPosition(3,index-1);
    MeasuredState.linearAcceleration=storedAcceleration(:,index-1);
    MeasuredState.quaternion=storedQuaternion(:,index-1);
    MeasuredState.angularRate=storedAngularRate(:,index-1);
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%
    %%%% deliberate %%%%%%%%%%%%
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    %translational control
    translationalForces=translationalController(MeasuredState,DesiredState,ControllerState,dt);

    %rotational control
    %how I'd like to use the rotationalController command
    %rotationalTorques=rotationalController(MeasuredState,DesiredState,ControllerState,dt);
    %how matlab forced me to use the rotationalController command
    %[rotationalTorques aHatNew]=rotationalController(MeasuredState,DesiredState,ControllerState,dt);
    [rotationalTorques aHatNew]=BongWiePDControl(MeasuredState,DesiredState,ControllerState,dt);
    ControllerState.inertiaEstimate = aHatNew;
    %store the new estimates of the adaptation parameters
    storedEstimatedParameters(:,index) = aHatNew;
    
    %format control signals for thrusters
    thrusterForces=computeThrusterForces(translationalForces,rotationalTorques);
    
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%
    %%%% react %%%%%%%%%%%%%%%%%
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    %apply control signals to simulated vehicle
    
    %thruster saturation model
    thrusterForces = thrusterSaturationModel(thrusterForces);
        
    %angular position
    %quaternion derivative
    q_dot = J(storedQuaternion(:,index-1))*storedAngularRate(:,index-1);    
    %integrate
    q_new = storedQuaternion(:,index-1) + q_dot*dt;
    %normalize
    q_new = q_new/norm(q_new);
    %save the new point in the trajectory for plotting
    storedQuaternion(:,index)=q_new;
    
    %angular velocity
    %compute body centered torques (basically undo the work of
    %computeThrusterForces)   
    outputTorques=[0; 
                   (rFore*thrusterForces(3)-rAft*thrusterForces(4));
                   (rStarboard*thrusterForces(2)-rPort*thrusterForces(1))];
    %outputTorques=rotationalTorques;
    
    
    
    
    %implement control torques into dynamic model of vehicle
    %grab quaternion from last interation
    q=storedQuaternion(:,index-1);
    %convert to roll pitch yaw
    rollPitchYaw=[atan2(2*(q(4)*q(1)+q(2)*q(3)),(1-2*(q(1)^2+q(2)^2)));
                  asin(2*(q(4)*q(2)-q(3)*q(1)));
                  atan2(2*(q(4)*q(3)+q(1)*q(2)),(1-2*(q(2)^2+q(3)^2)))];
    %gyroscopic term of dynamic simulation
    gyroTerm=S(realInertia*storedAngularRate(:,index-1))*storedAngularRate(:,index-1);
    %buoyant term of dynamic simulation (constants are magic numbers based
    %off SCAMP data, replace with real numbers later)  THIS IS IN INERTIAL
    %FRAME
    buoyancyTerm=[-5*sin(rollPitchYaw(1)-0);
                  -0.3*sin(rollPitchYaw(2)-5*pi/180);
                  0];
    %rotate buoyancy term to put in body frame
    buoyancyTerm=rotationMatrixFromQuaternion(storedQuaternion(:,index-1))*buoyancyTerm;
    %drag term of dynamic simulation (constants are magic numbers based off
    %SCAMP data, replace with real numbers late)
    dragTerm=(-4)*[0.5*storedAngularRate(1,index-1)*abs(storedAngularRate(1,index-1));
              0.5*storedAngularRate(2,index-1)*abs(storedAngularRate(2,index-1));
              0.5*storedAngularRate(3,index-1)*abs(storedAngularRate(3,index-1))];
    %incorporate all dynamic sim components into angular acceleration equation
    %outputTorques
    %gyroTerm
    %buoyancyTerm
    %dragTerm
    w_dot = inv(realInertia)*(outputTorques+gyroTerm+buoyancyTerm+dragTerm);
    %w_dot = inv(realInertia)*(gyroTerm+buoyancyTerm+dragTerm);
    %integrate
    w_new=storedAngularRate(:,index-1)+w_dot*dt;
    %save the new point in the trajectory for plotting
    storedAngularRate(:,index)=w_new;
    
    
end

storedValues = struct('position', storedPosition, 'velocity', storedVelocity, ...
                      'quaternion', storedQuaternion, ...
                      'acceleration', storedAcceleration, ... 
                      'angularRate', storedAngularRate, ...
                      'estimatedParameters', storedEstimatedParameters, ...
                      'time', time)
