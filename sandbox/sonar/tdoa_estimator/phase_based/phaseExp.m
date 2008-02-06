function phaseExp(delayBetweenPlots,inverseNoiseLevel,loopLength);
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

clf;
number_of_samples=64;
signal_frequency=30; %in kHz
sample_rate=500; %in kHz
desiredBin1=5;
desiredBin2=61;

for loop=1:loopLength
	%create signal
	k=0:number_of_samples-1;
	y=sin((2.*pi.*k./(sample_rate/signal_frequency))+(loop/10));
	y=y+(randn(1,number_of_samples)/inverseNoiseLevel);
	
	%fft and phase computations
	ffty=fft(y);
	imag1=imag(ffty(desiredBin1));
	real1=real(ffty(desiredBin1));
	imag2=imag(ffty(desiredBin2));
	real2=real(ffty(desiredBin2));
	phase1(loop)=(180/pi)*atan2(imag1,real1);
	phase2(loop)=(180/pi)*atan2(imag2,real2);
	phaseA(loop)=(180/pi)*atan(imag1/real1);
	phaseB(loop)=(180/pi)*atan(imag2/real2);
	
	%plot signal with FFT result on left plot
	subplot(1,2,1); hold off; plot(k,5.*y,'.-'); hold on;
	plot(k,real(ffty),'or'); plot(k,imag(ffty),'.r');
	axis([0 64 -35 35])
	title('input signal:blue, FFT:red(circles:real dots:imag')

	%plot phase over time on the right side plot
	subplot(1,2,2); hold on;
	plot(1:loop,phaseA,'b.'); 
	plot(1:loop,phaseB,'r.');
	plot(1:loop,phase1,'ko');
	plot(1:loop,phase2,'go');
	plot(1:loop,(phase1-phase2)/2,'c^')
	axis([1 loopLength -180 180])
	title('different attempts to compute phase') %in time
	
	%delay before plotting next iteration
	pause(delayBetweenPlots)
end
