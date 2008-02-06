%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% controllerCode.m is used to test the matlab implementation of the control
%code, iterates for a specified interval of time
%
% written by Joseph Gland
% June 22 2007
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Only does something on the first call
LoadCppController() 

% Run matlab version
[storedValues storedErrorQuaternion]= SimulateController(0);
PlotTestValues(storedValues,'b');

qErrorMag = zeros(1,length(storedValues.time));
for i=1:1:length(storedValues.time)
    qErrorMag(i) = sqrt(storedErrorQuaternion(1,i)^2+...
                     storedErrorQuaternion(2,i)^2+...
                     storedErrorQuaternion(3,i)^2);
end

%plot error quaternion (currently in matlab version only
figure(3)
subplot(4,1,1)
plot(storedValues.time,storedErrorQuaternion(1,:))
ylabel('q1')
subplot(4,1,2)
plot(storedValues.time,storedErrorQuaternion(2,:))
ylabel('q2')
subplot(4,1,3)
plot(storedValues.time,storedErrorQuaternion(3,:))
ylabel('q3')
subplot(4,1,4)
plot(storedValues.time,qErrorMag)
ylabel('qError')
xlabel('time')


% Run cpp version
cppStoredValues = SimulateController(1);
PlotTestValues(cppStoredValues,'r');