function PlotBinary(fname, skip)
% Plot sonar data stored in file fname, optionally skipping every skip samples

if nargin < 2
  skip = 1; % by default, don't skip any signals
end

% Read in the data
[A, B, C, D] = LoadBinary(fname);

% Construct indices
N = length(A);
fs = 500000;
Ts = 1/fs;
indices = 1:skip:N;
T = (indices-1)*Ts*1000;

subplot(2,2,1);plot(T,A(indices),'-c');xlabel('time (ms)');
subplot(2,2,2);plot(T,B(indices),'-m');xlabel('time (ms)');
subplot(2,2,3);plot(T,C(indices),'-b');xlabel('time (ms)');
subplot(2,2,4);plot(T,D(indices),'-k');xlabel('time (ms)');
end
