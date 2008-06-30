A = textread('/tmp/dataA.txt');
B = textread('/tmp/dataB.txt');
C = textread('/tmp/dataC.txt');
D = textread('/tmp/dataD.txt');

DA = A;


Range = 1:length(A);

close all

figure
plot((Range),A(Range),'g-', ...
    (Range),B(Range),'b-', ...
    (Range),C(Range),'r-', ...
    (Range),D(Range),'m-')
title 'Max DFT trigger point and search starting point'

hold on
%d=0; for t=1:length(DA)-512; l = abs(fft(DA(t:t+512))); d(t)=l(27); end;
%md = max(d)
%id = find(d == md)


% Max DFT method.
% Again, maximum does not occur at the beginning of the ping. Next attempt-
% Max DFT with backtracking and mini-blocks.
backtrackSize = 2000;
d=0; for t=backtrackSize:length(DA)/8-512/8; l = abs(fft(DA(t*8:(t*8)+512))); d(t)=l(27); end; md = max(d);
id = find(d == md) * 8


%d=0; for t=1:length(DA)/512-1; l = abs(fft(DA(t*512:(t*512)+512))); d(t)=l(27); end;
%md = max(d)
%id = find(d == md) * 512



searchStart = id - backtrackSize;
triggerPoint = id;

plot(searchStart, DA(searchStart), '.')
plot(triggerPoint, DA(triggerPoint), '.')


Range = (searchStart:triggerPoint);


DA = DA(Range);

% Block size = a few cycles of wave
BL=50; r=0; for x=1:length(DA)/BL; r(x) = max(DA(x*BL:min((x+1)*BL, length(DA)))); end;
r = abs(r');
r = (r / min(r));

% Find where R increases the quickest
rRate = (r(2:length(r)) ./ r(1:length(r)-1));
imr = find(rRate == max(rRate));

blockIndex = imr+1;
x = (blockIndex*BL) + searchStart


figure
Range = (searchStart:triggerPoint);
plot((Range),A(Range),'g-', ...
    (Range),B(Range),'b-', ...
    (Range),C(Range),'r-', ...
    (Range),D(Range),'m-')
hold on
plot(x, A(x), '.')
title 'Ping edge trigger point'


figure
Range = (x:x+BL*6);
plot((Range),A(Range),'g-', ...
    (Range),B(Range),'b-', ...
    (Range),C(Range),'r-', ...
    (Range),D(Range),'m-')
title 'Detected ping beginning'