%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% controllerCode.m is used to test the matlab implementation of the control
%code, iterates for a specified interval of time
%
% written by Joseph Gland
% June 22 2007
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%initial parameters%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%define structs 
DesiredState = struct('speed',1,'depth',1,'quaternion',[1 2 3 4]','angularRate',[1 2 3]');
MeasuredState = struct('depth',1,'linearAcceleration',[1 2 3]','quaternion',[1 2 3 4]','angularRate',[1 2 3]');
ControllerState = struct('inertiaEstimate',[1 2 3 4 5 6]','adaptationGain',[1 0 0; 0 1 0; 0 0 1],'angularGain',[1 2 3; 4 5 6; 7 8 9],'depthPGain',1,'speedPGain',1);

%generate a measured quaternion
%euler parameters
e=[0; 0; 1];
e=e/norm(e);
phi = 0;
%measured quaternion
qMeasured=[e(1)*sin(phi/2);
e(2)*sin(phi/2);
e(3)*sin(phi/2);
cos(phi/2)];

%set measured state
MeasuredState.depth = 12;
MeasuredState.linearAcceleration = [-1 -1 -1]';
MeasuredState.quaternion = qMeasured;
MeasuredState.angularRate = [0.5 2 2]';

%generate a desired quaternion
%euler parameters
e=[0; 0; 1];%0 0 1
e=e/norm(e);
phi = pi/2;%pi/2
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
ControllerState.inertiaEstimate = [1 1 1 1 1 1]';
ControllerState.adaptationGain = eye(6);
ControllerState.angularGain = 2*[1 0 0; 0 1 0; 0 0 1];
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
stopTime=50;
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
%
% assume starting at rest conditions
%
storedPosition(3,1)=MeasuredState.depth;
storedQuaternion(:,1)=MeasuredState.quaternion;
storedAngularRate(:,1)=MeasuredState.angularRate;
storedEstimatedParameters(:,1)=ControllerState.inertiaEstimate;


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
    [rotationalTorques aHatNew]=rotationalController(MeasuredState,DesiredState,ControllerState,dt);
    ControllerState.inertiaEstimate = aHatNew;
    %store the new estimates of the adaptation parameters
    storedEstimatedParameters(:,index) = aHatNew;
    
    %format control signals for thrusters
    thrusterForces=computeThrusterForces(translationalForces,rotationalTorques);

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%
    %%%% react %%%%%%%%%%%%%%%%%
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    %apply control signals to simulated vehicle
    
    %TODO: implement thruster saturation model here
    
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
    %TODO: add buoyant moment and drag terms here time permitting
    %grab quaternion from last interation
    q=storedQuaternion(:,index-1);
    %convert to roll pitch yaw
    rollPitchYaw=[atan2(2*(q(4)*q(1)+q(2)*q(3)),(1-2*(q(1)^2+q(2)^2)));
                  asin(2*(q(4)*q(2)-q(3)*q(1)));
                  atan2(2*(q(4)*q(3)+q(1)*q(2)),(1-2*(q(2)^2+q(3)^2)))];
    %gyroscopic term of dynamic simulation
    gyroTerm=S(realInertia*storedAngularRate(:,index-1))*storedAngularRate(:,index-1);
    w_dot = inv(realInertia)*(outputTorques+gyroTerm);
    %integrate
    w_new=storedAngularRate(:,index-1)+w_dot*dt;
    %save the new point in the trajectory for plotting
    storedAngularRate(:,index)=w_new;
    
    
end


%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%plot trajectories%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%

figure(1)
subplot(4,1,1)
plot(time,storedQuaternion(1,:))
ylabel('q_1')
subplot(4,1,2)
plot(time,storedQuaternion(2,:))
ylabel('q_2')
subplot(4,1,3)
plot(time,storedQuaternion(3,:))
ylabel('q_3')
subplot(4,1,4)
plot(time,storedQuaternion(4,:),time,1,':')
ylabel('q_4')
xlabel('time (s)')

figure(2)
subplot(3,1,1)
plot(time,storedAngularRate(1,:))
ylabel('\omega_1 (rad/s)')
subplot(3,1,2)
plot(time,storedAngularRate(2,:))
ylabel('\omega_2 (rad/s)')
subplot(3,1,3)
plot(time,storedAngularRate(3,:))
ylabel('\omega_3 (rad/s)')
xlabel('time (s)')

figure(3)
subplot(3,2,1)
plot(time,storedEstimatedParameters(1,:))
ylabel('a_1')
subplot(3,2,2)
plot(time,storedEstimatedParameters(2,:))
ylabel('a_2')
subplot(3,2,3)
plot(time,storedEstimatedParameters(3,:))
ylabel('a_3')
subplot(3,2,4)
plot(time,storedEstimatedParameters(4,:))
ylabel('a_4')
subplot(3,2,5)
plot(time,storedEstimatedParameters(5,:))
ylabel('a_5')
xlabel('time (s)')
subplot(3,2,6)
plot(time,storedEstimatedParameters(6,:))
ylabel('a_6')
xlabel('time (s)')