
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


%close all
figure
plot(t(Range),A(Range),'c-', ...
    t(Range),B(Range),'m-', ...
    t(Range),C(Range),'b-', ...
    t(Range),D(Range),'k-')

figure
subplot(2,2,1), plot(t(Range),A(Range),'c-')
subplot(2,2,2), plot(t(Range),B(Range),'m-')
subplot(2,2,3), plot(t(Range),C(Range),'b-')
subplot(2,2,4), plot(t(Range),D(Range),'k-')



end
