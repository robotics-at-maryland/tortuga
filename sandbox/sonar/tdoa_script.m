function tdoa_script(tdoa1,tdoa2,tdoa3)


%clear all;

sampfreq=500000;
signalfreq=25000;
speedofsound=1500;
numchan=4;
numsamp=2000;
numsamples=42*20;
phase(1)=signalfreq*tdoa1/sampfreq*2*pi();
phase(2)=signalfreq*tdoa2/sampfreq*2*pi();
phase(3)=signalfreq*tdoa3/sampfreq*2*pi();
colors=['c','m','b','k'];

figure(1);
hold on;
points=1:numsamp;

sine=zeros(numchan-1,numsamp);
for k=1:numchan-1
    %sine(k,:)=sin(signalfreq/sampfreq*2*pi*points*phase(k)
    for m=1:numsamp
        sine(k,m)=sin(signalfreq/sampfreq*2*pi()*points(m)+phase(k));
    end
    plot(points, sine(k,:),colors(k));
end

axis([0 numsamp -3 3]);


