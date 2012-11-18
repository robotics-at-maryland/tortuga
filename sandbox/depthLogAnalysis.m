desiredDepth = data(:,16);
%desiredDepthRate = data(:,2);
%desiredDepthAccel = data(:,3);

estimatedDepth = data(:,8);
%estimatedDepthRate = data(:,5);

% pSig = data(:,12);
% dSig = data(:,13);
% iSig = data(:,14);
% aSig = data(:,15);
% wSig = data(:,16);

% controlSignalxn = data(:,17);
% controlSignalyn = data(:,18);
% controlSignalzn = data(:,19);
% 
% controlSignalxb = data(:,20);
% controlSignalyb = data(:,21);
% controlSignalzb = data(:,22);

time = data(:,end);

figure()
subplot(2,1,1)
plot(time, estimatedDepth - desiredDepth, 'b.',...
    time, pSig, 'r.');
% subplot(4,1,2)
% plot(time, estimatedDepthRate - desiredDepthRate, 'b.', ...
%     time, dSig, 'r.');
% subplot(4,1,3)
% plot(time, desiredDepthAccel, 'b.', ...
%      time, aSig / 20, 'r.')
subplot(2,1,2)
plot(time, controlSignalzb, 'b.',...
     time, wSig, 'r.');
 
%  figure()
%  plot(time, pSig, 'r.',...
%       time, dSig, 'g.',...
%       time, iSig, 'b.',...
%       time, aSig, 'k.',...
%       time, wSig, 'c.')