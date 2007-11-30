%function graphing(alpha)
%function plotSimulation(alpha)

clear

alpha=0.5;
error = 15;

%y = x + vt + .5at^2

x0 = 150;
v0 = 0;
a = -9.8;
time = [];
x = [];%true measurements
measured = [];%noisy measurements
t2 = [];
sample_rate = .1;

%noise variance is error*variance_randn = error*1
noiseVariance=error*1;
R=sqrt(noiseVariance);
%assume no process noise
Q=zeros(2);
%discrete time system model
F=[1 .1; 0 1];
G=[.05; .1];
H=[1 0];
u=-9.8;%input is always 9.8 m/s^2
%initial condition (at k=1)
x_hat_minus=[x0; v0];
x_hat=[];
%initial covariance estimate (at k=1)
P_minus=[10 0; 0 1];


% for t = 0:.1:5
%    x(end+1) = x0 + v*t - .5*a*t;
%    v = a*t;
%    t2(end+1) = t;
%    t = t + .2;
% end

for t = 0:sample_rate:5
    
    %Create Time Array
    time(end+1) = t;

    %simulate actual dynamics
    y0 = [x0 v0];
    [time2,y] = ode45('makex',[0:sample_rate:(time(end)+.1)],y0);
    x(end+1) = y(end,1);

    %make noisy measurement
    measured(end+1) = x(end) + (error*randn);

    %iterate through data
    %for k=1:length(time)
        %compute kalman gain
        K=P_minus*H'*inv(H*P_minus*H'+R);
        %update state estimate
        x_hat(:,end+1)=x_hat_minus+K*(measured(end)-H*x_hat_minus);
        %update error covariance
        P=(eye(2)-K*H)*P_minus;
        %project state estimate
        x_hat_minus=F*x_hat(:,end)+G*u;
        %project coraviance estimate
        P_minus=F*P*F'+Q;

    %end
end

plot(time, x, ':g');
hold on;
plot(time, measured, '.r');

plot(time, x_hat(1,:), '.b');
xlabel('Time');
ylabel('Position');
legend('True', 'Measured', 'Estimated');


hold off;