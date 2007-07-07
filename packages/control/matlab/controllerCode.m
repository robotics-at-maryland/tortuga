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
storedValues = SimulateController(0);
PlotTestValues(storedValues,'b');

% Run cpp version
cppStoredValues = SimulateController(1);
PlotTestValues(cppStoredValues,'r');