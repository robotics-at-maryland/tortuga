function y=slidingGoertzelDFT(x,k,N)
% Sliding Goertzel DFT (one multiplication more efficient than a
% regular sliding DFT
% Author: Scott Watson
% adapted from block diagram in figure 8 of the DSP Tips & Tricks
% column of the IEEE Signal Processing Magazine (March 2003 issue)
% which was written by Eric Jacobsen and Richard Lyons
%
% Input parameter definitions
% x - input signal
% k - bin of interest  ( 0 <= k < N )
% N - size in samples of time window
SGcount=0; %3.27 seconds before overflow for a 16bit uint for 20ksps
v=0;
mem(1:2)=0;
y(1:size(x,2))=0;

for n=1:size(x,2)
	v = x(n) + mem(1)*2*cos((2*pi*k)/N) - mem(2);
	if SGcount>N+1
		v = v - x(n-N);
	end
	y(n) = v - ( mem(1) * ( exp((i*2*pi*k)/N) ) );
	
	%incriment the delay line
	mem(2)=mem(1);
	mem(1)=v;
	
	%incriment buffer counter
	SGcount=SGcount+1;
end
