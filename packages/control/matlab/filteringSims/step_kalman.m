function step_kalman

global Kalman;

%iterate through data
    %for k=1:length(time)
        %compute kalman gain
        K=Kalman.P_minus*Kalman.H'*inv(Kalman.H*Kalman.P_minus*Kalman.H'+Kalman.R);
        %update state estimate
        Kalman.x_hat(:,end+1)=Kalman.x_hat_minus+K*(Kalman.measured(end)-Kalman.H*Kalman.x_hat_minus);
        %update error covariance
        P=(eye(2)-K*Kalman.H)*Kalman.P_minus;
        %project state estimate
        Kalman.x_hat_minus=Kalman.F*Kalman.x_hat(:,end)+Kalman.G*Kalman.u;
        %project coraviance estimate
        Kalman.P_minus=Kalman.F*P*Kalman.F'+Kalman.Q;