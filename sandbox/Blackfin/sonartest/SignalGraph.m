
function SignalGraph(Range,SamplingRate)


if( nargin < 2 )
    SamplingRate = 500000;
end


A = textread('/tmp/dataA.txt');
B = textread('/tmp/dataB.txt');
C = textread('/tmp/dataC.txt');
D = textread('/tmp/dataD.txt');
t = (0:1/SamplingRate:(length(A)-1)/SamplingRate);



if( nargin < 1 )
    Range = 1:length(t);
end


close all
figure
plot(t(Range),A(Range),'g-', ...
    t(Range),B(Range),'b-', ...
    t(Range),C(Range),'r-', ...
    t(Range),D(Range),'m-')

figure
subplot(2,2,1), plot(t(Range),A(Range),'g-')
subplot(2,2,2), plot(t(Range),B(Range),'b-')
subplot(2,2,3), plot(t(Range),C(Range),'r-')
subplot(2,2,4), plot(t(Range),D(Range),'m-')

figure
hold on
stem(t(Range),A(Range),'g')
stem(t(Range),B(Range),'b')
stem(t(Range),C(Range),'r')
stem(t(Range),D(Range),'m')

figure
subplot(2,2,1), stem(t(Range),A(Range),'g')
subplot(2,2,2), stem(t(Range),B(Range),'b')
subplot(2,2,3), stem(t(Range),C(Range),'r')
subplot(2,2,4), stem(t(Range),D(Range),'m')



end