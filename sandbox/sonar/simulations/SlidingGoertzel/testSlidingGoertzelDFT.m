function testSlidingGoertzelDFT(N,precision)
count=1;
freqs=100:100:6000;
indexofmaxvalue(1:size(freqs,2))=0;
for freq=freqs
	x=sin(2*pi*(1:300)*(freq/20000));x(1:99)=0;x(201:300)=0;
	indexofmaxvalue(count)=slidingINTGoertzelDFTDemo(x,N,precision,freq);
	pause(.01)
	count=count+1;
end

figure
plot(freqs,indexofmaxvalue,'.')
xlabel('frequency (Hz)')
ylabel('k = this to detect')
title('A look at how many frequencies fall into each bin')
