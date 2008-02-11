function phaseExp(delayBetweenPlots,NoiseLevel,loopLength,freq,ksps,N);
% PHASEXP A visualization of the phase properties of a
%	single frequency signal (for now)
%	This function illustrates the relationship between the 
%	positive and negative Fourier components of a purely 
%	real, signle frequency signal rotated through a phase
%	shift.  Demonstrates that atan(im(X_k)/re(X_k)) does not
%	provide enough information to uniquely determine the 
%	phase angle of the original signal in the range -pi:pi.
%	Fixes include computing and comparing both the 
%	negative and positive Fourier components or using the 
%	atan2 function found in C's <math.h> and other languages.
%   ...
%   The code has been modified to plot the phase after an fft
%   on a quantized and noisy-quantized signal.  I also explore
%   the error of the phase from the no noise double precision 
%   version of the same signal.  So far the code demonstrates that
%   about 11bit depth does not pose much of a problem for phase
%   determination in the frequency and sample rate ranges that we'll
%   be working with.  However, if we see noise levels similar to last
%   year then we will face significant difficulties.  Noise can be
%   tackled somewhat by sampling faster(if it is achievable).

clf;
number_of_samples=N;
signal_frequency=freq; %in kHz
sample_rate=ksps; %in kHz
%desiredBin1=5;
desiredBin1=1+round(N*(freq/ksps));
%desiredBin2=61;
k=0:number_of_samples-1;
phase1(1:number_of_samples)=0; %preallocating for speed
phase2(1:number_of_samples)=0; %preallocating for speed
phase3(1:number_of_samples)=0; %preallocating for speed

for loop=1:loopLength
	%create signal
	y_clean=sin((2.*pi.*k./(sample_rate/signal_frequency))+(loop/10));
	%quantize signal
	y_quant=round(y_clean.*1024);
	%add quantized noise to signal
	y_noisy=y_quant+round(randn(1,number_of_samples)*NoiseLevel);
	
	%fft and phase computations
	ffty_clean=fft(y_clean);
	ffty_quant=fft(y_quant);
	ffty_noisy=fft(y_noisy);
	imag1=imag(ffty_clean(desiredBin1));
	real1=real(ffty_clean(desiredBin1));
	imag2=imag(ffty_quant(desiredBin1));
	real2=real(ffty_quant(desiredBin1));
	imag3=imag(ffty_noisy(desiredBin1));
	real3=real(ffty_noisy(desiredBin1));
	phase1(loop)=(180/pi)*atan2(imag1,real1);
	phase2(loop)=(180/pi)*atan2(imag2,real2);
	phase3(loop)=(180/pi)*atan2(imag3,real3);
	
	%plot signal on top left plot 
	subplot(2,2,1); hold off; 
	plot(k,1024.*y_clean,'.-b'); hold on;
	plot(k,y_quant,'og');
	plot(k,y_noisy,'x-r');
	axis([0 number_of_samples -1100 1100])
	title(['input signal: N:' num2str(number_of_samples) ' frequency:' num2str(signal_frequency) 'kHz sampleRate:' num2str(sample_rate) 'ksps'])
	
	%plot FFT result on left bottom plot
	subplot(2,2,3); hold off;
	plot(k,real(ffty_clean),'or'); hold on;
	plot(k,imag(ffty_clean),'.r');
	axis([0 number_of_samples -35 35])
	title(['FFT of input signal(circles:real dots:imag) desired bin:' num2str(desiredBin1)])
	
	%plot phase over time on the right side plot
	subplot(2,2,2); hold off;
	plot(1:loop,phase1(1:loop),'.b');hold on; 
	plot(1:loop,phase2(1:loop),'og');
	plot(1:loop,phase3(1:loop),'xr');
	axis([1 loopLength -180 180])
	title('different attempts to compute phase') %in time
	
	%plot of phase error in lower right
	subplot(2,2,4);hold off;
	plot(1:loop,phase1(1:loop)-phase2(1:loop),'.g');hold on; 
	noise_error=phase1(1:loop)-phase3(1:loop);
	plot(1:loop,noise_error,'or');
	stderror=std(noise_error);
	maxplot=stderror;
	if maxplot<.01; maxplot=.05; end
	axis([1 loopLength -maxplot maxplot])
	title(['estimated phase error, std.dev:' num2str(stderror)]) %in time
	
	%delay before plotting next iteration
	pause(delayBetweenPlots)
end
