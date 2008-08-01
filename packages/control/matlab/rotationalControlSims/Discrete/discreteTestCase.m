%void AdaptiveRotationalController(MeasuredState* measuredState,
%                                   DesiredState* desiredState,
%                                   ControllerState* controllerState,
%                                   double dt,
%                                   double* rotationalTorques)
%

%% input goes here
dt = 0.025;%time step
dtMin = 0.001;%from vehicle sim
dtMax = 0.05;%from vehicle sim
w = [pi/4 0 0]';%actual angular rate
wd = [0 0 0]';%desired angular rate
q = [[1 0 0]'*sin((pi/4)/2); cos((pi/4)/2)];%actual position
qd = [[1 0 0]'*sin(0/2); cos(0/2)];%desired position
adaptCtrlRotLambda = 1;
adaptCtrlRotGamma = 6;
adaptCtrlRotK = 3;
adaptCtrlParams = zeros(12,1);

%  //be extra safe about dt values
    if(dt < dtMin)
        dt = dtMin;
    end
    if(dt > dtMax)
        dt = dtMax;
    end


%	/****************************
%       propagate desired states 
%	*****************************/
	
%	//integrate desired angular velocity
%	//derivative of angular rate desired
dwd = zeros(3,1);
%	//simple integration
%	wd = wd+dwd*dt;
wd = wd+dwd*dt;
	
%	//integrate desired angular position
%	//compute derivative of quaternion desired
dqd = 0.5*Q(qd)*wd;
%	//simple integration
qd = qd+dqd*dt;
%	//fix numerical drift
qd=qd/norm(qd,2);
%	//	std::cout << "qd =  " << qd.x << " " << qd.y << " " << qd.z << " " << qd.w << std::endl;

%	//TODO: save to desiredStruct!!!!!

%	/****************************
 %      compute error metrics
%	*****************************/

%	//compute error quaternion
%	//WARNING!!! I had to "transpose" the quaternion order here.  
%	//Is this a problem with the errorQuaternion function (ie backwards)?
%	//	Quaternion qc_tilde = q.errorQuaternion(qd);

%qc_tilde = quaternionProduct(qd,q)  %THIS IS DEFINITELY INCORRECT    ---  JMG 2008-7-29

%	Quaternion qc_tilde = qd.errorQuaternion(q);
%	//	std::cout << "qc = " << qc_tilde.x << " " << qc_tilde.y << " " << qc_tilde.z << " " << qc_tilde.w << std::endl;

qc_tilde = quaternionProduct(q,qd);   %THIS IS CORRECT

%	//compute rotation matrix
RotMatc_tilde = R(qc_tilde);

%	//extract vector portion of qc_tilde
epsilon_c_tilde=qc_tilde(1:3);
	
%	//compute composite error metrics
%	Vector3 wr = RotMatc_tilde*wd-(controllerState->adaptCtrlRotLambda)*epsilon_c_tilde;
wr=RotMatc_tilde*wd-adaptCtrlRotLambda*epsilon_c_tilde;
shat = w-wr;

%	//compute angular rate error
wc_tilde = w-RotMatc_tilde*wd;

%	//compute derivative of wr
%	Matrix3 S;//temp skew symmetric matrix
Sepsilon_c_tilde=S(epsilon_c_tilde);
%	Matrix3 Q1;//temp Q1 matrix (subset of Q matrix)
%Q1epsilon_c_tilde=Q1(qc_tilde); % THIS IS TO DOUBLE CHECK THE WAY WE'RE DOING IT IN C (below)
%	S.ToSkewSymmetric(epsilon_c_tilde);
	Q1epsilon_c_tilde(1,1)=qc_tilde(4)+Sepsilon_c_tilde(1,1);
	Q1epsilon_c_tilde(1,2)=Sepsilon_c_tilde(1,2);
	Q1epsilon_c_tilde(1,3)=Sepsilon_c_tilde(1,3);
	Q1epsilon_c_tilde(2,1)=Sepsilon_c_tilde(2,1);
	Q1epsilon_c_tilde(2,2)=qc_tilde(4)+Sepsilon_c_tilde(2,2);
	Q1epsilon_c_tilde(2,3)=Sepsilon_c_tilde(2,3);
	Q1epsilon_c_tilde(3,1)=Sepsilon_c_tilde(3,1);
	Q1epsilon_c_tilde(3,2)=Sepsilon_c_tilde(3,2);
	Q1epsilon_c_tilde(3,3)=qc_tilde(4)+Sepsilon_c_tilde(3,3);
    Q1epsilon_c_tilde;% THIS MATCHES THE MATLAB IMPLEMENTATION USED ABOVE
	dwr = RotMatc_tilde*dwd;
	Swc_tilde=S(wc_tilde);
	dwr=dwr-Swc_tilde*RotMatc_tilde*wd;
	dwr=dwr-adaptCtrlRotLambda*Q1epsilon_c_tilde*wc_tilde;

%	/**********************************
%	  compute parameterization matrix
%	 **********************************/

%	//rotation matrix from vehicle quaternion
%	Matrix3 Rot;
%	q.ToRotationMatrix(Rot);

Rot=R(q);

%	//the dreaded parameterization matrix

%   C implementation
%	MatrixN Y(3,12);
Ycpp=zeros(3,12);
% 	//inertia terms
% 	Y[0][0]=dwr[0];
% 	Y[0][1]=dwr[1]-w[0]*wr[2];
% 	Y[0][2]=dwr[2]+w[0]*wr[1];
% 	Y[1][0]=w[0]*wr[2];
% 	Y[1][1]=dwr[0]+w[1]*wr[2];
% 	Y[1][2]=-w[0]*wr[0]+w[2]*wr[2];
% 	Y[2][0]=-w[0]*wr[1];
% 	Y[2][1]=w[0]*wr[0]-w[1]*wr[1];
% 	Y[2][2]=dwr[0]-w[2]*wr[1];
	Ycpp(1,1)=dwr(1);
	Ycpp(1,2)=dwr(2)-w(1)*wr(3);
	Ycpp(1,3)=dwr(3)+w(1)*wr(2);
	Ycpp(2,1)=w(1)*wr(3);
	Ycpp(2,2)=dwr(1)+w(2)*wr(3);
	Ycpp(2,3)=-w(1)*wr(1)+w(3)*wr(3);
	Ycpp(3,1)=-w(1)*wr(2);
	Ycpp(3,2)=w(1)*wr(1)-w(2)*wr(2);
	Ycpp(3,3)=dwr(1)-w(3)*wr(2);
%	//more inertia terms
% 	Y[0][3]=-w[1]*wr[2];
% 	Y[0][4]=w[1]*wr[1]-w[2]*wr[2];
% 	Y[0][5]=w[2]*wr[1];
% 	Y[1][3]=dwr[1];
% 	Y[1][4]=dwr[2]-w[1]*wr[0];
% 	Y[1][5]=-w[2]*wr[0];
% 	Y[2][3]=w[1]*wr[0];
% 	Y[2][4]=dwr[1]+w[2]*wr[0];
% 	Y[2][5]=dwr[2];
	Ycpp(1,4)=-w(2)*wr(3);
	Ycpp(1,5)=w(2)*wr(2)-w(3)*wr(3);
	Ycpp(1,6)=w(3)*wr(2);
	Ycpp(2,4)=dwr(2);
	Ycpp(2,5)=dwr(3)-w(2)*wr(1);
	Ycpp(2,6)=-w(3)*wr(1);
	Ycpp(3,4)=w(2)*wr(1);
	Ycpp(3,5)=dwr(2)+w(3)*wr(1);
	Ycpp(3,6)=dwr(3);
%	//buoyancy terms
% 	Y[0][6]=0;
% 	Y[0][7]=-Rot[2][2];
% 	Y[0][8]=Rot[1][2];
% 	Y[1][6]=Rot[2][2];
% 	Y[1][7]=0;
% 	Y[1][8]=-Rot[0][2];
% 	Y[2][6]=-Rot[1][2];
% 	Y[2][7]=Rot[0][2];
% 	Y[2][8]=0;
	Ycpp(1,7)=0;
	Ycpp(1,8)=-Rot(3,3);
	Ycpp(1,9)=Rot(2,3);
	Ycpp(2,7)=Rot(3,3);
	Ycpp(2,8)=0;
	Ycpp(2,9)=-Rot(1,3);
	Ycpp(3,7)=-Rot(2,3);
	Ycpp(3,8)=Rot(1,3);
	Ycpp(3,9)=0;
%	//drag terms
% 	Y[0][9]=-w[0]*fabs(w[0]);
% 	Y[0][10]=0;
% 	Y[0][11]=0;
% 	Y[1][9]=0;
% 	Y[1][10]=-w[1]*fabs(w[1]);
% 	Y[1][11]=0;
% 	Y[2][9]=0;
% 	Y[2][10]=0;
% 	Y[2][11]=-w[2]*fabs(w[2]);
	Ycpp(1,10)=-w(1)*abs(w(1));
	Ycpp(1,11)=0;
	Ycpp(1,12)=0;
	Ycpp(2,10)=0;
	Ycpp(2,11)=-w(2)*abs(w(2));
	Ycpp(2,12)=0;
	Ycpp(3,10)=0;
	Ycpp(3,11)=0;
	Ycpp(3,12)=-w(3)*abs(w(3));

% matlab implementation
Yinertia1=[dwr(1) dwr(2)-w(1)*wr(3) dwr(3)+w(1)*wr(2);
           w(1)*wr(3) dwr(1)+w(2)*wr(3) -w(1)*wr(1)+w(3)*wr(3);
           -w(1)*wr(2) w(1)*wr(1)-w(2)*wr(2) dwr(1)-w(3)*wr(2)];
       
Yinertia2=[-w(2)*wr(3) w(2)*wr(2)-w(3)*wr(3) w(3)*wr(2);
           dwr(2) dwr(3)-w(2)*wr(1) -w(3)*wr(1);
           w(2)*wr(1) dwr(2)+w(3)*wr(1) dwr(3)];
       
Ybuoyancy=[0 -Rot(3,3) Rot(2,3);
           Rot(3,3) 0 -Rot(1,3);
           -Rot(2,3) Rot(1,3) 0];
       
Ydrag=[-w(1)*abs(w(1)) 0 0;
       0 -w(2)*abs(w(2)) 0;
       0 0 -w(3)*abs(w(3))];


Ymatlab=[Yinertia1 Yinertia2 Ybuoyancy Ydrag];
    
%Compare the two Y matrices
Ycpp
Ymatlab
Ycpp-Ymatlab
    
Y=Ycpp;
    
%	/**********************************
%	  adaptation law
%	 **********************************/


%	//use parameter adaptation law
%	MatrixN Ytranspose = Y.transpose();
%	MatrixN dahat = -(controllerState->adaptCtrlRotGamma)*Ytranspose*shat;
dahat = -adaptCtrlRotGamma*(Y')*shat;
	
%	//integrate parameter estimates
adaptCtrlParams = adaptCtrlParams+dahat*dt;
%	//TODO: store in controllerState

%	/**********************************
%	  control law
%	 **********************************/

%	MatrixN adaptiveTerm = Y*(controllerState->adaptCtrlParams);
adaptiveTerm=Y*adaptCtrlParams;

%	Vector3 output(adaptiveTerm[0][0],adaptiveTerm[1][0],adaptiveTerm[2][0]);
output=adaptiveTerm;
output = output-adaptCtrlRotK*shat;
 
%rotationalTorques[0] = output[0];
%rotationalTorques[1] = output[1];
%rotationalTorques[2] = output[2];
rotationalTorques=output;

