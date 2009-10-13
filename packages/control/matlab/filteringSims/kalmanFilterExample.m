% this is a simulation of an object falling under the influence of gravity
% the simulation includes:
%    simulation of the actual dynamics in the real world
%    simulation of extremely noisey measurements made by an imperfect sensor
%    use of a Kalman filter to attempt to learn the position of the object
%          using only noisey measurements


clear

alpha=0.5;
error = 15;

%y = x + vt + .5at^2
% global sim;
% sim.x0 = 150;
% sim.v0 = 0;
% sim.a = -9.8;



sample_rate = .1;


%initial conditions (starts the true position
x=[150 0]'; %x=[position velocity]';
t0=0;

time = [0];
%x = [];%true measurements
measured = [151]';%noisy measurements
t2 = [];


%first make sure global variable Kalman is cleared
clear global Kalman;
%create a fresh copy of global variable Kalman
global Kalman;
Kalman.noiseVariance=error*1;
Kalman.R=sqrt(Kalman.noiseVariance);
%assume no process noise
Kalman.Q=zeros(2);
%discrete time system model
Kalman.F=[1 .1; 0 1];
Kalman.G=[.05; .1];
Kalman.H=[1 0];
Kalman.u=-9.8;%input is always 9.8 m/s^2
%initial condition (at k=1)
Kalman.x_hat_minus=[x(1,1); x(2,1)];
Kalman.x_hat=[Kalman.x_hat_minus];
%initial covariance estimate (at k=1)
Kalman.P_minus=[10 0; 0 1];



for t = t0+sample_rate:sample_rate:5
    
    %Create Time Array
    time(end+1) = t;

    %simulate actual dynamics
    %this line just integrates the differential equations for the dynamics
    [time_int,x_int]=ode45('makex',[time(end-1) time(end)],x(:,end));
    x(:,end+1) = x_int(end,:)';

    %make noisy measurement
    measured(end+1) = x(1,end) + (error*randn);
    
    Kalman.measured = measured;%gets recreated each time
    step_kalman%function to actually perform the kalman filter:
        %compute kalman gain
        %K=Kalman.P_minus*Kalman.H'*inv(Kalman.H*Kalman.P_minus*Kalman.H'+Kalman.R);
        %update state estimate
        %Kalman.x_hat(:,end+1)=Kalman.x_hat_minus+K*(measured(end)-Kalman.H*Kalman.x_hat_minus);
        %update error covariance
        %P=(eye(2)-K*Kalman.H)*Kalman.P_minus;
        %project state estimate
        %Kalman.x_hat_minus=Kalman.F*Kalman.x_hat(:,end)+Kalman.G*Kalman.u;
        %project coraviance estimate
        %Kalman.P_minus=Kalman.F*P*Kalman.F'+Kalman.Q;

end

plot(time, x(1,:), ':g', 'linewidth', 2);
hold on;
plot(time, measured, '.r');

plot(time, Kalman.x_hat(1,:), '.b');
xlabel('Time');
ylabel('Position');
legend('True', 'Measured', 'Estimated');


hold off;