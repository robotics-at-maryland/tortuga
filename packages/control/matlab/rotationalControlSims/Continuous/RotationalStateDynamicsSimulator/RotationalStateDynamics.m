function dx=RotationalSimDynamics(t,x)

%-------------------------------------------------------------------------%
%                                                                         
%       Rotational State Dynamics Simulator for Tortuga                         
%       
%       1.Initialization
%       2.Inside For loop or ode45
%           CANNOT be done by robot:
%               a.Measurements
%           CAN be done by robot:
%               b.Estimation (interperetation of measurements)
%               c.Planning (don't have)
%               d.Control (Execution of plan) (not yet)
%               e.Simulation of dynamics
%       3.Plot results/data file
%
%
%
%       Written by Simon Knister and John Martin. Based on the work of M.D.
%       Shuster and S.D. Oh in "Three-Axis Attitude Determination from Vector
%       Observations". Done with the assistance of Joe Gallante.
%       (c) 2009 for Robotics@Maryland
%
%-------------------------------------------------------------------------%
%% Pull in Global Constants

    %Global weights
    global a1; 
    global a2;

    %System Inertia
    global H;

    %Drag 
    global Cd;

    %Buoyancy Force
    global fb;

    %vector from center of gravity (CG) to center of buoyancy (CB)
    global rb;

    %Known inertial constants
    global mag_vec_nf;
    global acc_vec_nf;

%% unpack ODE data
q=x(1:4);
w=x(5:7);
q_d=x(8:11);
w_d=x(12:14);
qhat=x(15:18);

%fix numerical quaternion drift

%% measurement:CANNOT be done by robot
Rot = R(q);
acc_vec_bf = Rot * acc_vec_nf; 
mag_vec_bf = Rot * mag_vec_nf;

w_meas=w;

%% estimation:CAN be done by robot
        %Triad: (Develops the body to inertial rotation matrix bRn)
        n3 = -acc_vec_bf/norm(acc_vec_bf);
        n2 = cross(n3,mag_vec_bf)/norm(mag_vec_bf);
        n1 = cross(n2,n3);
        %Accounting for round-off errors
            n1 = n1/norm(n1);
        bRn = [n1 n2 n3];
        
        %Quest:
        cos_func = dot(mag_vec_bf,acc_vec_bf)*dot(mag_vec_nf,acc_vec_nf) + norm(cross(mag_vec_bf,acc_vec_bf))*norm(cross(mag_vec_nf,acc_vec_nf));
        eig_val_max = sqrt(a1^2 + 2*a1*a2*cos_func + a2^2)
        B = a1*mag_vec_bf*mag_vec_nf' + a2*acc_vec_bf*acc_vec_nf';
        S = B + B';
        Z = a1*cross(mag_vec_bf,mag_vec_nf) + a2*cross(acc_vec_bf,acc_vec_nf);
        sigma2 = 0.5*trace(S);
        delta = det(S);
        kappa = trace(inv(S)*det(S));       %Note: Assumes S is invertible
        alpha = eig_val_max^2 - sigma2^2 + kappa;
        beta = eig_val_max - sigma2;
        gamma = (eig_val_max + sigma2)*alpha - delta;
        X = (alpha*eye(3) + beta*S + S*S)*Z;
        q_quest = 1/sqrt(gamma^2 + norm(X)^2)*[X; gamma];
            q_quest = q_quest/norm(q_quest) %Ensuring that q_opt is normalized
        
        

q_meas = quaternionFromnCb(nCbFromIMU(mag_vec_bf,acc_vec_bf))
%q_meas=q;

%quaternion estimation that requires only angular rate gyro
dqhat = (1/2)*Q(qhat)*w_meas;
%dwhat = 

%% controller

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



%put output states into vector for ode45
dx=[dq; dw; dq_d; dw_d; dqhat];
