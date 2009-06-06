function indexofmaxvalue=slidingINTGoertzelDFTDemo(x,N,precision,freq)
% Sliding Goertzel DFT Demo {see slidingGoertzelDFT()}
% Author: Scott Watson

magdata(1:size(x,2),1:N)=0;
phasedata(1:size(x,2),1:N)=0;

for k=1:N-1
	result=slidingINTGoertzelDFT(x,k,N,precision);
	magdata(:,k)=abs(real(result))+abs(imag(result));
	phasedata(:,k)=atan2(imag(result),real(result));
end

[maxvalue,indexofmaxvalue] = max(max(magdata( : , 1:ceil(N/2) )));

subplot(1,2,1)
surf(magdata(:,1:ceil(N/2)))
xlabel('k')
ylabel('N')
zlabel('Amplitude')
title(['signal power at any given time or frequency bin, max = ' num2str(indexofmaxvalue)])

% subplot(1,2,2)
% k=4;
% plot((phasedata(:,k))*(180/pi))
% title(['phase of bin #' num2str(k)])
% xlabel('N')
% ylabel('phase')

subplot(1,2,2)
plot(x)
title(['input signal at freq ' num2str(freq) 'Hz'])
xlabel('sample')
ylabel('amplitude')