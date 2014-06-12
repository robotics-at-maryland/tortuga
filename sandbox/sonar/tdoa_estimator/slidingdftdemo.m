samprate = 1000000;
targetfreq = 25000;
sampduration = 0.003;
pingdelay = 0.0005;
pingduration = 0.0013;

t = 0:(1/samprate):sampduration;
samp = sin(2*pi*targetfreq*t) ...
       .* (t > pingdelay & (t < pingduration + pingdelay)) ...
       + rand(1,length(t))*3;

subplot(2,1,1);
plot(t,samp);
title("Signal");
xlabel("Time (seconds)");
ylabel("Signal (Volts)");

subplot(2,1,2);
title("Sliding DFT");
plot(t,slidingdft(samp,targetfreq,samprate));
xlabel("Time (seconds)");
ylabel("Sliding Fourier amplitude");
