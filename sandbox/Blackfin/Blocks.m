close all;

A = textread('/tmp/dataA.txt');
B = textread('/tmp/dataB.txt');
C = textread('/tmp/dataC.txt');
D = textread('/tmp/dataD.txt');


DA = A; %A-min(A);

Range=(1:length(A));

figure
plot((Range),A(Range),'g-', ...
    (Range),B(Range),'b-', ...
    (Range),C(Range),'r-', ...
    (Range),D(Range),'m-')


% Block size = 2x ping size
BL=2000; r=0; for x=1:length(DA)/BL; r(x) = max(DA(x*BL:min((x+1)*BL, length(DA)))); end;
r = abs(r');
minr = min(r);
r = (r / minr);


% Find where R increases the quickest
rRate = (r(2:length(r)) ./ r(1:length(r)-1));
imr = find(rRate == max(rRate));

blockIndex = imr+1;

pingStart = max(1, (blockIndex)*BL);
pingEnd = min(length(DA), (blockIndex+1)*BL);

hold on
plot(pingStart, A(pingStart), '.')
plot(pingEnd, A(pingEnd), '.')

Range = (pingStart:pingEnd);

figure
plot((Range),A(Range),'g-', ...
    (Range),B(Range),'b-', ...
    (Range),C(Range),'r-', ...
    (Range),D(Range),'m-')





DA = A(Range);


% Now do it all again, only this time find the quickest rate of increase
% within the current block

% use small blocks to walk the big one

% Block size = a few cycles of wave
BL=50; r=0; for x=1:length(DA)/BL; r(x) = max(DA(x*BL:min((x+1)*BL, length(DA)))); end;
r = abs(r');
minr = min(r);
r = (r / minr);
%blockIndex = find(r == max(r));

% Find where R increases the quickest
rRate = (r(2:length(r)) ./ r(1:length(r)-1));
imr = find(rRate == max(rRate));

blockIndex = imr+1;
x = (blockIndex*BL) + pingStart

hold on
Range = (x:x+BL*5);

plot(x, A(x), '.')
plot(max(Range), A(max(Range)), '.');




figure
plot((Range),A(Range),'g-', ...
    (Range),B(Range),'b-', ...
    (Range),C(Range),'r-', ...
    (Range),D(Range),'m-')




hold on
Range = (x+BL*5:x+BL*7);

figure
plot((Range),A(Range),'g-', ...
    (Range),B(Range),'b-', ...
    (Range),C(Range),'r-', ...
    (Range),D(Range),'m-')








%SignalGraph((pingStart:pingEnd))
