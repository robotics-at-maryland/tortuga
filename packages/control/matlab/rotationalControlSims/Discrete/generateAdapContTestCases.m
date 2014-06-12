%% sample TestCase 
clc
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
AdaptiveRotationalControllerTestCaseGenerator(dt,dtMin,dtMax,w,wd,q,qd,adaptCtrlRotLambda,adaptCtrlRotGamma,adaptCtrlRotK,adaptCtrlParams)
%% TestCase 1 
clc
dt = 0.2;%time step
dtMin = 0.01;%from vehicle sim
dtMax = 1;%from vehicle sim
w = [0 0 0]';%actual angular rate
wd = [0 0 0]';%desired angular rate
q = [0,0,0,1]';%actual position
qd = [0,0,0,1]';%desired position
adaptCtrlRotLambda = 1;
adaptCtrlRotGamma = 1;
adaptCtrlRotK = 1;
adaptCtrlParams = [0.5 0 -0.1 1 0 1 0 0 0 1 2 2]';
AdaptiveRotationalControllerTestCaseGenerator(dt,dtMin,dtMax,w,wd,q,qd,adaptCtrlRotLambda,adaptCtrlRotGamma,adaptCtrlRotK,adaptCtrlParams)
%% TestCase 2
clc
dt = 0.1;%time step
dtMin = 0.01;%from vehicle sim
dtMax = 1;%from vehicle sim
w = [0 0 0]';%actual angular rate
wd = [0 0 0]';%desired angular rate
q = [0,0,0,1]';%actual position
qd = [0,0,0.7071,0.7071]';%desired position
adaptCtrlRotLambda = 1;
adaptCtrlRotGamma = 1;
adaptCtrlRotK = 1;
adaptCtrlParams = [0.5 0 -0.1 1 0 1 0 0 0 1 2 2]';
AdaptiveRotationalControllerTestCaseGenerator(dt,dtMin,dtMax,w,wd,q,qd,adaptCtrlRotLambda,adaptCtrlRotGamma,adaptCtrlRotK,adaptCtrlParams)
%% TestCase 3
clc
dt = 0.3;%time step
dtMin = 0.01;%from vehicle sim
dtMax = 1;%from vehicle sim
w = [0 0 0]';%actual angular rate
wd = [0 0 0]';%desired angular rate
q = [0,0,0,1]';%actual position
qd = [0.7071,0,0,0.7071]';%desired position
adaptCtrlRotLambda = 1;
adaptCtrlRotGamma = 1;
adaptCtrlRotK = 1;
adaptCtrlParams = [0.5 0 -0.1 1 0 1 0 0 0 1 2 2]';
AdaptiveRotationalControllerTestCaseGenerator(dt,dtMin,dtMax,w,wd,q,qd,adaptCtrlRotLambda,adaptCtrlRotGamma,adaptCtrlRotK,adaptCtrlParams)
%% TestCase 4
clc
dt = 0.05;%time step
dtMin = 0.01;%from vehicle sim
dtMax = 1;%from vehicle sim
w = [0 0 0]';%actual angular rate
wd = [-0.2 0 0]';%desired angular rate
q = [0,0,0,1]';%actual position
qd = [0.7071,0,0,0.7071]';%desired position
adaptCtrlRotLambda = 1;
adaptCtrlRotGamma = 1;
adaptCtrlRotK = 1;
adaptCtrlParams = [0.5 0 -0.1 1 0 1 0 0 0 1 2 2]';
AdaptiveRotationalControllerTestCaseGenerator(dt,dtMin,dtMax,w,wd,q,qd,adaptCtrlRotLambda,adaptCtrlRotGamma,adaptCtrlRotK,adaptCtrlParams)
%% TestCase 5
clc
dt = 0.2;%time step
dtMin = 0.01;%from vehicle sim
dtMax = 1;%from vehicle sim
w = [0 0 0]';%actual angular rate
wd = [0 0 0]';%desired angular rate
q = [0,0,0,1]';%actual position
e = [1 1 1]';
et = pi/4;
e=e/norm(e);
qd=[e(1)*sin(et/2);
   e(2)*sin(et/2);
   e(3)*sin(et/2);
   cos(et/2)];
qd=qd/norm(qd);%desired position
adaptCtrlRotLambda = 1;
adaptCtrlRotGamma = 1;
adaptCtrlRotK = 1;
adaptCtrlParams = [0.5 0 -0.1 1 0 1 0 0 0 1 2 2]';
AdaptiveRotationalControllerTestCaseGenerator(dt,dtMin,dtMax,w,wd,q,qd,adaptCtrlRotLambda,adaptCtrlRotGamma,adaptCtrlRotK,adaptCtrlParams)
%% TestCase 6
clc
dt = 0.2;%time step
dtMin = 0.01;%from vehicle sim
dtMax = 1;%from vehicle sim
w = [0 0 0]';%actual angular rate
wd = [0 0 0]';%desired angular rate
q = [0,0,0,1]';%actual position
e = [1 1 0]';
et = pi/3;
e=e/norm(e);
qd=[e(1)*sin(et/2);
   e(2)*sin(et/2);
   e(3)*sin(et/2);
   cos(et/2)];
qd=qd/norm(qd);%desired position
adaptCtrlRotLambda = 1;
adaptCtrlRotGamma = 1;
adaptCtrlRotK = 1;
adaptCtrlParams = [0.5 0 -0.1 1 0 1 0 0 0 1 2 2]';
AdaptiveRotationalControllerTestCaseGenerator(dt,dtMin,dtMax,w,wd,q,qd,adaptCtrlRotLambda,adaptCtrlRotGamma,adaptCtrlRotK,adaptCtrlParams)
